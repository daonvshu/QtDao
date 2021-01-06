#pragma once

#include <qobject.h>

#include "../BaseTest.h"

#include "../sqliteentity/SqliteTest1.h"
#include "../sqliteentity/SqliteTest2.h"
#include "../sqliteentity/SqliteTest3.h"

#include "../mysqlentity/MysqlTest1.h"
#include "../mysqlentity/MysqlTest2.h"
#include "../mysqlentity/MysqlTest3.h"

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
};
