#pragma once

#include "QueryBuilder.h"

#include "../query/Delete.h"

template<typename T>
class DeleteBuilder : public QueryBuilder<T, Delete> {
public:
    DeleteBuilder() : filterCondition("and") {
    }

    template<typename... Args>
    DeleteBuilder& filter(const EntityCondition& condition, const Args&... args);

    template<typename... Args>
    DeleteBuilder& filter(const Connector& condition, const Args&... args);

    template<typename... Args>
    DeleteBuilder& filter(const FunctionCondition& condition, const Args&... args);

    DeleteBuilder& filter();

    Delete<T> build() override;

private:
    Connector filterCondition;
};

template<typename T>
template<typename ...Args>
inline DeleteBuilder<T>& DeleteBuilder<T>::filter(const EntityCondition& condition, const Args & ...args) {
    filterCondition.append(condition);
    return filter(args...);
}

template<typename T>
template<typename ...Args>
inline DeleteBuilder<T>& DeleteBuilder<T>::filter(const Connector& condition, const Args & ...args) {
    filterCondition.append(condition);
    return filter(args...);
}

template<typename T>
template<typename ...Args>
inline DeleteBuilder<T>& DeleteBuilder<T>::filter(const FunctionCondition& condition, const Args & ...args) {
    filterCondition.append(condition);
    return filter(args...);
}

template<typename T>
inline DeleteBuilder<T>& DeleteBuilder<T>::filter() {
    return *this;
}

template<typename T>
inline Delete<T> DeleteBuilder<T>::build() {
    Delete<T> query;
    query.filterCondition = filterCondition;
    query.queryThrowable = setThrowable;
    return query;
}