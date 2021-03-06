﻿#pragma once

#include "BaseQueryBuilder.h"

#include "../macro/macro.h"

#include "../query/InsertIntoSelect.h"

template<typename E>
class InsertIntoSelectBuilder : public BaseQueryBuilder {
public:
    QUERY_BUILDER_USE_THROWABLE(InsertIntoSelectBuilder)
    QUERY_BUILDER_USE_COLUMN(InsertIntoSelectBuilder)

    QUERY_BUILDER_USE_QUERY_FROM_SELECT2(InsertIntoSelectBuilder)
    QUERY_BUILDER_USE_QUERY_FROM_JOIN2(InsertIntoSelectBuilder)

    QUERY_BUILDER_BUILDER_DECLARE(InsertIntoSelect)
};
