#pragma once

#include "EntityField.h"
#include "FunctionConditionData.h"

class Connector;
class FunctionCondition {
public:
	FunctionCondition() {}

	FunctionCondition(const QString& expressions);

	template<typename T, typename... E>
	FunctionCondition& field(const EntityField<T>& field, const EntityField<E>&... n);

	template<typename T>
	FunctionCondition& field(const EntityField<T>& field);

	template<typename... Args>
	FunctionCondition& value(QVariant v, Args... args);

	FunctionCondition& value(QVariant v);

private:
	QSharedDataPointer<FunctionConditionData> d;

private:
	void setFieldPrefixGetter(std::function<QString(const QString&)> prefixGetter);

	void combine();

	friend class Connector;
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

inline FunctionCondition& FunctionCondition::value(QVariant v) {
	d->values << v;
	return *this;
}
