#pragma once

#include "basequerybuilder.h"

#include "../macro/macro.h"

#include "../query/delete.h"

QTDAO_BEGIN_NAMESPACE

template<typename E>
class DeleteBuilder : public BaseQueryBuilder {
public:
    QUERY_BUILDER_USE_FATAL_DISABLE(DeleteBuilder)
    QUERY_BUILDER_SET_LOGGING(DeleteBuilder)

    QUERY_BUILDER_USE_FILTER(DeleteBuilder)

    QUERY_BUILDER_BUILDER_DECLARE(Delete)
};

QTDAO_END_NAMESPACE