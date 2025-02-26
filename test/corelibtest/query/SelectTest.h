#pragma once

#include <qobject.h>

#include "BaseTest.h"

#include "entity/sqliteentity/sqlitetest1.h"
#include "entity/sqliteentity/sqlitetest2.h"

#include "entity/mysqlentity/mysqltest1.h"
#include "entity/mysqlentity/mysqltest2.h"
#include "entity/mysqlentity/mysqltest3.h"

#include "entity/sqlserverentity/sqlservertest1.h"
#include "entity/sqlserverentity/sqlservertest2.h"
#include "entity/sqlserverentity/sqlservertest3.h"

#include "entity/psqlentity/psqltest1.h"
#include "entity/psqlentity/psqltest2.h"
#include "entity/psqlentity/psqltest3.h"

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

    void functionSelectTest();

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
    PSqlTest1List psqlData1;
    PSqlTest2List psqlData2;
};