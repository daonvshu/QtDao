#pragma once

#include "BaseQueryBuilder.h"

#include "../macro/macro.h"

#include "../query/Select.h"

template<typename T>
class SelectBuilder : public BaseQueryBuilder {
public:
    QUERY_BUILDER_USE_THROWABLE(SelectBuilder);
    QUERY_BUILDER_USE_FILTER(SelectBuilder);
    QUERY_BUILDER_USE_WITH(SelectBuilder);
    QUERY_BUILDER_USE_COLUMN(SelectBuilder);

    QUERY_BUILDER_USE_QUERY_FROM(SelectBuilder);

    QUERY_BUILDER_BUILDER_DECLARE(Select);
};
