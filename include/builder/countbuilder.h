#pragma once

#include "option/debugbuilder.h"
#include "option/filterbuilder.h"
#include "option/fromselectbuilder.h"

#include "selectbuilder.h"

#include "../macro/macro.h"

QTDAO_BEGIN_NAMESPACE

template<typename E>
class CountBuilder
        : public DebugBuilder<CountBuilder<E>>
        , public FilterBuilder<CountBuilder<E>>
        , public FromSelfSelectBuilder<CountBuilder, E>
{
public:
    int count() {
        SelectBuilder<E> builder;
        builder.column(FunctionConnector("count(*) as __selectcount"));
        builder.setFatalEnabled = this->setFatalEnabled;
        builder.loggingCategoryPtr = this->loggingCategoryPtr;
        builder.filterCondition = this->filterCondition;
        builder.fromData = this->fromData;

        QList<E> data = builder.build().list();
        if (data.isEmpty()) {
            return 0;
        } 
        return data[0].template __getExtra<int>("__selectcount");
    }
};

QTDAO_END_NAMESPACE