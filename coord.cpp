#include "coord.h"
#include <QtCore/QVariant>

Coord::Coord()
{
}

Coord::Coord(const QSqlRecord &record)
{
	m_name = record.value("name").toString();
	m_class = record.value("class").toString();
	m_desc = record.value("desc").toString();
	m_easting = record.value("y").toDouble();
	m_northing = record.value("x").toDouble();
	m_elevation = record.value("h").toDouble();
}

QString Coord::GetUpdateQueryString(const QString &oldName)
{
	return QString("UPDATE coord SET `name`='%1', `y`=%2, `x`=%3, `h`=%4, `class`='%5', `desc`='%6' WHERE `name`='%7'")
			.arg(m_name)
			.arg(QString::number(m_easting, 'f', 6))
			.arg(QString::number(m_northing, 'f', 6))
			.arg(QString::number(m_elevation, 'f', 6))
			.arg(m_class)
			.arg(m_desc)
			.arg(oldName);
}
