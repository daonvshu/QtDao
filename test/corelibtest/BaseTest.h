#pragma once

#include <qobject.h>

#include "databaseselector.h"

QTDAO_USING_NAMESPACE

class BaseTest : public QObject , public DatabaseSelector {
public:
    explicit BaseTest(TestTargetDb targetDb);

private:
    static QList<QPair<QString, QVariantList>> cachedSqlLog;

    friend void SqlLogPrinter(const QString& sql, const QVariantList& values);

protected:
    void clearCacheAndPrintIfTestFail();
    void clearCache();

    void configDb() {
        setupDatabase("corelib_test");
        clearCacheAndPrintIfTestFail();
    }
};