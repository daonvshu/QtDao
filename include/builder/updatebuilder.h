#pragma once

#include "basequerybuilder.h"

#include "../macro/macro.h"

#include "../query/update.h"

template<typename E>
class UpdateBuilder : public BaseQueryBuilder {
public:
    QUERY_BUILDER_USE_THROWABLE(UpdateBuilder)
    QUERY_BUILDER_USE_SET(UpdateBuilder)
    QUERY_BUILDER_USE_FILTER(UpdateBuilder)

    QUERY_BUILDER_BUILDER_DECLARE(Update)
};
