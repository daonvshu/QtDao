#pragma once

#include "basequerybuilder.h"

#include "../macro/macro.h" 

#include "../query/join.h"

QTDAO_BEGIN_NAMESPACE

template<typename... E>
class JoinBuilder : public BaseQueryBuilder {
public:
    QUERY_BUILDER_USE_FATAL_DISABLE(JoinBuilder)

    QUERY_BUILDER_USE_FILTER(JoinBuilder)
    QUERY_BUILDER_USE_WITH(JoinBuilder)
    QUERY_BUILDER_USE_COLUMN(JoinBuilder)
    QUERY_BUILDER_USE_ON(JoinBuilder)

    QUERY_BUILDER_USE_QUERY_UNION_SELECT(JoinBuilder)
    QUERY_BUILDER_USE_QUERY_UNION_JOIN(JoinBuilder)

    void filter() override;
    void on() override;

    template<typename E2>
    JoinBuilder<E...>& columnAll() {
        BaseQueryBuilder::columnAll<E2>();
        return *this;
    }

    template<typename E2>
    JoinBuilder<E...>& from();

    template<typename E2>
    JoinBuilder<E...>& from(Select<E2>& select);

    JoinBuilder<E...>& from(RecursiveQueryBuilder& builder);

    template<typename E2>
    JoinBuilder<E...>& crossJoin();

    template<typename E2>
    JoinBuilder<E...>& crossJoin(Select<E2>& select);

    JoinBuilder<E...>& crossJoin(RecursiveQueryBuilder& builder);

    template<typename E2>
    JoinBuilder<E...>& innerJoin();

    template<typename E2>
    JoinBuilder<E...>& innerJoin(Select<E2>& select);

    JoinBuilder<E...>& innerJoin(RecursiveQueryBuilder& builder);

    template<typename E2>
    JoinBuilder<E...>& leftJoin();

    template<typename E2>
    JoinBuilder<E...>& leftJoin(Select<E2>& select);

    JoinBuilder<E...>& leftJoin(RecursiveQueryBuilder& builder);

    template<typename E2>
    JoinBuilder<E...>& rightJoin();

    template<typename E2>
    JoinBuilder<E...>& rightJoin(Select<E2>& select);

    JoinBuilder<E...>& rightJoin(RecursiveQueryBuilder& builder);

    template<typename E2>
    JoinBuilder<E...>& fullJoin();

    template<typename E2>
    JoinBuilder<E...>& fullJoin(Select<E2>& select);

    JoinBuilder<E...>& fullJoin(RecursiveQueryBuilder& builder);

    Join<E...> build();

private:
    QString lastTableName, mainTable;
    JoinType lastType = CrossJoin;
    JoinData mainData;
    QHash<QString, JoinData> subJoinData;
};

template<typename ...E>
inline void JoinBuilder<E...>::filter() {
    mainData.joinType = lastType;
    mainData.filter = filterCondition;
}

template<typename ...E>
inline void JoinBuilder<E...>::on() {
    JoinData data;
    data.joinType = lastType;
    data.filter = onCondition;
    data.fromSelectStatement = fromSelectStatement;
    data.fromSelectValues = fromSelectValues;
    data.fromSelectAs = fromSelectAs;
    data.recursiveQuery = recursiveQuery;
    subJoinData.insert(lastTableName, data);
    onCondition.clear();
    fromDataClear();
}

template<typename ...E>
template<typename E2>
inline JoinBuilder<E...>& JoinBuilder<E...>::from() {
    mainTable = E2::Info::getTableName();
    return *this;
}

template<typename ...E>
template<typename E2>
inline JoinBuilder<E...>& JoinBuilder<E...>::from(Select<E2>& select) {
    BaseQueryBuilder::from(select);
    mainTable = E2::Info::getTableName();
    mainData.fromSelectStatement = fromSelectStatement;
    mainData.fromSelectValues = fromSelectValues;
    mainData.fromSelectAs = fromSelectAs;
    mainData.recursiveQuery = recursiveQuery;
    fromDataClear();
    return *this;
}

template<typename ...E>
inline JoinBuilder<E...>& JoinBuilder<E...>::from(RecursiveQueryBuilder& builder) {
    BaseQueryBuilder::from(builder);
    mainTable = fromSelectAs;
    mainData.fromSelectStatement = fromSelectStatement;
    mainData.fromSelectValues = fromSelectValues;
    mainData.fromSelectAs = fromSelectAs;
    mainData.recursiveQuery = recursiveQuery;
    fromDataClear();
    return *this;
}

template<typename ...E>
template<typename E2>
inline JoinBuilder<E...>& JoinBuilder<E...>::crossJoin() {
    lastTableName = E2::Info::getTableName();
    lastType = CrossJoin;
    return *this;
}

template<typename ...E>
template<typename E2>
inline JoinBuilder<E...>& JoinBuilder<E...>::crossJoin(Select<E2>& select) {
    BaseQueryBuilder::from(select);
    crossJoin<E2>();
    return *this;
}

template<typename ...E>
inline JoinBuilder<E...>& JoinBuilder<E...>::crossJoin(RecursiveQueryBuilder& builder) {
    BaseQueryBuilder::from(builder);
    lastTableName = fromSelectAs;
    lastType = CrossJoin;
    return *this;
}

template<typename ...E>
template<typename E2>
inline JoinBuilder<E...>& JoinBuilder<E...>::innerJoin() {
    lastTableName = E2::Info::getTableName();
    lastType = InnerJoin;
    return *this;
}

template<typename ...E>
template<typename E2>
inline JoinBuilder<E...>& JoinBuilder<E...>::innerJoin(Select<E2>& select) {
    BaseQueryBuilder::from(select);
    innerJoin<E2>();
    return *this;
}

template<typename ...E>
inline JoinBuilder<E...>& JoinBuilder<E...>::innerJoin(RecursiveQueryBuilder& builder) {
    BaseQueryBuilder::from(builder);
    lastTableName = fromSelectAs;
    lastType = InnerJoin;
    return *this;
}

template<typename ...E>
template<typename E2>
inline JoinBuilder<E...>& JoinBuilder<E...>::leftJoin() {
    lastTableName = E2::Info::getTableName();
    lastType = LeftJoin;
    return *this;
}

template<typename ...E>
template<typename E2>
inline JoinBuilder<E...>& JoinBuilder<E...>::leftJoin(Select<E2>& select) {
    BaseQueryBuilder::from(select);
    leftJoin<E2>();
    return *this;
}

template<typename ...E>
inline JoinBuilder<E...>& JoinBuilder<E...>::leftJoin(RecursiveQueryBuilder& builder) {
    BaseQueryBuilder::from(builder);
    lastTableName = fromSelectAs;
    lastType = LeftJoin;
    return *this;
}

template<typename ...E>
template<typename E2>
inline JoinBuilder<E...>& JoinBuilder<E...>::rightJoin() {
    lastTableName = E2::Info::getTableName();
    lastType = RightJoin;
    return *this;
}

template<typename ...E>
template<typename E2>
inline JoinBuilder<E...>& JoinBuilder<E...>::rightJoin(Select<E2>& select) {
    BaseQueryBuilder::from(select);
    rightJoin<E2>();
    return *this;
}

template<typename ...E>
inline JoinBuilder<E...>& JoinBuilder<E...>::rightJoin(RecursiveQueryBuilder& builder) {
    BaseQueryBuilder::from(builder);
    lastTableName = fromSelectAs;
    lastType = RightJoin;
    return *this;
}

template<typename ...E>
template<typename E2>
inline JoinBuilder<E...>& JoinBuilder<E...>::fullJoin() {
    lastTableName = E2::Info::getTableName();
    lastType = FullJoin;
    return *this;
}

template<typename ...E>
template<typename E2>
inline JoinBuilder<E...>& JoinBuilder<E...>::fullJoin(Select<E2>& select) {
    BaseQueryBuilder::from(select);
    fullJoin<E2>();
    return *this;
}

template<typename ...E>
inline JoinBuilder<E...>& JoinBuilder<E...>::fullJoin(RecursiveQueryBuilder& builder) {
    BaseQueryBuilder::from(builder);
    lastTableName = fromSelectAs;
    lastType = FullJoin;
    return *this;
}

template<typename ...E>
inline Join<E...> JoinBuilder<E...>::build() {
    Join<E...> query(setFatalEnabled, this);
    query.mainData = mainData;
    query.mainTable = mainTable;
    query.subJoinData = subJoinData;
    return query;
}

QTDAO_END_NAMESPACE