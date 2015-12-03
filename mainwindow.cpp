#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "coordscontroller.h"
#include "calcscontroller.h"
#include "stationscontroller.h"
#include "coordstab.h"
#include "stationstab.h"
#include "calcstab.h"
#include "plantab.h"
#include "utils.h"
#include "Types/coord.h"
#include "Types/occupied.h"
#include "Types/calc.h"
#include "Types/desc.h"
#include "Dialogs/coordclassdlg.h"
#include "Dialogs/tabledlg.h"
#include <QFileDialog>
#include <QDebug>
#include <QErrorMessage>
#include <QMessageBox>
#include <QSettings>
#include <functional>

#define VERSION "v0.1"
#define UNTITLED "<Untitled>"
#define READY "Ready"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	m_db = QSqlDatabase::addDatabase("QSQLITE");

	m_pCoordsController = new CoordsController(this);
	m_pCalcsController = new CalcsController(this);
	m_pStationsController = new StationsController(this);

	m_pCoordsTab = new CoordsTab(*m_pCoordsController, this);
	m_pStationsTab = new StationsTab(*m_pStationsController, this);
	m_pCalcsTab = new CalcsTab(*m_pCalcsController, this);
	m_pPlanTab = new PlanTab(this);
	m_statusLabel = new QLabel(READY, this);

	statusBar()->addWidget(m_statusLabel);

	ui->w_coordsLayout->addWidget(m_pCoordsTab);
	ui->w_stationsLayout->addWidget(m_pStationsTab);
	ui->w_calcsLayout->addWidget(m_pCalcsTab);
	ui->w_planLayout->addWidget(m_pPlanTab);

	ui->w_tabs->setIconSize(QSize(24, 24));
	ui->w_tabs->setTabIcon(0, QIcon(":/axes-32.png"));
	ui->w_tabs->setTabIcon(1, QIcon(":/calculator-32.png"));
	ui->w_tabs->setTabIcon(2, QIcon(":/total-station-24.png"));
	ui->w_tabs->setTabIcon(3, QIcon(":/pin-map-32.png"));

	SetCaption();

	connect(this, SIGNAL(databaseChanged()), this, SLOT(onChangedDatabase()));
	connect(this, SIGNAL(databaseChanged()), m_pCoordsTab, SLOT(onLoad()));
	connect(this, SIGNAL(databaseChanged()), m_pStationsTab, SLOT(onLoad()));
	connect(this, SIGNAL(databaseChanged()), m_pCalcsTab, SLOT(onLoad()));
	connect(this, SIGNAL(databaseChanged()), m_pPlanTab, SLOT(onLoad()));

	connect(this, SIGNAL(databaseClosed()), this, SLOT(onCloseDatabase()));
	connect(this, SIGNAL(databaseClosed()), m_pCoordsTab, SLOT(onClear()));
	connect(this, SIGNAL(databaseClosed()), m_pStationsTab, SLOT(onClear()));
	connect(this, SIGNAL(databaseClosed()), m_pCalcsTab, SLOT(onClear()));
	connect(this, SIGNAL(databaseClosed()), m_pPlanTab, SLOT(onClear()));

	connect(this, SIGNAL(updatePlanView()), m_pPlanTab, SLOT(onLoad()));
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_actionFileOpen_triggered()
{
	auto filename = QFileDialog::getOpenFileName(this,
												 tr("Open File"), "", tr("sqlite3 database (*.sqlite)"));

	if (!filename.isEmpty())
	{
		emit databaseClosed();

		QString backupName = Utils::MakeWorkingCopy(filename);
		if (!backupName.isEmpty())
		{
			m_db.setDatabaseName(backupName);
			if (m_db.open())
			{
				m_filenames = qMakePair(filename, backupName);

				emit databaseChanged();

				on_w_tabs_currentChanged(ui->w_tabs->currentIndex());
			}
			else
			{
				QErrorMessage msg(this);
				msg.showMessage(QString("Could not open database %1").arg(backupName));
				m_db.setDatabaseName("");
			}
		}
		else
		{
			QString msg = QString("Unable to create working database %1").arg(backupName);
			QMessageBox::critical(this, "Error", msg, QMessageBox::Ok);
		}
	}
}

void MainWindow::on_actionFileClose_triggered()
{
	emit databaseClosed();
	on_w_tabs_currentChanged(ui->w_tabs->currentIndex());
}

void MainWindow::on_actionFileExit_triggered()
{
	emit databaseClosed();
	QApplication::quit();
}

void MainWindow::writePositionSettings()
{
	QSettings qsettings( "Alasdair Craig", "Survey Calcs" );

	qsettings.beginGroup( "MainWindow" );

	qsettings.setValue( "Geometry", saveGeometry() );
	qsettings.setValue( "SaveState", saveState() );
	qsettings.setValue( "Maximised", isMaximized() );
	if ( !isMaximized() ) {
		qsettings.setValue( "Pos", pos() );
		qsettings.setValue( "Size", size() );
	}

	qsettings.endGroup();
}

void MainWindow::readPositionSettings()
{
	QSettings qsettings( "Alasdair Craig", "Survey Calcs" );

	qsettings.beginGroup( "MainWindow" );

	restoreGeometry(qsettings.value( "Geometry", saveGeometry() ).toByteArray());
	restoreState(qsettings.value( "SaveState", saveState() ).toByteArray());
	move(qsettings.value( "Pos", pos() ).toPoint());
	resize(qsettings.value( "Size", size() ).toSize());
	if ( qsettings.value( "Maximised", isMaximized() ).toBool() )
		showMaximized();

	qsettings.endGroup();
}

void MainWindow::closeEvent( QCloseEvent* )
{
	emit databaseClosed();
	writePositionSettings();
}

void MainWindow::onPlanViewChanged()
{
	emit updatePlanView();
}

void MainWindow::onStatusTextChanged()
{
	on_w_tabs_currentChanged(ui->w_tabs->currentIndex());
}

void MainWindow::on_w_tabs_currentChanged(int index)
{
	if (m_db.isOpen())
	{
		switch (index)
		{
		case 0:
			m_statusLabel->setText(m_pCoordsTab->GetStatus());
			break;
		case 1:
			m_statusLabel->setText(m_pCalcsTab->GetStatus());
			break;
		case 2:
			m_statusLabel->setText(m_pStationsTab->GetStatus());
			break;
		case 3:
			m_statusLabel->setText(m_pPlanTab->GetStatus());
			break;
		}
	}
	else
	{
		m_statusLabel->setText(READY);
	}
}

void MainWindow::onCloseDatabase()
{
	if (m_db.isOpen())
	{
		m_db.close();

		auto original = m_filenames.first;
		auto backup = m_filenames.second;

		QString text = "Save changes to database?";
		if (QMessageBox::Save == QMessageBox::question(this, "Confirm", text, QMessageBox::Save|QMessageBox::Discard, QMessageBox::Discard))
		{
			if (original == UNTITLED)
			{
				original = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("sqlite3 database (*.sqlite)"));
			}

			if (!Utils::CopyAndOverwrite(backup, original))
			{
				QString msg = QString("Unable to save database %1").arg(original);
				QMessageBox::critical(this, "Error", msg, QMessageBox::Ok);
			}
		}

		QFile::remove(backup);

		m_filenames = qMakePair(QString(), QString());

		SetCaption();
	}
}

void MainWindow::onChangedDatabase()
{
	SetCaption();
}

void MainWindow::SetCaption()
{
	QString filename = m_filenames.first;
	if (filename != UNTITLED)
	{
		QString path = m_filenames.first;
		QStringList parts = QDir::toNativeSeparators(path).split(QDir::separator());
		filename = parts.last();
	}

	QString title = QString("Survey Calcs ") + VERSION;
	if (!filename.isEmpty())
		title += " - " + filename;

	setWindowTitle(title);
}

void MainWindow::on_actionCoordClass_triggered()
{
	CoordClassDlg dlg(this);
	dlg.exec();
}

void MainWindow::on_actionCoordDescr_triggered()
{
	Desc desc;
	QString query = "SELECT desc, description FROM desc ORDER BY desc";

	QSqlDatabase db = QSqlDatabase::database();
	db.transaction();

	TableDlg<Desc, QSqlQueryModel> dlg(desc, "Descriptions", query, this);
	dlg.exec();

	if (dlg.exec() == QDialog::Accepted)
		db.commit();
	else
		db.rollback();
}

void MainWindow::on_actionFileNew_triggered()
{
	emit databaseClosed();

	QString backupName = Utils::MakeTemporaryFile();

	m_db.setDatabaseName(backupName);
	if (m_db.open())
	{
		CreateRequiredTables();

		m_filenames = qMakePair(QString(UNTITLED), backupName);

		emit databaseChanged();

		on_w_tabs_currentChanged(ui->w_tabs->currentIndex());
	}
	else
	{
		QErrorMessage msg(this);
		msg.showMessage("Could not make a new database");
		m_db.setDatabaseName("");
	}

}

template <typename T>
void CreateTable()
{
	Utils::ExecQuery(T::SqlCreateQuery);
}

void MainWindow::CreateRequiredTables()
{
	CreateTable<Coord>();
	CreateTable<Occupied>();
	CreateTable<Calc>();
	CreateTable<Desc>();
}
