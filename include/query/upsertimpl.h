#pragma once

#include "basequery.h"
#include "entityreaderinterface.h"

QTDAO_BEGIN_NAMESPACE

class UpsertImpl : protected BaseQuery, protected virtual EntityReaderInterface {
public:
    using BaseQuery::BaseQuery;

protected:
    bool buildInsertBySetSqlStatement();

    QString buildInsertStatement(const QStringList& fields, const std::function<QVariant(const QString&)>& fieldToValue);

protected:
    Connector conflictCols, updateCols;
};

QTDAO_END_NAMESPACE