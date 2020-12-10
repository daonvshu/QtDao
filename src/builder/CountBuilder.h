#pragma once

#include "SelectBuilder.h"

#include "../macro/macro.h"

template<typename T>
class CountBuilder : SelectBuilder<T> {
public:
    CountBuilder() {
        column(FunctionCondition("count(*) as __selectcount"));
    }

    QUERY_BUILDER_USE_THROWABLE(CountBuilder);
    QUERY_BUILDER_USE_FILTER(CountBuilder);

    QUERY_BUILDER_USE_QUERY_FROM(CountBuilder);
    
    int count() {
        QList<T> data = build().list();
        if (data.isEmpty()) {
            return 0;
        } 
        return data[0].__getExtra("__selectcount").toInt();
    }
};