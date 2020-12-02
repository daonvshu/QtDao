#pragma once

#include "BaseQueryBuilder.h"

#include "../query/Join.h"

template<typename... T>
class JoinBuilder : BaseQueryBuilder {
public:
    QUERY_BUILDER_USE_THROWABLE(JoinBuilder);
    QUERY_BUILDER_USE_COLUMN(JoinBuilder);
    QUERY_BUILDER_USE_FILTER(JoinBuilder);
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
    QString lastTableName;
    typename Join<T...>::JoinType lastType;
    typename Join<T...>::JoinData mainData;
    QHash<QString, typename Join<T...>::JoinData> subJoinData;
};

template<typename ...T>
inline void JoinBuilder<T...>::filter() {

}

template<typename ...T>
inline void JoinBuilder<T...>::on() {

}

template<typename ...T>
template<typename E>
inline JoinBuilder<T...>& JoinBuilder<T...>::from() {
    lastTableName = E::Info::getTableName();
    return *this;
}

template<typename ...T>
template<typename E>
inline JoinBuilder<T...>& JoinBuilder<T...>::crossJoin() {
    lastTableName = E::Info::getTableName();
    lastType = Join<T...>::CrossJoin;
    return *this;
}

template<typename ...T>
template<typename E>
inline JoinBuilder<T...>& JoinBuilder<T...>::innerJoin() {
    lastTableName = E::Info::getTableName();
    lastType = Join<T...>::InnerJoin;
    return *this;
}

template<typename ...T>
template<typename E>
inline JoinBuilder<T...>& JoinBuilder<T...>::leftJoin() {
    lastTableName = E::Info::getTableName();
    lastType = Join<T...>::LeftJoin;
    return *this;
}

template<typename ...T>
template<typename E>
inline JoinBuilder<T...>& JoinBuilder<T...>::rightJoin() {
    lastTableName = E::Info::getTableName();
    lastType = Join<T...>::RightJoin;
    return *this;
}

template<typename ...T>
template<typename E>
inline JoinBuilder<T...>& JoinBuilder<T...>::fullJoin() {
    lastTableName = E::Info::getTableName();
    lastType = Join<T...>::FullJoin;
    return *this;
}

template<typename ...T>
inline Join<T...> JoinBuilder<T...>::build() {
    return Join<T...>();
}