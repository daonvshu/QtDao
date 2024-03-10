#include "dbexception.h"
#include "dao.h"

QTDAO_BEGIN_NAMESPACE

void transaction(LoggingCategoryPtr logging) {
    if (globalConfig->isSqlServer()) {
        BaseQuery::queryPrimitive("begin tran", {}, logging);
    } else {
        BaseQuery::queryPrimitive("begin", {}, logging);
    }
}

void commit(LoggingCategoryPtr logging) {
    BaseQuery::queryPrimitive("commit", {}, logging, false);
}

void transaction_save(const QString& savePoint, LoggingCategoryPtr logging) {
    if (globalConfig->isSqlServer()) {
        BaseQuery::queryPrimitive(QString("save tran %1").arg(savePoint), {}, logging);
    } else {
        BaseQuery::queryPrimitive(QString("savepoint %1").arg(savePoint), {}, logging);
    }
}

void rollback(const QString& savePoint, LoggingCategoryPtr logging) {
    if (globalConfig->isSqlServer()) {
        BaseQuery::queryPrimitive(
            savePoint.isEmpty() ? QString("rollback tran") : QString("rollback tran %1").arg(savePoint),
            {}, logging
        );
    } else {
        BaseQuery::queryPrimitive(
            savePoint.isEmpty() ? QString("rollback") : QString("rollback to %1").arg(savePoint),
            {}, logging
        );
    }
}

void loggingUseDefault(bool useDefault) {
    BaseQuery::useDefaultLoggingIfNull(useDefault);
}

QTDAO_END_NAMESPACE