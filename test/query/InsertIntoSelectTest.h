#pragma once

#include <qobject.h>

#include "../BaseTest.h"

#include "../sqliteentity/sqlitetest1.h"
#include "../sqliteentity/sqlitetest2.h"

#include "../mysqlentity/mysqltest1.h"
#include "../mysqlentity/mysqltest2.h"

#include "../sqlserverentity/sqlservertest1.h"
#include "../sqlserverentity/sqlservertest2.h"

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