#pragma once

#include "QueryBuilder.h"

#include "../query/Insert.h"

template<typename T>
class InsertBuilder : public QueryBuilder<T, Insert> {
public:
    template<typename... Args>
    InsertBuilder& set(const EntityCondition& condition, const Args&... args);

    InsertBuilder& set();

    Insert<T> build() override;

private:
    Connector connector;
};

template<typename T>
template<typename ...Args>
inline InsertBuilder<T>& InsertBuilder<T>::set(const EntityCondition& condition, const Args & ...args) {
    connector.append(condition);
    return set(args...);
}

template<typename T>
inline InsertBuilder<T>& InsertBuilder<T>::set() {
    return *this;
}

template<typename T>
inline Insert<T> InsertBuilder<T>::build() {
    Insert<T> query;
    query.connector = connector;
    query.queryThrowable = setThrowable;
    return query;
}
