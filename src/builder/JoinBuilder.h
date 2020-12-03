#pragma once

#include "BaseQueryBuilder.h"

#include "../query/Join.h"

template<typename... T>
class JoinBuilder : BaseQueryBuilder {
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

    template<typename E>
    JoinBuilder<T...>& crossJoin();

    template<typename E>
    JoinBuilder<T...>& innerJoin();

    template<typename E>
    JoinBuilder<T...>& leftJoin();

    template<typename E>
    JoinBuilder<T...>& rightJoin();

    template<typename E>
    JoinBuilder<T...>& fullJoin();

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
    subJoinData.insert(lastTableName, JoinData{ lastType, onCondition });
    onCondition.clear();
}

template<typename ...T>
template<typename E>
inline JoinBuilder<T...>& JoinBuilder<T...>::from() {
    mainTable = E::Info::getTableName();
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
template<typename E>
inline JoinBuilder<T...>& JoinBuilder<T...>::innerJoin() {
    lastTableName = E::Info::getTableName();
    lastType = InnerJoin;
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
template<typename E>
inline JoinBuilder<T...>& JoinBuilder<T...>::rightJoin() {
    lastTableName = E::Info::getTableName();
    lastType = RightJoin;
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
inline Join<T...> JoinBuilder<T...>::build() {
    Join<T...> query;
    query.mainData = mainData;
    query.mainTable = mainTable;
    query.subJoinData = subJoinData;
    query.columnBind = columnBind;
    query.constraintCondition = constraintCondition;
    return query;
}