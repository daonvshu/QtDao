#pragma once

#include "frombuilder.h"

QTDAO_BEGIN_NAMESPACE

template<typename T>
class FromRecursiveBuilder : FromBuilder {
public:
    T& from(RecursiveQueryBuilder& builder) {
        fromBuilder(builder);
        return static_cast<T&>(*this);
    }

    T& from(RecursiveQueryBuilder&& builder) {
        fromBuilder(builder);
        return static_cast<T&>(*this);
    }
};

QTDAO_END_NAMESPACE