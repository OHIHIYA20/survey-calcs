#include "plantab.h"
#include "ui_plantab.h"
#include "QSqlDatabase"
#include "QSqlQuery"
#include "QDebug"
#include <QVector>

PlanTab::PlanTab(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PlanTab)
{
	ui->setupUi(this);
}

PlanTab::~PlanTab()
{
	delete ui;
}

QString PlanTab::GetStatus() const
{
	return QString("View ready");
}

void PlanTab::onClear()
{
}

void PlanTab::onLoad()
{
	QSqlDatabase db = QSqlDatabase::database();
	if (db.isOpen())
	{
		QString whereFilter = "WHERE class IN ('WS', 'LAND', 'BP', 'BF')";

		QSqlQuery query1(db);
		query1.prepare(QString("SELECT MIN(y) AS lowEasting, MAX(y) AS highEasting, MIN(x) AS lowNorthing, MAX(x) AS highNorthing, AVG(y) AS midEasting, AVG(x) AS midNorthing FROM coord %1").arg(whereFilter));
		query1.exec();

		if (query1.first())
		{
			QRectF bounds;
			bounds.setTop(query1.value("highNorthing").toReal());
			bounds.setLeft(query1.value("lowEasting").toReal());
			bounds.setRight(query1.value("highEasting").toReal());
			bounds.setBottom(query1.value("lowNorthing").toReal());

			QPointF center;
			center.setX(query1.value("midEasting").toReal());
			center.setY(query1.value("midNorthing").toReal());

			ui->w_plotWidget->SetOrtho(bounds, center);
		}

		QSqlQuery query2(db);
		query2.prepare(QString("SELECT y AS easting, x AS northing FROM coord %1").arg(whereFilter));
		query2.exec();

		QVector<QPointF> points;

		while (query2.next())
		{
			QPointF p;
			p.setX(query2.value("easting").toReal());
			p.setY(query2.value("northing").toReal());
			points.push_back(p);
		}

		ui->w_plotWidget->SetPointList(points);
		qDebug() << points.size() << " points copied to plotWindow";
	}

}

void PlanTab::on_w_loadButton_clicked()
{
	onLoad();
}
