#include "dbexception.h"
#include "dao.h"

QTDAO_BEGIN_NAMESPACE

RecursiveQueryBuilder _recursive(bool unionAll, qint64 sessionId) {
    auto config = ConfigManager::getConfig(sessionId);
    Q_ASSERT_X(!config->isMysql(), "RecursiveQuery", "mysql recursive query unsupported!");
    return RecursiveQueryBuilder(unionAll);
}

void _truncate(const QString& tbName, qint64 sessionId) {
    auto config = ConfigManager::getConfig(sessionId);
    config->getClient()->truncateTable(tbName);
}

void transaction(qint64 sessionId, LoggingCategoryPtr logging) {
    auto config = ConfigManager::getConfig(sessionId);
    if (config->isSqlServer()) {
        BaseQuery::queryPrimitive("begin tran", {}, sessionId, logging);
    } else {
        BaseQuery::queryPrimitive("begin", {}, sessionId, logging);
    }
}

void commit(qint64 sessionId, LoggingCategoryPtr logging) {
    BaseQuery::queryPrimitive("commit", {}, sessionId, logging, false);
}

void transaction_save(const QString& savePoint, qint64 sessionId, LoggingCategoryPtr logging) {
    auto config = ConfigManager::getConfig(sessionId);
    if (config->isSqlServer()) {
        BaseQuery::queryPrimitive(QString("save tran %1").arg(savePoint), {}, sessionId, logging);
    } else {
        BaseQuery::queryPrimitive(QString("savepoint %1").arg(savePoint), {}, sessionId, logging);
    }
}

void rollback(const QString& savePoint, qint64 sessionId, LoggingCategoryPtr logging) {
    auto config = ConfigManager::getConfig(sessionId);
    if (config->isSqlServer()) {
        BaseQuery::queryPrimitive(
            savePoint.isEmpty() ? QString("rollback tran") : QString("rollback tran %1").arg(savePoint),
            {}, sessionId, logging
        );
    } else {
        BaseQuery::queryPrimitive(
            savePoint.isEmpty() ? QString("rollback") : QString("rollback to %1").arg(savePoint),
            {}, sessionId, logging
        );
    }
}

void loggingUseDefault(bool useDefault) {
    BaseQuery::useDefaultLoggingIfNull(useDefault);
}

QTDAO_END_NAMESPACE