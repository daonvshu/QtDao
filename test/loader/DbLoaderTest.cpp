#include "DbLoaderTest.h"

#include "connectionpool.h"

#include "../sqliteentity/SqliteTest1.h"
#include "../sqliteentity/SqliteTest2.h"
#include "../sqliteentity/SqliteConfig.h"

#include "../mysqlentity/MysqlTest1.h"
#include "../mysqlentity/MysqlTest2.h"
#include "../mysqlentity/MysqlConfig.h"

#include "../sqlserverentity/SqlServerTest1.h"
#include "../sqlserverentity/SqlServerTest2.h"
#include "../sqlserverentity/SqlServerConfig.h"

#include <QtTest>

QTDAO_USING_NAMESPACE

void DbLoaderTest::initTestCase() {

}

void DbLoaderTest::loadConfigTest() {
    setupDatabase();
    if (engineModel == Engine_Sqlite) {
        QVERIFY(globalConfig->getClient()->checkTableExist(SqliteTest1::Info::getTableName()));
        QVERIFY(globalConfig->getClient()->checkTableExist(SqliteTest2::Info::getTableName()));
    } else if (engineModel == Engine_Mysql) {
        QVERIFY(globalConfig->getClient()->checkTableExist(MysqlTest1::Info::getTableName()));
        QVERIFY(globalConfig->getClient()->checkTableExist(MysqlTest2::Info::getTableName()));
    } else if (engineModel == Engine_SqlServer) {
        QVERIFY(globalConfig->getClient()->checkTableExist(SqlServerTest1::Info::getTableName()));
        QVERIFY(globalConfig->getClient()->checkTableExist(SqlServerTest2::Info::getTableName()));
    }
    QCOMPARE(globalConfig->getLocalVersion(), 1);
}

void DbLoaderTest::upgradeTest() {
    //reinit
    setupDatabase(3);
    if (engineModel == Engine_Sqlite) {
        QVERIFY(globalConfig->getClient()->checkTableExist(SqliteTest1::Info::getTableName()));
        QVERIFY(globalConfig->getClient()->checkTableExist(SqliteTest2::Info::getTableName()));
    } else if (engineModel == Engine_Mysql) {
        QVERIFY(globalConfig->getClient()->checkTableExist(MysqlTest1::Info::getTableName()));
        QVERIFY(globalConfig->getClient()->checkTableExist(MysqlTest2::Info::getTableName()));
    } else if (engineModel == Engine_SqlServer) {
        QVERIFY(globalConfig->getClient()->checkTableExist(SqlServerTest1::Info::getTableName()));
        QVERIFY(globalConfig->getClient()->checkTableExist(SqlServerTest2::Info::getTableName()));
    }
    //test version
    QCOMPARE(globalConfig->getLocalVersion(), 3);
}

void DbLoaderTest::cleanup() {
    
}

void DbLoaderTest::cleanupTestCase() {
    ConnectionPool::release();
    globalConfig->getClient()->dropDatabase();
}
