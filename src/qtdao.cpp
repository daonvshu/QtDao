#include "dbexceptionhandler.h"
#include "dao.h"

QTDAO_BEGIN_NAMESPACE

QScopedPointer<ConfigBuilder> globalConfig;

static QueryLogPrinter queryLogPrinter = nullptr;
void daoSetQueryLogPrinter(QueryLogPrinter printer) {
    queryLogPrinter = printer;
}

QueryLogPrinter getQueryLogPrinter() {
    return queryLogPrinter;
}

void transcation() {
    BaseQuery::sqliteLockControl.trancationStart();
    if (globalConfig->isSqlServer()) {
        BaseQuery::queryPrimitiveThrowable("begin tran");
    } else {
        BaseQuery::queryPrimitiveThrowable("begin");
    }
}

void commit() {
    BaseQuery::sqliteLockControl.trancationPrepareEnd();
    BaseQuery::queryPrimitiveThrowable("commit");
    BaseQuery::sqliteLockControl.trancationEnd();
}

void transcation_save(const QString& savePoint) {
    if (globalConfig->isSqlServer()) {
        BaseQuery::queryPrimitiveThrowable(QString("save tran %1").arg(savePoint));
    } else {
        BaseQuery::queryPrimitiveThrowable(QString("savepoint %1").arg(savePoint));
    }
}

void rollback(const QString& savePoint) {
    if (savePoint.isEmpty()) {
        BaseQuery::sqliteLockControl.trancationPrepareEnd();
    }
    if (globalConfig->isSqlServer()) {
        BaseQuery::queryPrimitiveThrowable(
            savePoint.isEmpty() ? QString("rollback tran") : QString("rollback tran %1").arg(savePoint)
        );
    } else {
        BaseQuery::queryPrimitiveThrowable(
            savePoint.isEmpty() ? QString("rollback") : QString("rollback to %1").arg(savePoint)
        );
    }
    if (savePoint.isEmpty()) {
        BaseQuery::sqliteLockControl.trancationEnd();
    }
}

void sqlWriteSync(bool enable) {
    BaseQuery::sqliteLockControl.enableSqliteWriteSync(enable);
}

void transcation() {
    BaseQuery::sqliteLockControl.trancationStart();
    if (DbLoader::getConfig().isSqlServer()) {
        BaseQuery::queryPrimitiveThrowable("begin tran");
    } else {
        BaseQuery::queryPrimitiveThrowable("begin");
    }
}

void commit() {
    BaseQuery::sqliteLockControl.trancationPrepareEnd();
    BaseQuery::queryPrimitiveThrowable("commit");
    BaseQuery::sqliteLockControl.trancationEnd();
}

void transcation_save(const QString& savePoint) {
    if (DbLoader::getConfig().isSqlServer()) {
        BaseQuery::queryPrimitiveThrowable(QString("save tran %1").arg(savePoint));
    } else {
        BaseQuery::queryPrimitiveThrowable(QString("savepoint %1").arg(savePoint));
    }
}

void rollback(const QString& savePoint) {
    if (savePoint.isEmpty()) {
        BaseQuery::sqliteLockControl.trancationPrepareEnd();
    }
    if (DbLoader::getConfig().isSqlServer()) {
        BaseQuery::queryPrimitiveThrowable(
                savePoint.isEmpty() ? QString("rollback tran") : QString("rollback tran %1").arg(savePoint)
        );
    } else {
        BaseQuery::queryPrimitiveThrowable(
                savePoint.isEmpty() ? QString("rollback") : QString("rollback to %1").arg(savePoint)
        );
    }
    if (savePoint.isEmpty()) {
        BaseQuery::sqliteLockControl.trancationEnd();
    }
}

void sqlWriteSync(bool enable) {
    BaseQuery::sqliteLockControl.enableSqliteWriteSync(enable);
}

QTDAO_END_NAMESPACE