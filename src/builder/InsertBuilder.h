#pragma once

#include "BaseQueryBuilder.h"

#include "../query/Insert.h"

template<typename T>
class InsertBuilder : public BaseQueryBuilder {
public:
    QUERY_BUILDER_USE_THROWABLE(InsertBuilder);
    QUERY_BUILDER_USE_SET(InsertBuilder);

    Insert<T> build();
};

template<typename T>
inline Insert<T> InsertBuilder<T>::build() {
    Insert<T> query;
    query.connector = setCondition;
    query.queryThrowable = setThrowable;
    return query;
}
