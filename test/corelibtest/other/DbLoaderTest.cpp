#include "DbLoaderTest.h"

#include "connectionpool.h"

#include "entity/sqliteentity/sqlitetest1.h"
#include "entity/sqliteentity/sqlitetest2.h"

#include "entity/mysqlentity/mysqltest1.h"
#include "entity/mysqlentity/mysqltest2.h"

#include "entity/sqlserverentity/sqlservertest1.h"
#include "entity/sqlserverentity/sqlservertest2.h"

#include "entity/psqlentity/psqltest1.h"
#include "entity/psqlentity/psqltest2.h"

#include <QtTest>

QTDAO_USING_NAMESPACE

void DbLoaderTest::initTestCase() {

}

void DbLoaderTest::loadConfigTest() {
    setupDatabase("corelib_test");
    auto config = ConfigManager::getConfig();
    if (targetDb == TestTargetDb::Target_Sqlite) {
        QVERIFY(config->getClient()->checkTableExist(SqliteTest1::Info::getTableName()));
        QVERIFY(config->getClient()->checkTableExist(SqliteTest2::Info::getTableName()));
    } else if (targetDb == TestTargetDb::Target_Mysql) {
        QVERIFY(config->getClient()->checkTableExist(MysqlTest1::Info::getTableName()));
        QVERIFY(config->getClient()->checkTableExist(MysqlTest2::Info::getTableName()));
    } else if (targetDb == TestTargetDb::Target_SqlServer) {
        QVERIFY(config->getClient()->checkTableExist(SqlServerTest1::Info::getTableName()));
        QVERIFY(config->getClient()->checkTableExist(SqlServerTest2::Info::getTableName()));
    } else if (targetDb == TestTargetDb::Target_PSql) {
        QVERIFY(config->getClient()->checkTableExist(PSqlTest1::Info::getTableName()));
        QVERIFY(config->getClient()->checkTableExist(PSqlTest2::Info::getTableName()));
    }
    QCOMPARE(config->getLocalVersion(), 1);
}

void DbLoaderTest::upgradeTest() {
    //reinit
    setupDatabase("corelib_test", 3);
    auto config = ConfigManager::getConfig();
    if (targetDb == TestTargetDb::Target_Sqlite) {
        QVERIFY(config->getClient()->checkTableExist(SqliteTest1::Info::getTableName()));
        QVERIFY(config->getClient()->checkTableExist(SqliteTest2::Info::getTableName()));
    } else if (targetDb == TestTargetDb::Target_Mysql) {
        QVERIFY(config->getClient()->checkTableExist(MysqlTest1::Info::getTableName()));
        QVERIFY(config->getClient()->checkTableExist(MysqlTest2::Info::getTableName()));
    } else if (targetDb == TestTargetDb::Target_SqlServer) {
        QVERIFY(config->getClient()->checkTableExist(SqlServerTest1::Info::getTableName()));
        QVERIFY(config->getClient()->checkTableExist(SqlServerTest2::Info::getTableName()));
    } else if (targetDb == TestTargetDb::Target_PSql) {
        QVERIFY(config->getClient()->checkTableExist(PSqlTest1::Info::getTableName()));
        QVERIFY(config->getClient()->checkTableExist(PSqlTest2::Info::getTableName()));
    }
    //test version
    QCOMPARE(config->getLocalVersion(), 3);
}

void DbLoaderTest::cleanup() {
    
}

void DbLoaderTest::cleanupTestCase() {
    ConfigManager::getConfig()->getClient()->dropDatabase();
}
