#pragma once

#include "BaseQuery.h"

template<typename... T>
class JoinBuilder;

template<typename... E>
class Join : BaseQuery {
public:
    QList<std::tuple<E...>> list();

private:
    enum JoinType {
        CrossJoin,
        InnerJoin,
        LeftJoin,
        RightJoin,
        FullJoin,
    };

    struct JoinData {
        JoinType joinType;
        Connector filter;
    };

    friend class JoinBuilder<E...>;
};