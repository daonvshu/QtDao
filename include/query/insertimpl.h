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
    enum class InsertMode {
        INSERT_ONLY,
        INSERT_OR_REPLACE,
        INSERT_OR_IGNORE,
    };

    InsertMode insertMode = InsertMode::INSERT_ONLY;

private:
    QString buildInsertPrefix();
};

QTDAO_END_NAMESPACE