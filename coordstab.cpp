#include "coordstab.h"
#include "ui_coordstab.h"
#include "coordquerymodel.h"
#include "QtSql/QSqlDatabase"
#include "QtSql/QSqlQuery"
#include "QtSql/QSqlError"
#include "QDebug"

CoordsTab::CoordsTab(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CoordsTab)
{
	ui->setupUi(this);
	m_pModel = new CoordQueryModel(this);
}


CoordsTab::~CoordsTab()
{
	delete ui;
}

void CoordsTab::onClear()
{
	m_pModel->clear();
	ui->w_tableView->setModel(nullptr);
}

void CoordsTab::onLoad()
{
	QSqlDatabase db = QSqlDatabase::database();
	if (db.isOpen())
	{
		QSqlQuery query(db);
		query.prepare("SELECT name AS Name, y AS Y, x AS X, h AS H, desc AS Desc FROM coord ORDER BY class, name");
		query.exec();

		m_pModel->setQuery(query);
		if (m_pModel->lastError().isValid())
			qDebug() << m_pModel->lastError();

		ui->w_tableView->setModel(m_pModel);
		ui->w_tableView->show();
	}
}

void CoordsTab::on_w_loadButton_clicked()
{
	onLoad();
}
