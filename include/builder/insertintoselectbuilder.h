#pragma once

#include "option/debugbuilder.h"
#include "option/columnbuilder.h"
#include "option/fromselectbuilder.h"
#include "option/fromjoinbuilder.h"

#include "../macro/macro.h"

#include "../query/insertintoselect.h"

QTDAO_BEGIN_NAMESPACE

template<typename E>
class InsertIntoSelectBuilder
        : public DebugBuilder<InsertIntoSelectBuilder<E>>
        , public ColumnBuilder<InsertIntoSelectBuilder<E>>
        , public FromSelectBuilder<false, InsertIntoSelectBuilder, E>
        , public FromJoinBuilder<false, InsertIntoSelectBuilder, E>
{
public:
    InsertIntoSelect <E> build() {
        return InsertIntoSelect<E>(this->setFatalEnabled, this, this->loggingCategoryPtr);
    }
};

QTDAO_END_NAMESPACE