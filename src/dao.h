#pragma once

#include "builder/QueryBuilder.h"

#include "ConnectionPool.h"
#include "DbLoader.h"

#include "query/Insert.h"

class dao {
public:
    template<typename T>
    static QueryBuilder<T, Insert> _insert() {
        return QueryBuilder<T, Insert>();
    }
};
