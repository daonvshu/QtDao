#include "BaseQuery.h"

#include "../ConnectionPool.h"
#include "../DbExceptionHandler.h"

#include "../DbLoader.h"

#include <iostream>
#include <QThread>

QMutex BaseQuery::writeCheckLocker;
QWaitCondition BaseQuery::writeWait;
bool BaseQuery::currentIsWriting = false;
bool BaseQuery::sqlWriteLock = false;

BaseQuery::BaseQuery(bool throwable, BaseQueryBuilder* builder)
    : builder(builder)
    , queryThrowable(throwable)
{
    if (builder != nullptr) {
        this->builder = new BaseQueryBuilder(*builder);
    }
}

BaseQuery::BaseQuery(const BaseQuery& other) {
    this->statement = other.statement;
    this->values = other.values;
    this->builder = new BaseQueryBuilder(*other.builder);
    this->queryThrowable = other.queryThrowable;
}

BaseQuery::~BaseQuery() {
    if (builder != nullptr) {
        delete builder;
        builder = nullptr;
    }
}

void BaseQuery::exec(const std::function<void(QSqlQuery&)>& solveQueryResult) {
    bool prepareOk;
    auto query = getQuery(prepareOk, true);
    if (prepareOk && execByCheckEmptyValue(query, this)) {
        solveQueryResult(query);
        checkAndReleaseWriteLocker();
    } else {
        checkAndReleaseWriteLocker();
        printException(query.lastError().text());
    }
}

void BaseQuery::execBatch(const std::function<void(QSqlQuery&)>& solveQueryResult) {
    bool prepareOk;
    auto query = getQuery(prepareOk);
    if (prepareOk && query.execBatch()) {
        solveQueryResult(query);
        checkAndReleaseWriteLocker();
    } else {
        checkAndReleaseWriteLocker();
        printException(query.lastError().text());
    }
}

void BaseQuery::printException(const QString& reason) {
    if (queryThrowable) {
        throw DaoException(reason);
    } else {
        if (DbExceptionHandler::exceptionHandler) {
            DbExceptionHandler::exceptionHandler->execFail(reason);
            Q_ASSERT(DbExceptionHandler::exceptionHandler != nullptr);
        }
    }
}

void BaseQuery::printWarning(const QString& info) {
    if (DbExceptionHandler::exceptionHandler) {
        DbExceptionHandler::exceptionHandler->execWarning(info);
    }
    Q_ASSERT(DbExceptionHandler::exceptionHandler != nullptr);
}

void BaseQuery::checkAndLockWrite() {
    if (sqlWriteLock) {
        std::cout << "prepare check get locker! " << QThread::currentThreadId() << std::endl;
        QMutexLocker locker(&writeCheckLocker);
        if (sqlWriteLock) {
            if (currentIsWriting) {
                std::cout << "current wait locker! " << QThread::currentThreadId() << std::endl;
                writeWait.wait(&writeCheckLocker);
            }
            std::cout << "current catch locker! to set writing! " << QThread::currentThreadId() << std::endl;
            currentIsWriting = true;
        }
    }
}

void BaseQuery::checkAndReleaseWriteLocker() {
    if (sqlWriteLock) {
        std::cout << "prepare release write lock! " << QThread::currentThreadId() << std::endl;
        QMutexLocker locker(&writeCheckLocker);
        if (sqlWriteLock) {
            currentIsWriting = false;
            writeWait.notify_one();
        }
    }
}

void BaseQuery::queryPrimitive(const QString& statement, std::function<void(QSqlQuery& query)> callback, std::function<void(QString)> failCallback) {
    queryPrimitive(statement, QVariantList(), callback, failCallback);
}

void BaseQuery::queryPrimitive(const QString& statement, const QVariantList& values, std::function<void(QSqlQuery& query)> callback, std::function<void(QString)> failCallback) {
    BaseQuery executor;
    executor.setSqlQueryStatement(statement, values);
    bool prepareOk;
    auto query = executor.getQuery(prepareOk, true);
    if (prepareOk && execByCheckEmptyValue(query, &executor)) {
        if (callback) {
            callback(query);
        }
    } else {
        auto lastErr = query.lastError().text();
        if (failCallback) {
            failCallback(lastErr);
        } else {
            if (DbExceptionHandler::exceptionHandler) {
                DbExceptionHandler::exceptionHandler->execFail(lastErr);
            }
            Q_ASSERT(DbExceptionHandler::exceptionHandler != nullptr);
        }
    }
    checkAndReleaseWriteLocker();
}

QSqlQuery BaseQuery::queryPrimitiveThrowable(const QString& statement) {
    return queryPrimitiveThrowable(statement, QVariantList());
}

QSqlQuery BaseQuery::queryPrimitiveThrowable(const QString& statement, const QVariantList& values) {
    BaseQuery executor;
    executor.setSqlQueryStatement(statement, values);
    bool prepareOk;
    auto query = executor.getQuery(prepareOk, true);
    if (prepareOk && execByCheckEmptyValue(query, &executor)) {
        checkAndReleaseWriteLocker();
        return query;
    } else {
        checkAndReleaseWriteLocker();
        throw DaoException(query.lastError().text());
    }
}

void BaseQuery::setSqlQueryStatement(const QString& statement, const QVariantList& values) {
    this->statement = statement;
    this->values = values;
}

QSqlQuery BaseQuery::getQuery(bool& prepareOk, bool skipEmptyValue) {
    checkAndLockWrite();
    auto db = ConnectionPool::getConnection();
    QSqlQuery query(db);
    if (!skipEmptyValue || !values.isEmpty()) {
        if (!query.prepare(statement)) {
            prepareOk = false;
            return query;
        }
        bindQueryValues(query);
    }
    if (getQueryLogPrinter()) {
        getQueryLogPrinter()(statement, values);
    }
    prepareOk = true;
    return query;
}

void BaseQuery::bindQueryValues(QSqlQuery& query) {
    for (const auto& d : values) {
        query.addBindValue(d, d.type() == QVariant::ByteArray ? QSql::Binary : QSql::In);
    }
}

bool BaseQuery::execByCheckEmptyValue(QSqlQuery& query, const BaseQuery* executor) {
    if (!executor->values.isEmpty()) {
        return query.exec();
    }
    return query.exec(executor->statement);
}

QList<SqliteExplainInfo> BaseQuery::ExplainTool<SqliteExplainInfo>::toExplain(const QString& statement) {
    Q_ASSERT_X(DbLoader::getConfig().isSqlite(), "ExplainTool<SqliteExplainInfo>", "need config sqlite");
    QSqlQuery query = BaseQuery::queryPrimitiveThrowable("explain " + statement);
    QList<SqliteExplainInfo> result;
    while (query.next()) {
        SqliteExplainInfo info;
        info.addr = query.value("addr").toLongLong();
        info.opcode = query.value("opcode").toString();
        info.p1 = query.value("p1").toLongLong();
        info.p2 = query.value("p2").toLongLong();
        info.p3 = query.value("p3").toLongLong();
        info.p4 = query.value("p4").toString();
        info.p5 = query.value("p5").toString();
        info.comment = query.value("comment").toString();
        result << info;
    }
    return result;
}

QList<SqliteExplainQueryPlanInfo> BaseQuery::ExplainTool<SqliteExplainQueryPlanInfo>::toExplain(
    const QString& statement) {
    Q_ASSERT_X(DbLoader::getConfig().isSqlite(), "ExplainTool<SqliteExplainQueryPlanInfo>", "need config sqlite");
    QSqlQuery query = BaseQuery::queryPrimitiveThrowable("explain query plan " + statement);
    QList<SqliteExplainQueryPlanInfo> result;
    while (query.next()) {
        SqliteExplainQueryPlanInfo info;
        QSqlRecord record = query.record();
        for (int i = 0; i < record.count(); i++) {
            info.data << qMakePair(record.fieldName(i), record.value(i));
        }
        result << info;
    }
    return result;
}

QList<MysqlExplainInfo> BaseQuery::ExplainTool<MysqlExplainInfo>::toExplain(const QString& statement) {
    Q_ASSERT_X(DbLoader::getConfig().isMysql(), "ExplainTool<MysqlExplainInfo>", "need config mysql");
    QSqlQuery query = BaseQuery::queryPrimitiveThrowable("explain " + statement);
    QList<MysqlExplainInfo> result;
    while (query.next()) {
        MysqlExplainInfo info;
        info.id = query.value("id").toULongLong();
        info.selectType = query.value("select_type").toString();
        info.table = query.value("table").toString();
        info.partitions = query.value("partitions").toString();
        info.type = query.value("type").toString();
        info.possibleKeys = query.value("possible_keys").toString();
        info.key = query.value("key").toString();
        info.keyLen = query.value("key_len").toString();
        info.ref = query.value("ref").toString();
        info.rows = query.value("rows").toULongLong();
        info.filtered = query.value("filtered").toDouble();
        info.extra = query.value("Extra").toString();
        result << info;
    }
    return result;
}
