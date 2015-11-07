#ifndef CALC_H
#define CALC_H

#include <QString>
#include <QSqlRecord>
#include <memory>

class Calc
{
	using CalcPtr = std::unique_ptr<Calc>;

protected:
	int m_calcref = 0;

public:
	Calc();

	virtual ~Calc() = default;
	virtual QString desc() const = 0;
	virtual bool Edit() { return false; };

	static bool SortFunc(CalcPtr const& a, CalcPtr const& b);
};


class UnknownCalc : public Calc
{
public:
	virtual QString desc() const override { return "Unknown\n\tMy new line"; }
};

class BeaconsCalc : public Calc
{
public:
	virtual QString desc() const override { return "Beacon\n\tMy new line"; }
};

class CommentCalc : public Calc
{
public:
	virtual QString desc() const override { return "Comment\n\tMy new line"; }
};

class DpObsCalc : public Calc
{
public:
	virtual QString desc() const override { return "Double Polar\n\tMy new line"; }
};

class DpStnsCalc : public Calc
{
public:
	virtual QString desc() const override { return "DpStns\n\tMy new line"; }
};

class DupsCalc : public Calc
{
public:
	virtual QString desc() const override { return "Dups\n\tMy new line"; }
};

class HelmertCalc : public Calc
{
public:
	virtual QString desc() const override { return "Helmert\n\tMy new line"; }
};

class IntersCalc : public Calc
{
public:
	virtual QString desc() const override { return "Intersection\n\tMy new line"; }
};

class JoinsCalc : public Calc
{
	QString m_fromname;
	QString m_toname;
	double m_fy = 0, m_fx = 0;
	double m_ty = 0, m_tx = 0;
	double m_dirc = 0, m_dist = 0;

public:
	JoinsCalc() = default;
	explicit JoinsCalc(const QSqlRecord &record);
	static const QString SqlSelectQuery;
	virtual QString desc() const override;
	virtual bool Edit() override;
};

class OffsetLnCalc : public Calc
{
public:
	virtual QString desc() const override { return "Offset Line\n\tMy new line"; }
};

class OffsetsCalc : public Calc
{
public:
	virtual QString desc() const override { return "Offsets\n\tMy new line"; }
};

class PolarsCalc : public Calc
{
	QString m_fromname;
	QString m_toname;
	double m_fy = 0, m_fx = 0;
	double m_ty = 0, m_tx = 0;
	double m_dirc = 0, m_dist = 0, m_oc = 0;

public:
	PolarsCalc() = default;
	explicit PolarsCalc(const QSqlRecord &record);
	static const QString SqlSelectQuery;
	virtual QString desc() const override;
};

class ResecsCalc : public Calc
{
public:
	virtual QString desc() const override { return "Resection\n\tMy new line"; }
};

class RoadWCalc : public Calc
{
public:
	virtual QString desc() const override { return "Road width\n\tMy new line"; }
};

class RouteCalc : public Calc
{
public:
	virtual QString desc() const override { return "Route\n\tMy new line"; }
};

class RPolarsCalc : public Calc
{
public:
	virtual QString desc() const override { return "Reverse Polar\n\tMy new line"; }
};

class SpObsCalc : public Calc
{
public:
	virtual QString desc() const override { return "Batch Polars\n\tMy new line"; }
};

class SpStnsCalc : public Calc
{
public:
	virtual QString desc() const override { return "SpStns\n\tMy new line"; }
};

class TObsCalc : public Calc
{
public:
	virtual QString desc() const override { return "TObs\n\tMy new line"; }
};

class XFormCalc : public Calc
{
public:
	virtual QString desc() const override { return "XForm\n\tMy new line"; }
};

#endif // CALC_H
