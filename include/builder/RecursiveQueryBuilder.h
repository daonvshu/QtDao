﻿#pragma once

#include "../query/Select.h"

class BaseQueryBuilder;

class RecursiveQueryBuilder {
public:
    RecursiveQueryBuilder(bool unionAll = false) : unionAll(unionAll) {}

    template<typename E>
    RecursiveQueryBuilder& initialSelect(Select<E>& select);

    template<typename... E>
    RecursiveQueryBuilder& initialSelect(Join<E...>& join);

    template<typename E>
    RecursiveQueryBuilder& recursiveSelect(Select<E>& select);

    template<typename... E>
    RecursiveQueryBuilder& recursiveSelect(Join<E...>& join);

    template<typename E>
    RecursiveQueryBuilder& tmp();

private:
    QString initialQueryStatement, recursiveQueryStatement;
    QVariantList initialQueryValue, recursiveQueryValue;
    QString tmpTableName;
    bool unionAll;

    friend class BaseQueryBuilder;
};

template<typename E>
inline RecursiveQueryBuilder& RecursiveQueryBuilder::initialSelect(Select<E>& select) {
    select.buildFilterSqlStatement();
    initialQueryStatement = select.statement;
    initialQueryValue = select.values;
    return *this;
}

template<typename ...E>
inline RecursiveQueryBuilder& RecursiveQueryBuilder::initialSelect(Join<E...>& join) {
    join.insideRecursiveQuery = true;
    join.buildJoinSqlStatement();
    initialQueryStatement = join.statement;
    initialQueryValue = join.values;
    return *this;
}

template<typename E>
inline RecursiveQueryBuilder& RecursiveQueryBuilder::recursiveSelect(Select<E>& select) {
    select.buildFilterSqlStatement();
    recursiveQueryStatement = select.statement;
    recursiveQueryValue = select.values;
    return *this;
}

template<typename ...E>
inline RecursiveQueryBuilder& RecursiveQueryBuilder::recursiveSelect(Join<E...>& join) {
    join.insideRecursiveQuery = true;
    join.buildJoinSqlStatement();
    recursiveQueryStatement = join.statement;
    recursiveQueryValue = join.values;
    return *this;
}

template<typename E>
inline RecursiveQueryBuilder& RecursiveQueryBuilder::tmp() {
    tmpTableName = E::Info::getTableName();
    return *this;
}
