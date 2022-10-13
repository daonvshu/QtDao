#pragma once

#include "global.h"

#include <qvariant.h>
#include <qstring.h>
#include <qexception.h>

#include "dberrcode.h"

QTDAO_BEGIN_NAMESPACE

/// <summary>
/// set query statement and value list printer
/// </summary>
typedef void (*QueryLogPrinter)(const QString&, const QVariantList&);
void daoSetQueryLogPrinter(QueryLogPrinter);

QueryLogPrinter getQueryLogPrinter();


class DaoException : public QException {
public:
    DaoException(DbErrCode::Code code, const QString& reason) : reason(reason), code(code) {}

    void raise() const override { throw* this; }
    DaoException* clone() const override { return new DaoException(*this); }

    QString reason;
    DbErrCode code;
};

QTDAO_END_NAMESPACE