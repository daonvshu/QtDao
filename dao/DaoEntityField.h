/*
Descript:	字段Key-Value、字段Key-Value之间的拼接类，用于组合sql的条件语句
Author:		daonvshu
Version:	2.1
Date:		2018/12/21
Last-Md:	2019/01/09
*/
#pragma once

#include <qobject.h>
#include <qvariant.h>

#define and &&
#define or	||
#define _B	*

class DaoEntityField {
public:
	DaoEntityField() = default;

	explicit DaoEntityField(const char* n) {
		this->n = n;
	}

	/*获取条件语句拼接结果*/
	QString getKvPair() {
		return kvPairStr;
	}

	/*获取条件语句对应值*/
	QVariantList getVList() {
		return vlist;
	}

	static void bindJoinOrder(int o) {}

	/*绑定join前缀*/
	template<typename... T>
	static void bindJoinOrder(int o, DaoEntityField& f, T&... t) {
		f.joinPrefix = ('a' + o);
		f.joinPrefix.append('.');
		bindJoinOrder(o, t...);
	}

	/*获取字段名，包含join前缀（如果设置过）*/
	QString name() {
		return joinPrefix + n;
	}

private:
	QString joinPrefix;
	const char* n;
	QString kvPairStr;//键值对字符串
	QVariantList vlist;//值列表
	bool suffix;

private:
	DaoEntityField sv(const char* op, const QVariant& v) {
		clear();
		kvPairStr.append(name()).append(op).append('?');
		vlist.append(v);
		return *this;
	}

	DaoEntityField sv2(const char* op, const QVariant& v) {
		clear();
		kvPairStr.append(name()).append('=').append(name()).append(op).append('?');
		vlist.append(v);
		return *this;
	}

	DaoEntityField sv(const char* op, DaoEntityField& f) {
		clear();
		kvPairStr.append(name()).append(op).append(f.name());
		return *this;
	}

	void clear() {
		kvPairStr = "";
		vlist.clear();
		suffix = false;
	}

public:
	/*equal "="*/
	DaoEntityField operator==(const QVariant& v) {
		return sv("=", v);
	}
	/*like "field1=field2"*/
	DaoEntityField operator==(DaoEntityField& f) {
		return sv("=", f);
	}

	/*no equal "!="*/
	DaoEntityField operator!=(const QVariant& v) {
		return sv("!=", v);
	}
	/*like "field1!=field2"*/
	DaoEntityField operator!=(DaoEntityField& f) {
		return sv("!=", f);
	}

	/*greater than ">"*/
	DaoEntityField operator>(const QVariant& v) {
		return sv(">", v);
	}
	/*like "field1>field2"*/
	DaoEntityField operator>(DaoEntityField& f) {
		return sv(">", f);
	}

	/*greater than and equal ">=" */
	DaoEntityField operator>=(const QVariant& v) {
		return sv(">=", v);
	}
	/*like "field1>=field2"*/
	DaoEntityField operator>=(DaoEntityField& f) {
		return sv(">=", f);
	}

	/*less than "<"*/
	DaoEntityField operator<(const QVariant& v) {
		return sv("<", v);
	}
	/*like "field1<field2"*/
	DaoEntityField operator<(DaoEntityField& f) {
		return sv("<", f);
	}

	/*less than and equal "<="*/
	DaoEntityField operator<=(const QVariant& v) {
		return sv("<=", v);
	}
	/*like "field1<=field2"*/
	DaoEntityField operator<=(DaoEntityField& f) {
		return sv("<=", f);
	}

	/*like "like"*/
	DaoEntityField like(const QVariant& v) {
		return sv("like", v);
	}

	/*mod "a=a%value*/
	DaoEntityField operator%(const QVariant& v) {
		return sv2("%", v);
	}
	/*plus value "a=a+value"*/
	DaoEntityField operator+(const QVariant& v) {
		return sv2("+", v);
	}
	/*minus value "a=a-value"*/
	DaoEntityField operator-(const QVariant& v) {
		return sv2("-", v);
	}
	/*repeat count "a=a*count"*/
	DaoEntityField operator*(const QVariant& v) {
		return sv2("*", v);
	}
	/*divide count "a=a/count"*/
	DaoEntityField operator/(const QVariant& v) {
		return sv2("/", v);
	}
	/*plus one "a=a+1"*/
	DaoEntityField operator++(int) {
		return sv2("+", 1);
	}
	/*minus one "a=a-1"*/
	DaoEntityField operator--(int) {
		return sv2("-", 1);
	}

	/*get field name*/
	const char* operator()() {
		return n;
	}

	/*used by set condition*/
	DaoEntityField operator,(const DaoEntityField& f) {
		if (f.kvPairStr.isEmpty()) {
			return *this;
		}
		kvPairStr.append(',').append(f.kvPairStr);
		vlist.append(f.vlist);
		return *this;
	}

	/*condition and, consider using macro 'and'*/
	DaoEntityField operator&&(const DaoEntityField& f) {
		if (f.kvPairStr.isEmpty()) {
			return *this;
		}
		if (!f.suffix) {
			kvPairStr.append(" and");
		}
		kvPairStr.append(' ').append(f.kvPairStr);
		vlist.append(f.vlist);
		return *this;
	}

	/*condition or, consider using macro 'or'*/
	DaoEntityField operator||(const DaoEntityField& f) {
		if (f.kvPairStr.isEmpty()) {
			return *this;
		}
		if (!f.suffix) {
			kvPairStr.append(" or");
		}
		kvPairStr.append(' ').append(f.kvPairStr);
		vlist.append(f.vlist);
		return *this;
	}

	/*for bracket expressions, consider using macro '_B'*/
	DaoEntityField operator*() {
		if (kvPairStr.isEmpty()) {
			return *this;
		}
		kvPairStr = '(' + kvPairStr + ')';
		return *this;
	}

	/*condition in*/
	DaoEntityField in(const QVariantList& v) {
		clear();
		kvPairStr.append(name()).append(" in (").append(QString("?,").repeated(v.size()));
		kvPairStr = kvPairStr.left(kvPairStr.length() - 1);
		kvPairStr.append(')');
		vlist.append(v);
		return *this;
	}

	/*condition between*/
	DaoEntityField between(const QVariant& a, const QVariant& b) {
		clear();
		kvPairStr.append(name()).append(" between ? and ?");
		vlist.append(a);
		vlist.append(b);
		return *this;
	}

	/*condition limit*/
	DaoEntityField limit(int a, int b) {
		if (!kvPairStr.isEmpty()) {
			kvPairStr.append(' ');
		}
		kvPairStr.append("limit ").append(QString::number(a)).append(',').append(QString::number(b));
		suffix = true;
		return *this;
	}

	/*condition order by*/
	DaoEntityField orderBy(bool sequence = true) {
		if (!kvPairStr.isEmpty()) {
			kvPairStr.append(' ');
		}
		kvPairStr.append("order by ").append(name()).append(sequence ? "asc" : "desc");
		suffix = true;
		return *this;
	}

	/*condition group by*/
	DaoEntityField groupBy(bool sequence = true) {
		if (!kvPairStr.isEmpty()) {
			kvPairStr.append(' ');
		}
		kvPairStr.append("group by ").append(name()).append(sequence ? "asc" : "desc");
		suffix = true;
		return *this;
	}

	/*return a empty data*/
	DaoEntityField empty() {
		clear();
		return *this;
	}
};

/*create field used by a string*/
inline DaoEntityField operator"" _field(const char* c, size_t n) {
	return DaoEntityField(c);
}
