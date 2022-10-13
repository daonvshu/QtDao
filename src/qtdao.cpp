#include "dbloader.h"
#include "dbexceptionhandler.h"
#include "dao.h"

QTDAO_BEGIN_NAMESPACE

DbExceptionHandler* DbExceptionHandler::exceptionHandler = nullptr;

static QueryLogPrinter queryLogPrinter = nullptr;
void daoSetQueryLogPrinter(QueryLogPrinter printer) {
    queryLogPrinter = printer;
}

QueryLogPrinter getQueryLogPrinter() {
    return queryLogPrinter;
}

void DbExceptionHandler::setExceptionHandler(DbExceptionHandler* exceptionHandler) {
    if (exceptionHandler != nullptr) {
        DbExceptionHandler::exceptionHandler = exceptionHandler;
    }
}

void DbExceptionHandler::initDbFail(DbErrCode errcode, const QString& reason) {
    Q_UNUSED(errcode)
    Q_UNUSED(reason)
}

void DbExceptionHandler::databaseOpenFail(DbErrCode errcode, const QString& failReason) {
    Q_UNUSED(errcode)
    Q_UNUSED(failReason)
}

void DbExceptionHandler::execFail(DbErrCode errcode, const QString& lastErr) {
    Q_UNUSED(errcode)
    Q_UNUSED(lastErr)
}

void DbExceptionHandler::execWarning(const QString& info) {
    Q_UNUSED(info)
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