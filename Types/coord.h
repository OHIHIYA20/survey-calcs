#ifndef COORD_H
#define COORD_H

#include <QSqlRecord>

class Coord
{
public:
	Coord() = default;
	explicit Coord(const QSqlRecord &record);

public:
	QString m_name;
	QString m_class;
	QString m_desc;
	double m_easting = 0.0;
	double m_northing = 0.0;
	double m_elevation = 0.0;

	QString GetUpdateQueryString(const QString &oldName);
	static const QString SqlCreateQuery;
};

#endif // COORD_H
