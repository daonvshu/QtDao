#pragma once

#include <qobject.h>

#include "EngineModelSelector.h"

QTDAO_USING_NAMESPACE

class BaseTest : public QObject , public EngineModelSelector {
public:
    BaseTest(EngineModel model);

private:
    static QList<QPair<QString, QVariantList>> cachedSqlLog;

    friend void SqlLogPrinter(const QString& sql, const QVariantList& values);

protected:
    void clearCacheAndPrintIfTestFail();
    void clearCache();

    void configDb() {
        setupDatabase();
        clearCacheAndPrintIfTestFail();
    }
};