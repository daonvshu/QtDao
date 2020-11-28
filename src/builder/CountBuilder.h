#pragma once

#include "SelectBuilder.h"

template<typename T>
class CountBuilder : SelectBuilder<T> {
public:
    CountBuilder() {
        column(FunctionCondition("count(*) as __selectcount"));
    }

    template<typename... Args>
    CountBuilder& filter(const EntityCondition& condition, const Args&... args) {
        __super::filter(condition, args...);
        return *this;
    }

    template<typename... Args>
    CountBuilder& filter(const Connector& condition, const Args&... args) {
        __super::filter(condition, args...);
        return *this;
    }
    
    int count() {
        QList<T> data = build().list();
        if (data.isEmpty()) {
            return 0;
        } 
        return data[0].__getExtra("__selectcount").toInt();
    }
};