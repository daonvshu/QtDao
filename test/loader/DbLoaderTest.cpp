#include "DbLoaderTest.h"

#include "../../src/DbLoader.h"
#include "../../src/ConnectionPool.h"

#include "../../entity/Test1.h"
#include "../../entity/Test2.h"

#include <qtest.h>

class DbLoaderTestExceptionHandler : public DbExceptionHandler {
public:
    void initDbFail(const QString& reason) {
        QFAIL(("init db fail:" + reason).toUtf8());
    }

    void databaseOpenFail(const QString& failReason) {
        QFAIL(("database open fail:" + failReason).toUtf8());
    }

    void execFail(const QString& lastErr) {
        QFAIL(("query fail:" + lastErr).toUtf8());
    }
};

void DbLoaderTest::initTestCase() {

}

void DbLoaderTest::sqliteloadConfigTest() {
    DbLoader::init(":/QtDao/entity/sqlite_cfg.xml", new DbLoaderTestExceptionHandler);
    Test1::Info t1info;
    QVERIFY(DbLoader::getClient().checkTableExist(t1info.getTableName()));
    Test2::Info t2info;
    QVERIFY(DbLoader::getClient().checkTableExist(t2info.getTableName()));
}

void DbLoaderTest::cleanup() {
    ConnectionPool::release();
    DbLoader::getClient().dropDatabase();
}

void DbLoaderTest::cleanupTestCase() {

}
