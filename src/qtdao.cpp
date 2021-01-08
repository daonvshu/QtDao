#include "DbLoader.h"
#include "DbExceptionHandler.h"
#include "dao.h"

#include <qdebug.h>

class DefaultExceptionHandler : public DbExceptionHandler {
public:
    void databaseOpenFail(const QString& failReason) {
        qWarning() << failReason;
    };

    void execFail(const QString& lastErr) {
        qWarning() << lastErr;
    }

    void execWarning(const QString& info) {
        qWarning() << info;
    }
};

DbConfig DbLoader::config;
AbstractClient* DbLoader::sqlClient = nullptr;

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