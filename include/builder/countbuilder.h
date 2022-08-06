#pragma once

#include "selectbuilder.h"

#include "../macro/macro.h"

template<typename E>
class CountBuilder : SelectBuilder<E> {
public:
    CountBuilder() {
        this->column(FunctionCondition("count(*) as __selectcount"));
    }

    QUERY_BUILDER_USE_THROWABLE(CountBuilder)
    QUERY_BUILDER_USE_FILTER(CountBuilder)

    QUERY_BUILDER_USE_QUERY_FROM_SELECT(CountBuilder)
    QUERY_BUILDER_USE_QUERY_FROM_JOIN2(CountBuilder)
    QUERY_BUILDER_USE_QUERY_FROM_RECURSIVE(CountBuilder)
    
    int count() {
        QList<E> data = this->build().list();
        if (data.isEmpty()) {
            return 0;
        } 
        return data[0].__getExtra("__selectcount").toInt();
    }
};
