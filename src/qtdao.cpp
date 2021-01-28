#include "DbLoader.h"
#include "DbExceptionHandler.h"
#include "dao.h"

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
