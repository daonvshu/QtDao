#pragma once

#include "basequerybuilder.h"

#include "../macro/macro.h"

#include "../query/upsert.h"

QTDAO_BEGIN_NAMESPACE

template<typename E>
class UpsertBuilder : public BaseQueryBuilder {
public:
    QUERY_BUILDER_USE_FATAL_DISABLE(UpsertBuilder)
    QUERY_BUILDER_SET_LOGGING(UpsertBuilder)

    QUERY_BUILDER_USE_SET(UpsertBuilder)

    QUERY_BUILDER_USE_CONFLICT_COLUMNS(UpsertBuilder)

    QUERY_BUILDER_USE_UPDATE_COLUMNS(UpsertBuilder)

    //only supported for sqlite
    QUERY_BUILDER_USE_FILTER(UpsertBuilder)

    QUERY_BUILDER_BUILDER_DECLARE(Upsert)
};

QTDAO_END_NAMESPACE