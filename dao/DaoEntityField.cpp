#include "DaoEntityField.h"
#include <qdatetime.h>

DaoEntityField DaoEntityField::operator,(const DaoEntityField& f) {
	if (f.kvPairStr.isEmpty()) {
		return *this;
	}
	kvPairStr.append(',').append(f.kvPairStr);
	vlist.append(f.vlist);
	return *this;
}

DaoEntityField DaoEntityField::operator&&(const DaoEntityField& f) {
	if (f.kvPairStr.isEmpty()) {
		return *this;
	}
	if (!kvPairStr.isEmpty()) {
		kvPairStr.append(" and").append(' ');
	}
	kvPairStr.append(f.kvPairStr);
	vlist.append(f.vlist);
	return *this;
}

DaoEntityField DaoEntityField::operator&&(const DaoEntityFunction& f) {
	if (!kvPairStr.isEmpty()) {
		kvPairStr.append(' ');
	}
	kvPairStr.append(f.functionStr);
	return *this;
}

DaoEntityField DaoEntityField::operator||(const DaoEntityField& f) {
	if (f.kvPairStr.isEmpty()) {
		return *this;
	}
	if (!kvPairStr.isEmpty()) {
		kvPairStr.append(" or").append(' ');
	}
	kvPairStr.append(f.kvPairStr);
	vlist.append(f.vlist);
	return *this;
}

DaoEntityField DaoEntityField::operator||(const DaoEntityFunction& f) {
	if (!kvPairStr.isEmpty()) {
		kvPairStr.append(' ');
	}
	kvPairStr.append(f.functionStr);
	return *this;
}

DaoEntityField DaoEntityField::operator*() {
	if (kvPairStr.isEmpty()) {
		return *this;
	}
	kvPairStr = '(' + kvPairStr + ')';
	return *this;
}

DaoEntityField DaoEntityField::in(const QVariantList& v) {
	clear();
	kvPairStr.append(name()).append(" in (").append(QString("?,").repeated(v.size()));
	kvPairStr = kvPairStr.left(kvPairStr.length() - 1);
	kvPairStr.append(')');
	vlist.append(v);
	return *this;
}

DaoEntityField DaoEntityField::between(const QVariant& a, const QVariant& b) {
	clear();
	kvPairStr.append(name()).append(" between ? and ?");
	vlist.append(a);
	vlist.append(b);
	return *this;
}

DaoEntityField DaoEntityField::between(const DaoEntityFunction& a, const DaoEntityFunction& b) {
	clear();
	kvPairStr.append(name()).append(" between ").append(a.functionStr).append(" and ").append(b.functionStr);
	return *this;
}


DaoEntityField DaoEntityField::max() {
	return DaoEntityFunction().function("max(%1)", *this).as();
}

DaoEntityField DaoEntityField::min() {
	return DaoEntityFunction().function("min(%1)", *this).as();
}

DaoEntityField DaoEntityField::avg(const char* key) {
	return DaoEntityFunction().function("avg(%1)", *this).as(key);
}

DaoEntityField DaoEntityField::betweenDay2DayEndV1(qint64 st, qint64 et, bool useMsec) {
	return between(DaoEntityFunction().function(QString("strftime('%s', date(datetime(%1, 'unixepoch'), 'start of day'), 'utc')%2")
		.arg(QString::number(useMsec ? st / 1000 : st), useMsec ? " * 1000" : "")),
		DaoEntityFunction().function(QString("(strftime('%s', datetime(date(datetime(%1, 'unixepoch'), 'start of day', '+24 hours'), 'utc')%2 - 1)")
			.arg(QString::number(useMsec ? et / 1000 : et), useMsec ? " * 1000" : "")));
}

DaoEntityField DaoEntityField::betweenDay2DayEndV2(qint64 st, qint64 et, bool useMsec) {
	return between(DaoEntityFunction().function(QString("unix_timestamp(date(from_unixtime(%1)))%2")
		.arg(QString::number(useMsec ? st / 1000 : st), useMsec ? " * 1000" : "")),
		DaoEntityFunction().function(QString("(unix_timestamp(date_add(date(from_unixtime(%1)), interval 1 day))%2 - 1)")
			.arg(QString::number(useMsec ? et / 1000 : et), useMsec ? " * 1000" : "")));
}

DaoEntityField DaoEntityField::betweenDay2DayEndV3(qint64 st, qint64 et, bool useMsec) {
	QString startStr, endStr;
	if (useMsec) {
		startStr = QString("cast(datediff(ss, '1970-01-01 08:00:00', convert(varchar(10),dateadd(s,%1,'1970-01-01 08:00:00'),120)) as bigint) * 1000")
			.arg(QString::number(st / 1000));
		endStr = QString("(cast(datediff(ss, '1970-01-01 08:00:00', convert(varchar(10),dateadd(day, 1, dateadd(s,%1,'1970-01-01 08:00:00')),120)) as bigint) * 1000 - 1)")
			.arg(QString::number(et / 1000));
	} else {
		startStr = QString("datediff(ss, '1970-01-01 08:00:00', convert(varchar(10),dateadd(s,%1,'1970-01-01 08:00:00'),120))")
			.arg(QString::number(st));
		endStr = QString("(datediff(ss, '1970-01-01 08:00:00', convert(varchar(10),dateadd(day, 1, dateadd(s,%1,'1970-01-01 08:00:00')),120)) - 1)")
			.arg(QString::number(et));
	}
	return between(DaoEntityFunction().function(startStr), DaoEntityFunction().function(endStr));
}

DaoEntityField DaoEntityField::betweenCurDayV1(bool useMsec) {
	auto ct = useMsec ? QDateTime::currentMSecsSinceEpoch() : QDateTime::currentSecsSinceEpoch();
	return betweenDay2DayEndV1(ct, ct, useMsec);
}

DaoEntityField DaoEntityField::betweenCurDayV2(bool useMsec) {
	auto ct = useMsec ? QDateTime::currentMSecsSinceEpoch() : QDateTime::currentSecsSinceEpoch();
	return betweenDay2DayEndV2(ct, ct, useMsec);
}

DaoEntityField DaoEntityField::betweenCurDayV3(bool useMsec) {
	auto ct = useMsec ? QDateTime::currentMSecsSinceEpoch() : QDateTime::currentSecsSinceEpoch();
	return betweenDay2DayEndV3(ct, ct, useMsec);
}

DaoEntityField DaoEntityField::betweenMonth2MonthEndV1(qint64 st, qint64 et, bool useMsec) {
	return between(DaoEntityFunction().function(QString("strftime('%s', date(datetime(%1, 'unixepoch'), 'start of month'), 'utc')%2")
		.arg(QString::number(useMsec ? st / 1000 : st), useMsec ? " * 1000" : "")),
		DaoEntityFunction().function(QString("(strftime('%s', datetime(date(datetime(%1, 'unixepoch'), 'start of month', '+1 month'), 'utc')%2 - 1)")
			.arg(QString::number(useMsec ? et / 1000 : et), useMsec ? " * 1000" : "")));
}

DaoEntityField DaoEntityField::betweenMonth2MonthEndV2(qint64 st, qint64 et, bool useMsec) {
	return between(DaoEntityFunction().function(QString("unix_timestamp(date_sub(last_day(date(from_unixtime(%1))), interval day(last_day(date(from_unixtime(%1)))) - 1 day))%2")
		.arg(QString::number(useMsec ? st / 1000 : st), useMsec ? " * 1000" : "")),
		DaoEntityFunction().function(QString("(unix_timestamp(date_add(last_day(date(from_unixtime(%1))), interval 1 day))%2 - 1)")
			.arg(QString::number(useMsec ? et / 1000 : et), useMsec ? " * 1000" : "")));
}

DaoEntityField DaoEntityField::betweenMonth2MonthEndV3(qint64 st, qint64 et, bool useMsec) {
	QString startStr, endStr;
	if (useMsec) {
		startStr = QString("cast(datediff(ss, '1970-01-01 08:00:00', dateadd(mm,datediff(mm,0,dateadd(s, %1,'1970-01-01 08:00:00')),0)) as bigint) * 1000")
			.arg(QString::number(st / 1000));
		endStr = QString("(cast(datediff(ss, '1970-01-01 08:00:00', dateadd(mm,1,dateadd(mm,datediff(mm,0,dateadd(s, %1,'1970-01-01 08:00:00')),0))) as bigint) * 1000 - 1)")
			.arg(QString::number(et / 1000));
	} else {
		startStr = QString("datediff(ss, '1970-01-01 08:00:00', dateadd(mm,datediff(mm,0,dateadd(s, %1,'1970-01-01 08:00:00')),0))")
			.arg(QString::number(st));
		endStr = QString("(datediff(ss, '1970-01-01 08:00:00',dateadd(mm,1, dateadd(mm,datediff(mm,0,dateadd(s, %1,'1970-01-01 08:00:00')),0))) - 1)")
			.arg(QString::number(et));
	}

	return between(DaoEntityFunction().function(startStr), DaoEntityFunction().function(endStr));
}

DaoEntityField DaoEntityField::betweenCurMonthV1(bool useMsec) {
	auto ct = useMsec ? QDateTime::currentMSecsSinceEpoch() : QDateTime::currentSecsSinceEpoch();
	return betweenMonth2MonthEndV1(ct, ct, useMsec);
}

DaoEntityField DaoEntityField::betweenCurMonthV2(bool useMsec) {
	auto ct = useMsec ? QDateTime::currentMSecsSinceEpoch() : QDateTime::currentSecsSinceEpoch();
	return betweenMonth2MonthEndV2(ct, ct, useMsec);
}

DaoEntityField DaoEntityField::betweenCurMonthV3(bool useMsec) {
	auto ct = useMsec ? QDateTime::currentMSecsSinceEpoch() : QDateTime::currentSecsSinceEpoch();
	return betweenMonth2MonthEndV3(ct, ct, useMsec);
}

DaoEntityField DaoEntityField::betweenYear2YearEndV1(qint64 st, qint64 et, bool useMsec) {
	return between(DaoEntityFunction().function(QString("strftime('%s', date(datetime(%1, 'unixepoch'), 'start of year'), 'utc')%2")
		.arg(QString::number(useMsec ? st / 1000 : st), useMsec ? " * 1000" : "")),
		DaoEntityFunction().function(QString("(strftime('%s', datetime(date(datetime(%1, 'unixepoch'), 'start of year', '+1 year'), 'utc')%2 - 1)")
			.arg(QString::number(useMsec ? et / 1000 : et), useMsec ? " * 1000" : "")));
}

DaoEntityField DaoEntityField::betweenYear2YearEndV2(qint64 st, qint64 et, bool useMsec) {
	return between(DaoEntityFunction().function(QString("unix_timestamp(date_format(date(from_unixtime(%1)), '%Y-01-01'))%2")
		.arg(QString::number(useMsec ? st / 1000 : st), useMsec ? " * 1000" : "")),
		DaoEntityFunction().function(QString("(unix_timestamp(date_add(date_format(date(from_unixtime(%1)), '%Y-01-01'), interval 1 year)) - 1)%2 - 1)")
			.arg(QString::number(useMsec ? et / 1000 : et), useMsec ? " * 1000" : "")));
}

DaoEntityField DaoEntityField::betweenYear2YearEndV3(qint64 st, qint64 et, bool useMsec) {
	QString startStr, endStr;
	if (useMsec) {
		startStr = QString("cast(datediff(ss, '1970-01-01 08:00:00', dateadd(yy,datediff(yy,0,dateadd(s, %1,'1970-01-01 08:00:00')),0)) as bigint) * 1000")
			.arg(QString::number(st / 1000));
		endStr = QString("(cast(datediff(ss, '1970-01-01 08:00:00', dateadd(yy,1,dateadd(yy,datediff(yy,0,dateadd(s, %1,'1970-01-01 08:00:00')),0))) as bigint) * 1000 - 1)")
			.arg(QString::number(et / 1000));
	} else {
		startStr = QString("datediff(ss, '1970-01-01 08:00:00', dateadd(yy,datediff(yy,0,dateadd(s, %1,'1970-01-01 08:00:00')),0))")
			.arg(QString::number(st));
		endStr = QString("(datediff(ss, '1970-01-01 08:00:00',dateadd(yy,1, dateadd(yy,datediff(yy,0,dateadd(s, %1,'1970-01-01 08:00:00')),0))) - 1)")
			.arg(QString::number(et));
	}

	return between(DaoEntityFunction().function(startStr), DaoEntityFunction().function(endStr));
}

DaoEntityField DaoEntityField::betweenCurYearV1(bool useMsec) {
	auto ct = useMsec ? QDateTime::currentMSecsSinceEpoch() : QDateTime::currentSecsSinceEpoch();
	return betweenYear2YearEndV1(ct, ct, useMsec);
}

DaoEntityField DaoEntityField::betweenCurYearV2(bool useMsec) {
	auto ct = useMsec ? QDateTime::currentMSecsSinceEpoch() : QDateTime::currentSecsSinceEpoch();
	return betweenYear2YearEndV2(ct, ct, useMsec);
}

DaoEntityField DaoEntityField::betweenCurYearV3(bool useMsec) {
	auto ct = useMsec ? QDateTime::currentMSecsSinceEpoch() : QDateTime::currentSecsSinceEpoch();
	return betweenYear2YearEndV3(ct, ct, useMsec);
}