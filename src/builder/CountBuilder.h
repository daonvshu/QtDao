﻿#pragma once

#include "SelectBuilder.h"

#include "../macro/macro.h"

template<typename E>
class CountBuilder : SelectBuilder<E> {
public:
    CountBuilder() {
        column(FunctionCondition("count(*) as __selectcount"));
    }

    QUERY_BUILDER_USE_THROWABLE(CountBuilder);
    QUERY_BUILDER_USE_FILTER(CountBuilder);

    QUERY_BUILDER_USE_QUERY_FROM_SELECT(CountBuilder);
    QUERY_BUILDER_USE_QUERY_FROM_JOIN2(CountBuilder);
    
    int count() {
        QList<E> data = build().list();
        if (data.isEmpty()) {
            return 0;
        } 
        return data[0].__getExtra("__selectcount").toInt();
    }
};