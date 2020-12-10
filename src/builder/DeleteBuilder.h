#pragma once

#include "BaseQueryBuilder.h"

#include "../macro/macro.h"

#include "../query/Delete.h"

template<typename T>
class DeleteBuilder : public BaseQueryBuilder {
public:
    QUERY_BUILDER_USE_THROWABLE(DeleteBuilder);
    QUERY_BUILDER_USE_FILTER(DeleteBuilder);

    QUERY_BUILDER_BUILDER_DECLARE(Delete);
};
