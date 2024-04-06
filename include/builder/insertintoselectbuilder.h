#pragma once

#include "option/debugbuilder.h"
#include "option/columnbuilder.h"
#include "option/fromselectbuilder.h"
#include "option/sessionbuilder.h"

#include "../macro/macro.h"

#include "../query/insertintoselect.h"

QTDAO_BEGIN_NAMESPACE

template<typename E>
class InsertIntoSelectBuilder
    : public DebugBuilder<InsertIntoSelectBuilder<E>>
    , public ColumnBuilder<InsertIntoSelectBuilder<E>>
    , public FromE2SelectBuilder<InsertIntoSelectBuilder<E>>
    , public SessionBuilder<InsertIntoSelectBuilder<E>>
{
public:
    using SessionBuilder<InsertIntoSelectBuilder<E>>::SessionBuilder;

    InsertIntoSelect <E> build() {
        return InsertIntoSelect<E>(*this);
    }
};

QTDAO_END_NAMESPACE