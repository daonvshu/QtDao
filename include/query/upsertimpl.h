#pragma once

#include "basequery.h"
#include "reader/entityreaderinterface.h"
#include "reader/builderreaderinterface.h"

QTDAO_BEGIN_NAMESPACE

class UpsertImpl
        : protected BaseQuery
        , protected virtual EntityReaderInterface
        , protected virtual BuilderReaderInterface
{
public:
    using BaseQuery::BaseQuery;

protected:
    bool buildInsertBySetSqlStatement();

    QString buildInsertStatement(const QStringList& fields, const std::function<QVariant(const QString&)>& fieldToValue);
};

QTDAO_END_NAMESPACE