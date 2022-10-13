#pragma once

#include "basequery.h"
#include "entityreaderinterface.h"

QTDAO_BEGIN_NAMESPACE

class InsertImpl : protected BaseQuery, protected virtual EntityReaderInterface {
public:
    using BaseQuery::BaseQuery;

protected:
    bool buildInsertBySetSqlStatement();

    QString buildInsertObjectSqlStatement();

    QString buildInsertObjectsSqlStatement();

    QString buildInsertObjects2SqlStatement(int valueSize);

protected:
    bool insertOrRp = false;
};

QTDAO_END_NAMESPACE