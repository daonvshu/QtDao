#pragma once

#include "BaseQueryBuilder.h"

#include "../query/Insert.h"

template<typename T>
class InsertBuilder : public BaseQueryBuilder<T, Insert> {
public:
    QUERY_BUILDER_USE_SET(InsertBuilder);

    Insert<T> build() override;
};

template<typename T>
inline Insert<T> InsertBuilder<T>::build() {
    Insert<T> query;
    query.connector = setCondition;
    query.queryThrowable = setThrowable;
    return query;
}
