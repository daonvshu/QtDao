#pragma once

#include "option/debugbuilder.h"
#include "option/filterbuilder.h"
#include "option/fromselectbuilder.h"
#include "option/fromjoinbuilder.h"
#include "option/fromrecursivebuilder.h"

#include "selectbuilder.h"

#include "../macro/macro.h"

QTDAO_BEGIN_NAMESPACE

template<typename E>
class CountBuilder
        : SelectBuilder<E>
        , public DebugBuilder<CountBuilder<E>>
        , public FilterBuilder<CountBuilder<E>>
        , public FromSelectBuilder<true, CountBuilder, E>
        , public FromJoinBuilder<false, CountBuilder, E>
        , public FromRecursiveBuilder<CountBuilder<E>>
{
public:
    CountBuilder() {
        this->column(FunctionCondition("count(*) as __selectcount"));
    }

    int count() {
        QList<E> data = this->build().list();
        if (data.isEmpty()) {
            return 0;
        } 
        return data[0].__getExtra("__selectcount").toInt();
    }
};

QTDAO_END_NAMESPACE