#pragma once

#include <qobject.h>

#include "BaseTest.h"

#include "entity/sqliteentity/sqlitetest1.h"
#include "entity/sqliteentity/sqlitetest2.h"

#include "entity/mysqlentity/mysqltest1.h"
#include "entity/mysqlentity/mysqltest2.h"

#include "entity/sqlserverentity/sqlservertest1.h"
#include "entity/sqlserverentity/sqlservertest2.h"

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