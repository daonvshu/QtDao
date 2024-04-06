#pragma once

#include "insertintoselectimpl.h"

#include "../macro/macro.h"

QTDAO_BEGIN_NAMESPACE

template<typename E>
class InsertIntoSelectBuilder;

template<typename E>
class InsertIntoSelect
        : EntityReaderProvider<E>
        , BuilderReaderProvider<InsertIntoSelectBuilder, E>
        , InsertIntoSelectImpl
{
public:
    /**
     * select and copy from other select results
     */
    void insert();

private:
    BASE_QUERY_CONSTRUCTOR_DECLARE(InsertIntoSelect)
};

template<typename E>
inline void InsertIntoSelect<E>::insert() {
    buildSqlStatement();
    setDebug(this->builder);
    exec(getSessionId());
}

QTDAO_END_NAMESPACE