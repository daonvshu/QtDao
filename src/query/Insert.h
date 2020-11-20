#pragma once

#include "BaseQuery.h"

template<typename E>
class Insert {
public:
    void insert(E& entity);
    void insert(QList<E>& entities);    
}

