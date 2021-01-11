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