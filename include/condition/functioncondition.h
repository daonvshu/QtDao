#pragma once

#include "entityfield.h"
#include "functionconditiondata.h"

template<typename E>
class Select;

template<typename... E>
class Join;

class ConditionConstraint;

class Connector;
class FunctionCondition {
public:
    FunctionCondition();

	FunctionCondition(const QString& expressions);

	template<typename T, typename... E>
	FunctionCondition& field(const EntityField<T>& field, const EntityField<E>&... n);

	template<typename T>
	FunctionCondition& field(const EntityField<T>& field);

	template<typename... Args>
	FunctionCondition& value(QVariant v, Args... args);

	FunctionCondition& value(QVariant v);

	template<typename E>
	FunctionCondition& from(Select<E>& select);

	template<typename... E>
	FunctionCondition& from(Join<E...>& join);

private:
	QSharedDataPointer<FunctionConditionData> d;

private:
	void setFieldPrefixGetter(std::function<QString(const QString&)> prefixGetter);

	void combine();

	friend class Connector;
	friend class ConditionConstraint;
};

template<typename T, typename ...E>
inline FunctionCondition& FunctionCondition::field(const EntityField<T>& f, const EntityField<E>& ...n) {
	return field(f).field(n...);
}

template<typename T>
inline FunctionCondition& FunctionCondition::field(const EntityField<T>& f) {
	d->fields << FieldInfo{f.name, f.bindTable};
	return *this;
}

template<typename ...Args>
inline FunctionCondition& FunctionCondition::value(QVariant v, Args ...args) {
	return value(v).value(args...);
}

template<typename E>
inline FunctionCondition& FunctionCondition::from(Select<E>& select) {
	select.buildFilterSqlStatement();
	d->fields << FieldInfo{ select.statement.prepend("(").append(")"), "" };
	d->values << select.values;
	return *this;
}

template<typename ...E>
inline FunctionCondition& FunctionCondition::from(Join<E...>& join) {
	join.buildJoinSqlStatement();
	d->fields << FieldInfo{ join.statement.prepend("(").append(")"), "" };
	d->values << join.values;
	return *this;
}
