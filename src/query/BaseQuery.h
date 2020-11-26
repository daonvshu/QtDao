#pragma once

#include <qvariant.h>
#include <qsqlquery.h>
#include <qsqlrecord.h>

#include <functional>

#include "../condition/Connector.h"

template<typename T, template<typename> class Query>
class QueryBuilder;

class BaseQuery {
public:
    BaseQuery();

    static void queryPrimitive(
        const QString& statement, 
        std::function<void(QSqlQuery& query)> callback = nullptr,
        std::function<void(QString)> failCallback = nullptr
    );

    static void queryPrimitive(
        const QString& statement,
        const QVariantList& values,
        std::function<void(QSqlQuery& query)> callback = nullptr,
        std::function<void(QString)> failCallback = nullptr
    );

    static QSqlQuery queryPrimitiveThrowable(
        const QString& statement
    );

    static QSqlQuery queryPrimitiveThrowable(
        const QString& statement,
        const QVariantList& values
    );

protected:
    void setSqlQueryStatement(const QString& statement, const QVariantList& values);

    void exec(const std::function<void(QSqlQuery&)>& solveQueryResult);
    void execBatch(const std::function<void(QSqlQuery&)>& solveQueryResult);

    void printException(const QString& reason);

private:
    QString statement;
    QVariantList values;

protected:
    bool queryThrowable;

    template<typename T, template<typename> class Query>
    friend class QueryBuilder;

private:
    QSqlQuery getQuery();
    void bindQueryValues(QSqlQuery& query);
};

