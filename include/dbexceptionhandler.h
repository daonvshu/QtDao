#pragma once

#include "global.h"

#include <qvariant.h>
#include <qstring.h>
#include <qexception.h>
#include <qsqlerror.h>

#include <utility>

QTDAO_BEGIN_NAMESPACE

/// <summary>
/// set query statement and value list printer
/// </summary>
typedef void (*QueryLogPrinter)(const QString&, const QVariantList&);
void daoSetQueryLogPrinter(QueryLogPrinter);

QueryLogPrinter getQueryLogPrinter();


class DaoException : public QException {
public:
    explicit DaoException(const QSqlError& error)
        : reason(error.text())
        , errorType(error.type())
        , nativeCode(error.nativeErrorCode())
    {}

    explicit DaoException(QString  reason)
        : reason(std::move(reason))
        , errorType(QSqlError::ConnectionError)
    {}

    void raise() const override { throw* this; }
    DaoException* clone() const override { return new DaoException(*this); }

    QString reason;
    QSqlError::ErrorType errorType;
    QString nativeCode;
};

QTDAO_END_NAMESPACE