#pragma once

#include <qvariant.h>
#include <qsqlquery.h>

#include <functional>

typedef std::function<void(QSqlQuery& query)> QueryCallback;
typedef std::function<void(QString)> QueryFailCallback;

class BaseQuery {
public:
    BaseQuery(const QString& statement, const QVariantList& values);

    void exec();
    void execBatch();

    static void queryPrimitive(
        const QString& statement, 
        QueryCallback callback = nullptr,
        QueryFailCallback failCallback = nullptr
    );

    static void queryPrimitive(
        const QString& statement,
        const QVariantList& values,
        QueryCallback callback = nullptr,
        QueryFailCallback failCallback = nullptr
    );

    static QSqlQuery queryPrimitiveThrowable(
        const QString& statement
    );

    static QSqlQuery queryPrimitiveThrowable(
        const QString& statement,
        const QVariantList& values
    );

protected:
    virtual void solveQueryResult(const QSqlQuery& query) {};

private:
    const QString statement;
    const QVariantList* values;

private:
    QSqlQuery getQuery();
    void bindQueryValues(QSqlQuery& query);
};

