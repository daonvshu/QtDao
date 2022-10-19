#pragma once

#include "global.h"

#include <qvariant.h>
#include <qstring.h>
#include <qexception.h>
#include <qsqlerror.h>

QTDAO_BEGIN_NAMESPACE

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