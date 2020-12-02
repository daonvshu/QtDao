#pragma once

#include "BaseQueryBuilder.h"

#include "../query/Delete.h"

template<typename T>
class DeleteBuilder : public BaseQueryBuilder {
public:
    QUERY_BUILDER_USE_THROWABLE(DeleteBuilder);
    QUERY_BUILDER_USE_FILTER(DeleteBuilder);

    Delete<T> build();
};

template<typename T>
inline Delete<T> DeleteBuilder<T>::build() {
    Delete<T> query;
    query.filterCondition = filterCondition;
    query.queryThrowable = setThrowable;
    return query;
}