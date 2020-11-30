#pragma once

#include "BaseQueryBuilder.h"

#include "../query/Update.h"

template<typename T>
class UpdateBuilder : public BaseQueryBuilder<T, Update> {
public:
    QUERY_BUILDER_USE_SET(UpdateBuilder);
    QUERY_BUILDER_USE_FILTER(UpdateBuilder);

    Update<T> build() override;
};

template<typename T>
inline Update<T> UpdateBuilder<T>::build() {
    Update<T> query;
    query.setCondition = setCondition;
    query.filterCondition = filterCondition;
    query.queryThrowable = setThrowable;
    return query;
}
