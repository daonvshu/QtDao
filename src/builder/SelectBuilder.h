#pragma once

#include "QueryBuilder.h"

#include "../query/Select.h"

template<typename T>
class SelectBuilder : public QueryBuilder<T, Select> {
public:
    SelectBuilder() : filterCondition("and") {
    }

    template<typename... Args>
    SelectBuilder& filter(const EntityCondition& condition, const Args&... args);

    template<typename... Args>
    SelectBuilder& filter(const Connector& condition, const Args&... args);

    template<typename... Args>
    SelectBuilder& with(const ConditionConstraint& constaint, const Args&... args);

    SelectBuilder& filter();

    SelectBuilder& with();

    Select<T> build() override;

private:
    Connector filterCondition, constraintCondition;
};

template<typename T>
template<typename ...Args>
inline SelectBuilder<T>& SelectBuilder<T>::filter(const EntityCondition& condition, const Args & ...args) {
    filterCondition.append(condition);
    return filter(args...);
}

template<typename T>
template<typename ...Args>
inline SelectBuilder<T>& SelectBuilder<T>::filter(const Connector& condition, const Args & ...args) {
    filterCondition.append(condition);
    return filter(args...);
}

template<typename T>
template<typename ...Args>
inline SelectBuilder<T>& SelectBuilder<T>::with(const ConditionConstraint& constaint, const Args & ...args) {
    constraintCondition.append(constaint);
    return with(args...);
}

template<typename T>
inline SelectBuilder<T>& SelectBuilder<T>::filter() {
    return *this;
}

template<typename T>
inline SelectBuilder<T>& SelectBuilder<T>::with() {
    return *this;
}

template<typename T>
inline Select<T> SelectBuilder<T>::build() {
    Select<T> query;
    query.filterCondition = filterCondition;
    query.constraintCondition = constraintCondition;
    query.queryThrowable = setThrowable;
    return query;
}