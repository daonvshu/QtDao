#pragma once

#include "basequerybuilder.h"

#include "../macro/macro.h"

#include "../query/insertintoselect.h"

QTDAO_BEGIN_NAMESPACE

template<typename E>
class InsertIntoSelectBuilder : public BaseQueryBuilder {
public:
    QUERY_BUILDER_USE_FATAL_DISABLE(InsertIntoSelectBuilder)
    QUERY_BUILDER_SET_LOGGING(InsertIntoSelectBuilder)

    QUERY_BUILDER_USE_COLUMN(InsertIntoSelectBuilder)

    QUERY_BUILDER_USE_QUERY_FROM_SELECT2(InsertIntoSelectBuilder)
    QUERY_BUILDER_USE_QUERY_FROM_JOIN2(InsertIntoSelectBuilder)

    QUERY_BUILDER_BUILDER_DECLARE(InsertIntoSelect)
};

QTDAO_END_NAMESPACE