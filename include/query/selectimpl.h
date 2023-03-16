#pragma once

#include "basequery.h"
#include "entityreaderinterface.h"

QTDAO_BEGIN_NAMESPACE

class FromBuilder;
class SelectImpl : protected BaseQuery, protected virtual EntityReaderInterface {
public:
    using BaseQuery::BaseQuery;

protected:
    void buildFilterSqlStatement();

    QString getBindColumns(QVariantList& values);

    QString readExplainStatement();

    //----- executors -----
    typedef std::function<void(const QString&, const QVariant&)> EntityBinder;

    static void recordBind(const QSqlRecord& record, const EntityBinder& entityBinder);

    void uniqueExec(const EntityBinder& entityBinder);

    void oneExec(const EntityBinder& entityBinder);

    void listExec(const std::function<void(const QSqlRecord&)>& recordHandler);

protected:
    int topSize = 0;
    bool topPercent = false;

    friend class FromBuilder;
};

QTDAO_END_NAMESPACE
