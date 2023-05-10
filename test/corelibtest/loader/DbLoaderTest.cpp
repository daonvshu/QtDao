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
    globalConfig->getClient()->dropDatabase();
}
