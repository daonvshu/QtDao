#pragma once

#include "option/debugbuilder.h"
#include "option/filterbuilder.h"
#include "option/setbuilder.h"

#include "../macro/macro.h"

#include "../query/upsert.h"

QTDAO_BEGIN_NAMESPACE

template<typename E>
class UpsertBuilder
        : public DebugBuilder<UpsertBuilder<E>>
        , public FilterBuilder<UpsertBuilder<E>> //only supported for sqlite
        , public SetBuilder<UpsertBuilder<E>>
{
public:
    template<typename Col, typename... Args>
    UpsertBuilder& conflictColumns(const Col& field, const Args&... args) {
        conflictCols.appendCol(field);
        return conflictColumns(args...);
    }

    template<typename Col>
    UpsertBuilder& conflictColumns(bool enabled, const Col& field) {
        if (enabled) {
            conflictCols.appendCol(field);
        }
        return *this;
    }

    template<typename Col, typename... Args>
    UpsertBuilder& updateColumns(const Col& field, const Args&... args) {
        updateCols.appendCol(field);
        return updateColumns(args...);
    }

    template<typename Col>
    UpsertBuilder& updateColumns(bool enabled, const Col& field) {
        if (enabled) {
            updateCols.appendCol(field);
        }
        return *this;
    }

    void conflictColumns() {}

    void updateColumns() {}

    Upsert <E> build() {
        return Upsert<E>(this->setFatalEnabled, this, this->loggingCategoryPtr);
    }

private:
    Connector conflictCols, updateCols;
};

QTDAO_END_NAMESPACE