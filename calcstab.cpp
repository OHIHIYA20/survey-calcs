#include "calcstab.h"
#include "ui_calcstab.h"
#include "calc.h"
#include "calcfactory.h"
#include "calcslistmodel.h"
#include "calcscontroller.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>

CalcsTab::CalcsTab(CalcsController &calcsController, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CalcsTab),
	m_calcsController(calcsController)
{
	ui->setupUi(this);

	m_pModel = new CalcsListModel(this);
}

CalcsTab::~CalcsTab()
{
	delete ui;
}

QString CalcsTab::GetStatus() const
{
	int rowCount = m_pModel->rowCount();
	return QString("%1 calculations").arg(rowCount);
}

void CalcsTab::onClear()
{
	qDebug() << "in CalcsTab::onClear()";
	m_pModel->clear();
}

void CalcsTab::onLoad()
{
//	qDebug() << "in CalcsTab::onLoad()";
//	onClear();

//	QSqlDatabase db = QSqlDatabase::database();
//	if (db.isOpen())
//	{
//		QSqlQuery query(db);
//		query.prepare("SELECT type FROM calcs ORDER BY `order`");
//		query.exec();

//		QStringList descList;
//		while (query.next())
//		{
//			int calcType = query.value("type").toInt();
//			auto pCalc = CalcFactory::Instance(calcType);
//			descList.push_back(pCalc->desc());
//			delete pCalc;
//		}

//		m_pModel->addDesc(descList);
//		ui->w_listView->setModel(m_pModel);
//		ui->w_listView->show();
//	}
	QStringList descList = m_calcsController.GetDescriptions();
	m_pModel->addDesc(descList);
	ui->w_listView->setModel(m_pModel);
	ui->w_listView->show();
}

void CalcsTab::on_w_loadButton_clicked()
{
	onLoad();
}
