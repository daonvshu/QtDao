#pragma once

#include "QueryBuilder.h"

#include "../query/Select.h"

template<typename T>
class EntityField;

template<typename T>
class SelectBuilder : public QueryBuilder<T, Select> {
public:
    SelectBuilder() : columnBind(","), filterCondition("and") {
    }

    template<typename... Args>
    SelectBuilder& filter(const EntityCondition& condition, const Args&... args);

    template<typename... Args>
    SelectBuilder& filter(const Connector& condition, const Args&... args);

    template<typename... Args>
    SelectBuilder& filter(const FunctionCondition& condition, const Args&... args);

    template<typename... Args>
    SelectBuilder& with(const ConditionConstraint& constaint, const Args&... args);

    template<typename Col, typename... Args>
    SelectBuilder& column(const Col& function, const Args&... args);

    SelectBuilder& filter();

    SelectBuilder& with();

    SelectBuilder& column();

    Select<T> build() override;

protected:
    Connector columnBind, filterCondition, constraintCondition;
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
inline SelectBuilder<T>& SelectBuilder<T>::filter(const FunctionCondition& condition, const Args & ...args) {
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
template<typename Col, typename ...Args>
inline SelectBuilder<T>& SelectBuilder<T>::column(const Col& function, const Args & ...args) {
    columnBind.appendCol(function);
    return column(args...);
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
inline SelectBuilder<T>& SelectBuilder<T>::column() {
    return *this;
}

template<typename T>
inline Select<T> SelectBuilder<T>::build() {
    Select<T> query;
    query.columnBind = columnBind;
    query.filterCondition = filterCondition;
    query.constraintCondition = constraintCondition;
    query.queryThrowable = setThrowable;
    return query;
}