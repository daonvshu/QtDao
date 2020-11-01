#pragma once

class DbExceptionHandler {
public:
    static DbExceptionHandler* exceptionHandler;

    virtual void initDbFail(const char* reason) = 0;
};