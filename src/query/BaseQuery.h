#pragma once

#include <qvariant.h>
#include <qsqlquery.h>

#include <functional>

class BaseQuery {
public:
    BaseQuery(const QString& statement, const QVariantList& values);

    void exec();
    void execBatch();

    static void queryPrimitive(const QString& statement, std::function<void(QSqlQuery& query)> callback);

protected:
    virtual void solveQueryResult(const QSqlQuery& query) {};

private:
    const QString statement;
    const QVariantList* values;

private:
    QSqlQuery getQuery();
    void bindQueryValues(QSqlQuery& query);
};

