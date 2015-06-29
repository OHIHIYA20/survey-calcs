#include "stationstab.h"
#include "ui_stationstab.h"
#include "stationsquerymodel.h"
#include "QtSql/QSqlDatabase"
#include "QtSql/QSqlQuery"
#include "QtSql/QSqlError"
#include "QDebug"

StationsTab::StationsTab(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::StationsTab)
{
	ui->setupUi(this);
	m_pModel = new StationsQueryModel(this);
}

StationsTab::~StationsTab()
{
	delete ui;
}

void StationsTab::onClear()
{
	m_pModel->clear();
	ui->w_tableView->setModel(nullptr);
}

void StationsTab::onLoad()
{
	QSqlDatabase db = QSqlDatabase::database();
	if (db.isOpen())
	{
		QSqlQuery query(db);
		query.prepare("SELECT station AS Name, setupno AS Setup, insthgt AS `Instr. hgt`, date AS Date FROM occupied ORDER BY station, setupno");
		query.exec();

		m_pModel->setQuery(query);
		if (m_pModel->lastError().isValid())
			qDebug() << m_pModel->lastError();

		ui->w_tableView->setModel(m_pModel);
		ui->w_tableView->show();
	}
}

void StationsTab::on_w_loadButton_clicked()
{
	onLoad();
}
