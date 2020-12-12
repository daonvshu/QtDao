#pragma once

#include "BaseQueryBuilder.h"

#include "../macro/macro.h" 

#include "../query/Join.h"

template<typename... T>
class JoinBuilder : public BaseQueryBuilder {
public:
    QUERY_BUILDER_USE_THROWABLE(JoinBuilder);
    QUERY_BUILDER_USE_FILTER(JoinBuilder);
    QUERY_BUILDER_USE_WITH(JoinBuilder);
    QUERY_BUILDER_USE_COLUMN(JoinBuilder);
    QUERY_BUILDER_USE_ON(JoinBuilder);

    void filter() override;
    void on() override;

    template<typename E>
    JoinBuilder<T...>& from();

    template<typename K>
    JoinBuilder<T...>& from(Select<K>& select);

    template<typename E>
    JoinBuilder<T...>& crossJoin();

    template<typename K>
    JoinBuilder<T...>& crossJoin(Select<K>& select);

    template<typename E>
    JoinBuilder<T...>& innerJoin();

    template<typename K>
    JoinBuilder<T...>& innerJoin(Select<K>& select);

    template<typename E>
    JoinBuilder<T...>& leftJoin();

    template<typename K>
    JoinBuilder<T...>& leftJoin(Select<K>& select);

    template<typename E>
    JoinBuilder<T...>& rightJoin();

    template<typename K>
    JoinBuilder<T...>& rightJoin(Select<K>& select);

    template<typename E>
    JoinBuilder<T...>& fullJoin();

    template<typename K>
    JoinBuilder<T...>& fullJoin(Select<K>& select);

    Join<T...> build();

private:
    QString lastTableName, mainTable;
    JoinType lastType;
    JoinData mainData;
    QHash<QString, JoinData> subJoinData;
};

template<typename ...T>
inline void JoinBuilder<T...>::filter() {
    mainData.joinType = lastType;
    mainData.filter = filterCondition;
}

template<typename ...T>
inline void JoinBuilder<T...>::on() {
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

template<typename ...T>
template<typename E>
inline JoinBuilder<T...>& JoinBuilder<T...>::from() {
    mainTable = E::Info::getSourceName();
    return *this;
}

template<typename ...T>
template<typename K>
inline JoinBuilder<T...>& JoinBuilder<T...>::from(Select<K>& select) {
    __super::from(select);
    mainTable = K::Info::getSourceName();
    mainData.fromSelectStatement = fromSelectStatement;
    mainData.fromSelectValues = fromSelectValues;
    mainData.fromSelectAs = fromSelectAs;
    fromDataClear();
    return *this;
}

template<typename ...T>
template<typename E>
inline JoinBuilder<T...>& JoinBuilder<T...>::crossJoin() {
    lastTableName = E::Info::getTableName();
    lastType = CrossJoin;
    return *this;
}

template<typename ...T>
template<typename K>
inline JoinBuilder<T...>& JoinBuilder<T...>::crossJoin(Select<K>& select) {
    __super::from(select);
    crossJoin<K>();
    return *this;
}

template<typename ...T>
template<typename E>
inline JoinBuilder<T...>& JoinBuilder<T...>::innerJoin() {
    lastTableName = E::Info::getTableName();
    lastType = InnerJoin;
    return *this;
}

template<typename ...T>
template<typename K>
inline JoinBuilder<T...>& JoinBuilder<T...>::innerJoin(Select<K>& select) {
    __super::from(select);
    innerJoin<K>();
    return *this;
}

template<typename ...T>
template<typename E>
inline JoinBuilder<T...>& JoinBuilder<T...>::leftJoin() {
    lastTableName = E::Info::getTableName();
    lastType = LeftJoin;
    return *this;
}

template<typename ...T>
template<typename K>
inline JoinBuilder<T...>& JoinBuilder<T...>::leftJoin(Select<K>& select) {
    __super::from(select);
    leftJoin<K>();
    return *this;
}

template<typename ...T>
template<typename E>
inline JoinBuilder<T...>& JoinBuilder<T...>::rightJoin() {
    lastTableName = E::Info::getTableName();
    lastType = RightJoin;
    return *this;
}

template<typename ...T>
template<typename K>
inline JoinBuilder<T...>& JoinBuilder<T...>::rightJoin(Select<K>& select) {
    __super::from(select);
    rightJoin<K>();
    return *this;
}

template<typename ...T>
template<typename E>
inline JoinBuilder<T...>& JoinBuilder<T...>::fullJoin() {
    lastTableName = E::Info::getTableName();
    lastType = FullJoin;
    return *this;
}

template<typename ...T>
template<typename K>
inline JoinBuilder<T...>& JoinBuilder<T...>::fullJoin(Select<K>& select) {
    __super::from(select);
    fullJoin<K>();
    return *this;
}

template<typename ...T>
inline Join<T...> JoinBuilder<T...>::build() {
    Join<T...> query(setThrowable, this);
    query.mainData = mainData;
    query.mainTable = mainTable;
    query.subJoinData = subJoinData;
    return query;
}