#pragma once

#include "global.h"

#include <qvariant.h>
#include <qstring.h>
#include <qexception.h>
#include <qsqlerror.h>

QTDAO_BEGIN_NAMESPACE

class DaoException : public QException {
public:
    explicit DaoException(const QSqlError& error, QString sql = QString(), QVariantList values = QVariantList())
        : reason(error.text())
        , errorType(error.type())
        , nativeCode(error.nativeErrorCode())
        , sql(std::move(sql))
        , values(std::move(values))
    {}

    explicit DaoException(QString reason, QString sql = QString(), QVariantList values = QVariantList())
        : reason(std::move(reason))
        , errorType(QSqlError::ConnectionError)
        , sql(std::move(sql))
        , values(std::move(values))
    {}

    void raise() const override { throw* this; }
    DaoException* clone() const override { return new DaoException(*this); }

    QString reason;
    QSqlError::ErrorType errorType;
    QString nativeCode;

    QString sql;
    QVariantList values;
};

QTDAO_END_NAMESPACE