#pragma once

#include "BaseQueryBuilder.h"

#include "../query/Select.h"

template<typename T>
class SelectBuilder : public BaseQueryBuilder {
public:
    QUERY_BUILDER_USE_THROWABLE(SelectBuilder);
    QUERY_BUILDER_USE_FILTER(SelectBuilder);
    QUERY_BUILDER_USE_WITH(SelectBuilder);
    QUERY_BUILDER_USE_COLUMN(SelectBuilder);

    Select<T> build();
};

template<typename T>
inline Select<T> SelectBuilder<T>::build() {
    Select<T> query;
    query.columnBind = columnBind;
    query.filterCondition = filterCondition;
    query.constraintCondition = constraintCondition;
    query.queryThrowable = setThrowable;
    return query;
}