#pragma once

#include "option/debugbuilder.h"
#include "option/setbuilder.h"

#include "../macro/macro.h"

#include "../query/insert.h"

QTDAO_BEGIN_NAMESPACE

template<typename E>
class InsertBuilder
        : public DebugBuilder<InsertBuilder<E>>
        , public SetBuilder<InsertBuilder<E>>
{
public:
    Insert <E> build() {
        return Insert<E>(this);
    }
};

QTDAO_END_NAMESPACE