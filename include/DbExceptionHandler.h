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
    /// 
    /// </summary>
    /// <param name="reason"></param>
    virtual void initDbFail(const QString& reason) { Q_UNUSED(reason) };

    /// <summary>
    /// 
    /// </summary>
    /// <param name="failReason"></param>
    virtual void databaseOpenFail(const QString& failReason) { Q_UNUSED(failReason) };

    /// <summary>
    /// 
    /// </summary>
    /// <param name="lastErr"></param>
    virtual void execFail(const QString& lastErr) { Q_UNUSED(lastErr) };

    /// <summary>
    /// 
    /// </summary>
    /// <param name="info"></param>
    virtual void execWarning(const QString& info) { Q_UNUSED(info) };
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
