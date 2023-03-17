#pragma once

#include "basequery.h"
#include "reader/entityreaderinterface.h"
#include "reader/builderreaderinterface.h"

QTDAO_BEGIN_NAMESPACE

class InsertIntoSelectImpl
        : protected BaseQuery
        , protected virtual EntityReaderInterface
        , protected virtual BuilderReaderInterface
{
public:
    using BaseQuery::BaseQuery;

protected:
    void buildSqlStatement();
};

QTDAO_END_NAMESPACE