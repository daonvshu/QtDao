#pragma once

#include <qobject.h>

#include "sqliteentity/SqliteConfig.h"
#include "mysqlentity/MysqlConfig.h"
#include "EngineModelSelector.h"

class BaseTest : public QObject , public EngineModelSelector {
public:
    BaseTest(EngineModel model);

private:
    static QList<QPair<QString, QVariantList>> cachedSqlLog;

    friend void SqlLogPrinter(const QString& sql, const QVariantList& values);

protected:
    void clearCacheAndPrintIfTestFail();
};