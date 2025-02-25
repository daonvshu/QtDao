#pragma once

#include <qobject.h>

#include "BaseTest.h"

#include "entity/sqliteentity/sqlitetest1.h"
#include "entity/sqliteentity/sqlitetest2.h"
#include "entity/sqliteentity/sqlitetest3.h"

#include "entity/mysqlentity/mysqltest1.h"
#include "entity/mysqlentity/mysqltest2.h"
#include "entity/mysqlentity/mysqltest3.h"

#include "entity/sqlserverentity/sqlservertest1.h"
#include "entity/sqlserverentity/sqlservertest2.h"
#include "entity/sqlserverentity/sqlservertest3.h"

#include "entity/psqlentity/psqltest1.h"
#include "entity/psqlentity/psqltest2.h"
#include "entity/psqlentity/psqltest3.h"

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

    PSqlTest1List psqlData1;
    PSqlTest2List psqlData2;
    PSqlTest3List psqlData3;
};
