#include "DbLoader.h"
#include "DbExceptionHandler.h"

class DefaultExceptionHandler : public DbExceptionHandler {
public:
    void initDbFail(const char* reason) {}
};

DbConfig DbLoader::config;
DbInitClient* DbLoader::initClient = nullptr;

DbExceptionHandler* DbExceptionHandler::exceptionHandler = new DefaultExceptionHandler;