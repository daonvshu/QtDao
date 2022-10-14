#pragma once

#include "basequerybuilder.h"

#include "../macro/macro.h"

#include "../query/insert.h"

QTDAO_BEGIN_NAMESPACE

template<typename E>
class InsertBuilder : public BaseQueryBuilder {
public:
    QUERY_BUILDER_USE_FATAL_DISABLE(InsertBuilder)
    QUERY_BUILDER_USE_SET(InsertBuilder)

    QUERY_BUILDER_BUILDER_DECLARE(Insert)
};

QTDAO_END_NAMESPACE