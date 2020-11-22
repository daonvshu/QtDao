#pragma once

#include "BaseQuery.h"

template<typename E>
class Select : public BaseQuery {
public:
    E unique();
    E one();
    QList<E> list();
};