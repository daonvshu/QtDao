#pragma once

#include "builder/InsertBuilder.h"

#include "ConnectionPool.h"
#include "DbLoader.h"

#include "query/Insert.h"

class dao {
public:
    template<typename T>
    static InsertBuilder<T> _insert() {
        return InsertBuilder<T>();
    }
};
