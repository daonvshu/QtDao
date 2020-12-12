#pragma once

#include "BaseQueryBuilder.h"

#include "../macro/macro.h" 

#include "../query/Join.h"

template<typename... E>
class JoinBuilder : public BaseQueryBuilder {
public:
    QUERY_BUILDER_USE_THROWABLE(JoinBuilder);
    QUERY_BUILDER_USE_FILTER(JoinBuilder);
    QUERY_BUILDER_USE_WITH(JoinBuilder);
    QUERY_BUILDER_USE_COLUMN(JoinBuilder);
    QUERY_BUILDER_USE_ON(JoinBuilder);

    QUERY_BUILDER_USE_QUERY_UNION_SELECT(JoinBuilder);
    QUERY_BUILDER_USE_QUERY_UNION_JOIN(JoinBuilder);

    void filter() override;
    void on() override;

    template<typename E2>
    JoinBuilder<E...>& from();

    template<typename E2>
    JoinBuilder<E...>& from(Select<E2>& select);

    template<typename E2>
    JoinBuilder<E...>& crossJoin();

    template<typename E2>
    JoinBuilder<E...>& crossJoin(Select<E2>& select);

    template<typename E2>
    JoinBuilder<E...>& innerJoin();

    template<typename E2>
    JoinBuilder<E...>& innerJoin(Select<E2>& select);

    template<typename E2>
    JoinBuilder<E...>& leftJoin();

    template<typename E2>
    JoinBuilder<E...>& leftJoin(Select<E2>& select);

    template<typename E2>
    JoinBuilder<E...>& rightJoin();

    template<typename E2>
    JoinBuilder<E...>& rightJoin(Select<E2>& select);

    template<typename E2>
    JoinBuilder<E...>& fullJoin();

    template<typename E2>
    JoinBuilder<E...>& fullJoin(Select<E2>& select);

    Join<E...> build();

private:
    QString lastTableName, mainTable;
    JoinType lastType;
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
    subJoinData.insert(lastTableName, data);
    onCondition.clear();
    fromDataClear();
}

template<typename ...E>
template<typename E2>
inline JoinBuilder<E...>& JoinBuilder<E...>::from() {
    mainTable = E2::Info::getSourceName();
    return *this;
}

template<typename ...E>
template<typename E2>
inline JoinBuilder<E...>& JoinBuilder<E...>::from(Select<E2>& select) {
    __super::from(select);
    mainTable = E2::Info::getSourceName();
    mainData.fromSelectStatement = fromSelectStatement;
    mainData.fromSelectValues = fromSelectValues;
    mainData.fromSelectAs = fromSelectAs;
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
    __super::from(select);
    crossJoin<E2>();
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
    __super::from(select);
    innerJoin<E2>();
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
    __super::from(select);
    leftJoin<E2>();
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
    __super::from(select);
    rightJoin<E2>();
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
    __super::from(select);
    fullJoin<E2>();
    return *this;
}

template<typename ...E>
inline Join<E...> JoinBuilder<E...>::build() {
    Join<E...> query(setThrowable, this);
    query.mainData = mainData;
    query.mainTable = mainTable;
    query.subJoinData = subJoinData;
    return query;
}