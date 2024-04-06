#include "DbLoaderTest.h"

#include "connectionpool.h"

#include "sqliteentity/sqlitetest1.h"
#include "sqliteentity/sqlitetest2.h"

#include "mysqlentity/mysqltest1.h"
#include "mysqlentity/mysqltest2.h"

#include "sqlserverentity/sqlservertest1.h"
#include "sqlserverentity/sqlservertest2.h"

#include <QtTest>

QTDAO_USING_NAMESPACE

void DbLoaderTest::initTestCase() {

}

void DbLoaderTest::loadConfigTest() {
    setupDatabase();
    auto config = ConfigManager::getConfig();
    if (engineModel == Engine_Sqlite) {
        QVERIFY(config->getClient()->checkTableExist(SqliteTest1::Info::getTableName()));
        QVERIFY(config->getClient()->checkTableExist(SqliteTest2::Info::getTableName()));
    } else if (engineModel == Engine_Mysql) {
        QVERIFY(config->getClient()->checkTableExist(MysqlTest1::Info::getTableName()));
        QVERIFY(config->getClient()->checkTableExist(MysqlTest2::Info::getTableName()));
    } else if (engineModel == Engine_SqlServer) {
        QVERIFY(config->getClient()->checkTableExist(SqlServerTest1::Info::getTableName()));
        QVERIFY(config->getClient()->checkTableExist(SqlServerTest2::Info::getTableName()));
    }
    QCOMPARE(config->getLocalVersion(), 1);
}

void DbLoaderTest::upgradeTest() {
    //reinit
    setupDatabase(3);
    auto config = ConfigManager::getConfig();
    if (engineModel == Engine_Sqlite) {
        QVERIFY(config->getClient()->checkTableExist(SqliteTest1::Info::getTableName()));
        QVERIFY(config->getClient()->checkTableExist(SqliteTest2::Info::getTableName()));
    } else if (engineModel == Engine_Mysql) {
        QVERIFY(config->getClient()->checkTableExist(MysqlTest1::Info::getTableName()));
        QVERIFY(config->getClient()->checkTableExist(MysqlTest2::Info::getTableName()));
    } else if (engineModel == Engine_SqlServer) {
        QVERIFY(config->getClient()->checkTableExist(SqlServerTest1::Info::getTableName()));
        QVERIFY(config->getClient()->checkTableExist(SqlServerTest2::Info::getTableName()));
    }
    //test version
    QCOMPARE(config->getLocalVersion(), 3);
}

void DbLoaderTest::cleanup() {
    
}

void DbLoaderTest::cleanupTestCase() {
    ConfigManager::getConfig()->getClient()->dropDatabase();
}
