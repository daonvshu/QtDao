#pragma once

#include <qobject.h>

#include "../BaseTest.h"

#include "../sqliteentity/SqliteTest1.h"
#include "../sqliteentity/SqliteTest2.h"
#include "../sqliteentity/SqliteTest3.h"

class JoinTest : public BaseTest {
    Q_OBJECT

public:
    using BaseTest::BaseTest;

private slots:
    void initTestCase();

    void testJoinTable();
    void testJoinSelfTable();
    void testSelectFromJoin();
    void testJoinFromSelect();
    void testJoinOnSelect();
    void testSelectUnionJoin();
    void testJoinUnionSelect();
    void testJoinUnionJoin();
    void recursiveQueryTest();
    void functionSubJoinTest();

    void cleanup();

    void cleanupTestCase();

private:
    SqliteTest1List data1;
    SqliteTest2List data2;
    SqliteTest3List data3;
};