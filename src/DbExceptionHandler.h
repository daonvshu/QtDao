#pragma once

class DbExceptionHandler {
public:
    static DbExceptionHandler* exceptionHandler;
    static void setExceptionHandler(DbExceptionHandler* exceptionHandler);

    virtual void initDbFail(const char* reason) = 0;
    virtual void databaseOpenFail(const QString& failReason) = 0;
};