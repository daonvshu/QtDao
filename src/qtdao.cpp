#include "DbLoader.h"
#include "DbExceptionHandler.h"

#include <qdebug.h>

class DefaultExceptionHandler : public DbExceptionHandler {
public:
    void initDbFail(const char* reason) {}
    void databaseOpenFail(const QString& failReason) {
        qWarning() << failReason;
    };
};

DbConfig DbLoader::config;
DbInitClient* DbLoader::initClient = nullptr;

DbExceptionHandler* DbExceptionHandler::exceptionHandler = new DefaultExceptionHandler;

void DbExceptionHandler::setExceptionHandler(DbExceptionHandler* exceptionHandler) {
    if (exceptionHandler != nullptr) {
        delete DbExceptionHandler::exceptionHandler; //delete default hander
        DbExceptionHandler::exceptionHandler = exceptionHandler;
    }
}