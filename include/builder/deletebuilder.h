#pragma once

#include "basequerybuilder.h"

#include "../macro/macro.h"

#include "../query/delete.h"

template<typename E>
class DeleteBuilder : public BaseQueryBuilder {
public:
    QUERY_BUILDER_USE_THROWABLE(DeleteBuilder)
    QUERY_BUILDER_USE_FILTER(DeleteBuilder)

    QUERY_BUILDER_BUILDER_DECLARE(Delete)
};
