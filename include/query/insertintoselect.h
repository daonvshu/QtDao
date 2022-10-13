#pragma once

#include "insertintoselectimpl.h"

#include "../macro/macro.h"

QTDAO_BEGIN_NAMESPACE

template<typename E>
class InsertIntoSelectBuilder;

template<typename E>
class InsertIntoSelect : EntityReaderProvider<E>, InsertIntoSelectImpl {
public:
    /// <summary>
    /// select and copy from other select results
    /// </summary>
    void insert();

private:
    BASE_QUERY_CONSTRUCTOR_DECLARE_W(InsertIntoSelect)
};

template<typename E>
inline void InsertIntoSelect<E>::insert() {
    buildSqlStatement();
    exec();
}

QTDAO_END_NAMESPACE