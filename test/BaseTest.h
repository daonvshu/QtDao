#pragma once

#include <qobject.h>

#include "sqliteentity/SqliteConfig.h"
#include "mysqlentity/MysqlConfig.h"
#include "EngineModelSelector.h"

#include "../src/dao.h"

class BaseTest : public QObject , public EngineModelSelector {
public:
    BaseTest(EngineModel model);

private:
    static QList<QPair<QString, QVariantList>> cachedSqlLog;

    friend void SqlLogPrinter(const QString& sql, const QVariantList& values);

protected:
    void clearCacheAndPrintIfTestFail();

    void configDb() {
        if (engineModel == Engine_Sqlite) {
            DbLoader::init(SqliteConfig());
        } else if (engineModel == Engine_Mysql) {
            DbLoader::init(MysqlConfig());
        }
        clearCacheAndPrintIfTestFail();
    }
};