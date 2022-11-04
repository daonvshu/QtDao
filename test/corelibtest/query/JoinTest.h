#pragma once

#include <qobject.h>

#include "BaseTest.h"

#include "sqliteentity/sqlitetest1.h"
#include "sqliteentity/sqlitetest2.h"
#include "sqliteentity/sqlitetest3.h"

#include "mysqlentity/mysqltest1.h"
#include "mysqlentity/mysqltest2.h"
#include "mysqlentity/mysqltest3.h"

#include "sqlserverentity/sqlservertest1.h"
#include "sqlserverentity/sqlservertest2.h"
#include "sqlserverentity/sqlservertest3.h"

class JoinTest : public BaseTest {
    Q_OBJECT

public:
    using BaseTest::BaseTest;

private slots:
    void initTestCase();

    void testJoinTable();
    void testJoinTableUseWith();
    void testJoinTableFilterOn();

    void testJoinSelfTable();

    void testSelectFromJoin();

    void testJoinFromSelect();

    void testJoinOnSelect();

    void testSelectUnionJoin();

    void testJoinUnionSelect();

    void testJoinUnionJoin();

    void recursiveQueryTest();

    void functionSubJoinTest_data();
    void functionSubJoinTest();

    void explainTest();

    void cleanup();

    void cleanupTestCase();

private:
    SqliteTest1List sqliteData1;
    SqliteTest2List sqliteData2;
    SqliteTest3List sqliteData3;

    MysqlTest1List mysqlData1;
    MysqlTest2List mysqlData2;
    MysqlTest3List mysqlData3;

    SqlServerTest1List sqlserverData1;
    SqlServerTest2List sqlserverData2;
    SqlServerTest3List sqlserverData3;
};
