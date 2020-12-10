#pragma once

#include "BaseQueryBuilder.h"

#include "../macro/macro.h"

#include "../query/Update.h"

template<typename T>
class UpdateBuilder : public BaseQueryBuilder {
public:
    QUERY_BUILDER_USE_THROWABLE(UpdateBuilder);
    QUERY_BUILDER_USE_SET(UpdateBuilder);
    QUERY_BUILDER_USE_FILTER(UpdateBuilder);

    QUERY_BUILDER_BUILDER_DECLARE(Update);
};