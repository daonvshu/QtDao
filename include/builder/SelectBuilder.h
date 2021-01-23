#pragma once

#include "BaseQueryBuilder.h"

#include "../macro/macro.h"

#include "../query/Select.h"

template<typename E>
class SelectBuilder : public BaseQueryBuilder {
public:
    QUERY_BUILDER_USE_THROWABLE(SelectBuilder)
    QUERY_BUILDER_USE_FILTER(SelectBuilder)
    QUERY_BUILDER_USE_WITH(SelectBuilder)
    QUERY_BUILDER_USE_COLUMN(SelectBuilder)
    QUERY_BUILDER_USE_COLUMNALL(SelectBuilder)

    QUERY_BUILDER_USE_QUERY_FROM_SELECT(SelectBuilder)
    QUERY_BUILDER_USE_QUERY_FROM_JOIN2(SelectBuilder)
    QUERY_BUILDER_USE_QUERY_FROM_RECURSIVE(SelectBuilder)

    QUERY_BUILDER_USE_QUERY_UNION_SELECT(SelectBuilder)
    QUERY_BUILDER_USE_QUERY_UNION_JOIN(SelectBuilder)

    QUERY_BUILDER_BUILDER_DECLARE(Select)
};
