#pragma once

#include "ConnectionPool.h"
#include "DbLoader.h"

#include "builder/InsertBuilder.h"
#include "builder/SelectBuilder.h"
#include "builder/CountBuilder.h"

#include "condition/ConditionOperator.h"

class dao {
public:
    template<typename T>
    static InsertBuilder<T> _insert() {
        return InsertBuilder<T>();
    }

    template<typename T>
    static SelectBuilder<T> _select() {
        return SelectBuilder<T>();
    }

    template<typename T>
    static QList<T> _selectAll() {
        return SelectBuilder<T>().build().list();
    }

    template<typename T>
    static CountBuilder<T> _count() {
        return CountBuilder<T>();
    }
};
