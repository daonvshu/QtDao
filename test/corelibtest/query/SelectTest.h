﻿#pragma once

#include <qobject.h>

#include "BaseTest.h"

#include "sqliteentity/sqlitetest1.h"
#include "sqliteentity/sqlitetest2.h"

#include "mysqlentity/mysqltest1.h"
#include "mysqlentity/mysqltest2.h"
#include "mysqlentity/mysqltest3.h"

#include "sqlserverentity/sqlservertest1.h"
#include "sqlserverentity/sqlservertest2.h"
#include "sqlserverentity/sqlservertest3.h"

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