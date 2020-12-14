﻿#pragma once

#include <qobject.h>

#include "sqliteentity/SqliteConfig.h"

class BaseTest : public QObject {
public:
    BaseTest();

private:
    static QList<QPair<QString, QVariantList>> cachedSqlLog;

    friend void SqlLogPrinter(const QString& sql, const QVariantList& values);

protected:
    void clearCacheAndPrintIfTestFail();
};