#pragma once

#include <qobject.h>

#include "../BaseTest.h"

#include "../sqliteentity/SqliteTest1.h"
#include "../sqliteentity/SqliteTest2.h"

#include "../mysqlentity/MysqlTest1.h"
#include "../mysqlentity/MysqlTest2.h"

#include "../sqlserverentity/SqlServerTest1.h"
#include "../sqlserverentity/SqlServerTest2.h"

class InsertIntoSelectTest : public BaseTest {
    Q_OBJECT

public:
    using BaseTest::BaseTest;

private slots:
    void initTestCase();

    void testInsertIntoSelect_data();
    void testInsertIntoSelect();

    void testInsertIntoJoin_data();
    void testInsertIntoJoin();

    void cleanup();

    void cleanupTestCase();

private:
    SqliteTest1List sqliteData1;
    MysqlTest1List mysqlData1;
    SqlServerTest1List sqlserverData1;
};