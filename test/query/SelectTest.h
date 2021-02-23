#pragma once

#include <qobject.h>

#include "../BaseTest.h"

#include "../sqliteentity/SqliteTest1.h"
#include "../sqliteentity/SqliteTest2.h"

#include "../mysqlentity/MysqlTest1.h"
#include "../mysqlentity/MysqlTest2.h"
#include "../mysqlentity/MysqlTest3.h"

#include "../sqlserverentity/SqlServerTest1.h"
#include "../sqlserverentity/SqlServerTest2.h"
#include "../sqlserverentity/SqlServerTest3.h"

class SelectTest : public BaseTest {
    Q_OBJECT

public:
    using BaseTest::BaseTest;

private slots:
    void initTestCase();

    void uniqueSelectTest_data();
    void uniqueSelectTest();

    void listSelectTest_data();
    void listSelectTest();

    void rawSelectTest_data();
    void rawSelectTest();

    void funtionSelectTest();

    void countSelectTest();

    void selectFromSelectTest_data();
    void selectFromSelectTest();

    void unionSelectTest_data();
    void unionSelectTest();

    void funtionSubSelectTest_data();
    void funtionSubSelectTest();

    void explainTest();

    void cleanup();

    void cleanupTestCase();

private:
    SqliteTest1List sqliteData1;
    SqliteTest2List sqliteData2;
    MysqlTest1List mysqlData1;
    MysqlTest2List mysqlData2;
    SqlServerTest1List sqlserverData1;
    SqlServerTest2List sqlserverData2;
};