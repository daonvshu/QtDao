#include "DbLoaderTest.h"

#include "DbLoader.h"
#include "ConnectionPool.h"

#include "../sqliteentity/SqliteTest1.h"
#include "../sqliteentity/SqliteTest2.h"
#include "../sqliteentity/SqliteConfig.h"

#include "../mysqlentity/MysqlTest1.h"
#include "../mysqlentity/MysqlTest2.h"
#include "../mysqlentity/MysqlConfig.h"

#include <QtTest>

class DbLoaderTestExceptionHandler : public DbExceptionHandler {
public:
    using DbExceptionHandler::DbExceptionHandler;

    void initDbFail(DbErrCode errcode, const QString& reason) {
        Q_UNUSED(errcode)
        QFAIL(("init db fail:" + reason).toUtf8());
    }

    void databaseOpenFail(DbErrCode errcode, const QString& failReason) {
        Q_UNUSED(errcode)
        QFAIL(("database open fail:" + failReason).toUtf8());
    }

    void execFail(const QString& lastErr) {
        QFAIL(("query fail:" + lastErr).toUtf8());
    }
};

void DbLoaderTest::initTestCase() {

}

void DbLoaderTest::loadConfigTest() {
    if (engineModel == Engine_Sqlite) {
        DbLoader::init(SqliteConfig(), new DbLoaderTestExceptionHandler(this));
        SqliteTest1::Info t1info;
        QVERIFY(DbLoader::getClient().checkTableExist(t1info.getTableName()));
        SqliteTest2::Info t2info;
        QVERIFY(DbLoader::getClient().checkTableExist(t2info.getTableName()));
    } else if (engineModel == Engine_Mysql) {
        DbLoader::init(MysqlConfig(), new DbLoaderTestExceptionHandler(this));
        MysqlTest1::Info t1info;
        QVERIFY(DbLoader::getClient().checkTableExist(t1info.getTableName()));
        MysqlTest2::Info t2info;
        QVERIFY(DbLoader::getClient().checkTableExist(t2info.getTableName()));
    }
    QCOMPARE(DbLoader::getLocalVersion(), 1);
}

void DbLoaderTest::upgradeTest() {
    //reinit
    if (engineModel == Engine_Sqlite) {
        SqliteConfig cfg;
        cfg.ver = 3;
        DbLoader::init(cfg);
        SqliteTest1::Info t1info;
        QVERIFY(DbLoader::getClient().checkTableExist(t1info.getTableName()));
        SqliteTest2::Info t2info;
        QVERIFY(DbLoader::getClient().checkTableExist(t2info.getTableName()));
    } else if (engineModel == Engine_Mysql) {
        MysqlConfig cfg;
        cfg.ver = 3;
        DbLoader::init(cfg);
        MysqlTest1::Info t1info;
        QVERIFY(DbLoader::getClient().checkTableExist(t1info.getTableName()));
        MysqlTest2::Info t2info;
        QVERIFY(DbLoader::getClient().checkTableExist(t2info.getTableName()));
    }
    //test version
    QCOMPARE(DbLoader::getLocalVersion(), 3);
}

void DbLoaderTest::cleanup() {
    
}

void DbLoaderTest::cleanupTestCase() {
    ConnectionPool::release();
    DbLoader::getClient().dropDatabase();
}
