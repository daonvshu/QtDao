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

    QUERY_BUILDER_BUILDER_DECLARE(Upsert)

    template<typename... Args>
    UpsertBuilder<E>& conflictColumns(const EntityField<E>& field, const Args&... fields) {
        conflictCols.appendCol(field);
        conflictColumns(fields...);
    }

    UpsertBuilder<E>& conflictColumns(const EntityField<E>& field) {
        conflictCols.appendCol(field);
    }

    template<typename... Args>
    UpsertBuilder<E>& updateColumns(const EntityField<E>& field, const Args&... fields) {
        updateCols.appendCol(field);
        updateColumns(fields...);
    }

    UpsertBuilder<E>& updateColumns(const EntityField<E>& field) {
        updateCols.appendCol(field);
    }

private:
    Connector conflictCols, updateCols;

    friend class Upsert<E>;
};

QTDAO_END_NAMESPACE