#include "BaseQuery.h"

#include "../ConnectionPool.h"
#include "../DbExceptionHandler.h"

BaseQuery::BaseQuery()
    : queryThrowable(false)
{
}

void BaseQuery::exec(const std::function<void(const QSqlQuery&)>& solveQueryResult) {
    auto query = getQuery();
    if (query.exec()) {
        solveQueryResult(query);
    } else {
        if (queryThrowable) {
            throw DaoException(query.lastError().text());
        } else {
            DbExceptionHandler::exceptionHandler->execFail(query.lastError().text());
        }
    }
}

void BaseQuery::execBatch(const std::function<void(const QSqlQuery&)>& solveQueryResult) {
    auto query = getQuery();
    if (query.execBatch()) {
        solveQueryResult(query);
    } else {
        if (queryThrowable) {
            throw DaoException(query.lastError().text());
        } else {
            DbExceptionHandler::exceptionHandler->execFail(query.lastError().text());
        }
    }
}

void BaseQuery::queryPrimitive(const QString& statement, std::function<void(QSqlQuery& query)> callback, std::function<void(QString)> failCallback) {
    queryPrimitive(statement, QVariantList(), callback, failCallback);
}

void BaseQuery::queryPrimitive(const QString& statement, const QVariantList& values, std::function<void(QSqlQuery& query)> callback, std::function<void(QString)> failCallback) {
    BaseQuery executor;
    executor.setSqlQueryStatement(statement, values);
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
    BaseQuery executor;
    executor.setSqlQueryStatement(statement, values);
    auto query = executor.getQuery();
    if (query.exec()) {
        return query;
    } else {
        throw DaoException(query.lastError().text());
    }
}

void BaseQuery::setSqlQueryStatement(const QString& statement, const QVariantList& values) {
    this->statement = statement;
    this->values = values;
    if (getQueryLogPrinter()) {
        getQueryLogPrinter()(statement, values);
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
