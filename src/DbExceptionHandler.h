#pragma once

#include <qvariant.h>
#include <qstring.h>

class DbExceptionHandler {
public:
    static DbExceptionHandler* exceptionHandler;
    static void setExceptionHandler(DbExceptionHandler* exceptionHandler);

    /// <summary>
    /// 初始化数据库失败
    /// </summary>
    /// <param name="reason"></param>
    virtual void initDbFail(const char* reason) {};

    /// <summary>
    /// 连接到数据库失败
    /// </summary>
    /// <param name="failReason"></param>
    virtual void databaseOpenFail(const QString& failReason) {};

    /// <summary>
    /// 执行SQL语句失败
    /// </summary>
    /// <param name="lastErr"></param>
    virtual void execFail(const QString& lastErr) {};
};

/// <summary>
/// 设置query语句和值列表的打印
/// </summary>
typedef void (*QueryLogPrinter)(const QString&, const QVariantList&);
void daoSetQueryLogPrinter(QueryLogPrinter);

QueryLogPrinter getQueryLogPrinter();
