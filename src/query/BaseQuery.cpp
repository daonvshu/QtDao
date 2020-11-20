#include "BaseQuery.h"

#include "../ConnectionPool.h"
#include "../DbExceptionHandler.h"

BaseQuery::BaseQuery(const QString& statement, const QVariantList& values)
    : statement(statement)
    , values(values)
{
    if (getQueryLogPrinter()) {
        getQueryLogPrinter()(statement, values);
    }
}

void BaseQuery::exec() {
    auto query = getQuery();
    if (query.exec()) {
        solveQueryResult(query);
    } else {
        DbExceptionHandler::exceptionHandler->execFail(query.lastError().text());
    }
}

void BaseQuery::execBatch() {
    auto query = getQuery();
    if (query.execBatch()) {
        solveQueryResult(query);
    } else {
        DbExceptionHandler::exceptionHandler->execFail(query.lastError().text());
    }
}

void BaseQuery::queryPrimitive(const QString& statement, QueryCallback callback, QueryFailCallback failCallback) {
    queryPrimitive(statement, QVariantList(), callback, failCallback);
}

void BaseQuery::queryPrimitive(const QString& statement, const QVariantList& values, QueryCallback callback, QueryFailCallback failCallback) {
    BaseQuery executor(statement, values);
    auto query = executor.getQuery();
    if (query.exec()) {
        if (callback) {
            callback(query);
        }
    } else {
        if (failCallback) {
            failCallback(query.lastError().text());
        } else {
            DbExceptionHandler::exceptionHandler->execFail(query.lastError().text());
        }
    }
}

QSqlQuery BaseQuery::queryPrimitiveThrowable(const QString& statement) {
    return queryPrimitiveThrowable(statement, QVariantList());
}

QSqlQuery BaseQuery::queryPrimitiveThrowable(const QString& statement, const QVariantList& values) {
    BaseQuery executor(statement, values);
    auto query = executor.getQuery();
    if (query.exec()) {
        return query;
    } else {
        throw DaoException(query.lastError().text());
    }
}

QSqlQuery BaseQuery::getQuery() {
    auto db = ConnectionPool::getConnection();
    QSqlQuery query(db);
    query.prepare(statement);
    bindQueryValues(query);
    return query;
}

void BaseQuery::bindQueryValues(QSqlQuery& query) {
    for (const auto& d : values) {
        query.addBindValue(d, d.type() == QMetaType::QByteArray ? QSql::Binary : QSql::In);
    }
}
