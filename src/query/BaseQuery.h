#pragma once

#include <qvariant.h>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>

#include <functional>

#include "../condition/Connector.h"
#include "../builder/BaseQueryBuilder.h"

class BaseQuery {
public:
    BaseQuery(bool throwable = false, BaseQueryBuilder* builder = nullptr);
    BaseQuery(const BaseQuery& other);
    ~BaseQuery();

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
    void printWarning(const QString& info);

protected:
    QString statement;
    QVariantList values;
    BaseQueryBuilder* builder;
    bool queryThrowable;

    friend class BaseQueryBuilder;

private:
    QSqlQuery getQuery();
    void bindQueryValues(QSqlQuery& query);
};

