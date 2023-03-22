#pragma once

#include "option/debugbuilder.h"
#include "option/columnbuilder.h"
#include "option/filterbuilder.h"
#include "option/constraintbuilder.h"
#include "option/fromselectbuilder.h"
#include "option/fromjoinbuilder.h"
#include "option/unionbuilder.h"

#include "../macro/macro.h"

#include "../query/select.h"

QTDAO_BEGIN_NAMESPACE

template<typename E>
class CountBuilder;

template<typename E>
class SelectBuilder
        : public DebugBuilder<SelectBuilder<E>>
        , public ColumnBuilder<SelectBuilder<E>>
        , public FilterBuilder<SelectBuilder<E>>
        , public ConstraintBuilder<SelectBuilder<E>>
        , public FromSelfSelectBuilder<SelectBuilder, E>
        , public UnionBuilder<SelectBuilder<E>>
{
public:
    Select <E> build() {
        return Select<E>(this);
    }

    friend class CountBuilder<E>;
};

QTDAO_END_NAMESPACE