#pragma once

#include <qvariant.h>
#include <qstring.h>
#include <qexception.h>

class DbExceptionHandler : public QObject {
public:
    using QObject::QObject;

    static DbExceptionHandler* exceptionHandler;
    static void setExceptionHandler(DbExceptionHandler* exceptionHandler);

    /// <summary>
    /// 初始化数据库失败
    /// </summary>
    /// <param name="reason"></param>
    virtual void initDbFail(const QString& reason) {};

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

    /// <summary>
    /// 执行SQL语句警告
    /// </summary>
    /// <param name="info"></param>
    virtual void execWarning(const QString& info) {};

    /// <summary>
    /// 版本升级失败
    /// </summary>
    /// <param name="reason"></param>
    virtual void upgradeFail(const QString& reason) {};
};

/// <summary>
/// 设置query语句和值列表的打印
/// </summary>
typedef void (*QueryLogPrinter)(const QString&, const QVariantList&);
void daoSetQueryLogPrinter(QueryLogPrinter);

QueryLogPrinter getQueryLogPrinter();


class DaoException : public QException {
public:
    DaoException(const QString& reason) : reason(reason) {}

    void raise() const override { throw* this; }
    DaoException* clone() const override { return new DaoException(*this); }

    QString reason;
};