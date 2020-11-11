#include "BaseQuery.h"

#include "../ConnectionPool.h"
#include "../DbExceptionHandler.h"

BaseQuery::BaseQuery(const QString& statement, const QVariantList& values)
    : statement(statement)
    , values(&values)
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

void BaseQuery::queryPrimitive(const QString& statement, std::function<void(QSqlQuery& query)> callback) {
    BaseQuery executor(statement, QVariantList());
    auto query = executor.getQuery();
    if (query.exec()) {
        callback(query);
    } else {
        DbExceptionHandler::exceptionHandler->execFail(query.lastError().text());
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
    for (const auto& d : *values) {
        query.addBindValue(d, d.type() == QMetaType::QByteArray ? QSql::Binary : QSql::In);
    }
}
