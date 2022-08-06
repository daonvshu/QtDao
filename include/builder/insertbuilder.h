#pragma once

#include "basequerybuilder.h"

#include "../macro/macro.h"

#include "../query/insert.h"

template<typename E>
class InsertBuilder : public BaseQueryBuilder {
public:
    QUERY_BUILDER_USE_THROWABLE(InsertBuilder)
    QUERY_BUILDER_USE_SET(InsertBuilder)

    QUERY_BUILDER_BUILDER_DECLARE(Insert)
};
