#pragma once

#include "option/debugbuilder.h"
#include "option/columnbuilder.h"
#include "option/filterbuilder.h"
#include "option/setbuilder.h"

#include "../macro/macro.h"

#include "../query/upsert.h"

QTDAO_BEGIN_NAMESPACE

template<typename E>
class UpsertBuilder
        : public DebugBuilder<UpsertBuilder<E>>
        , public ConflictColumnBuilder<UpsertBuilder<E>>
        , public UpdateColumnBuilder<UpsertBuilder<E>>
        , public FilterBuilder<UpsertBuilder<E>> //only supported for sqlite
        , public SetBuilder<UpsertBuilder<E>>
{
public:
    Upsert <E> build() {
        return Upsert<E>(*this);
    }
};

QTDAO_END_NAMESPACE