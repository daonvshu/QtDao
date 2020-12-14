﻿#pragma once

#include <qobject.h>

#include "../BaseTest.h"

#include "../sqliteentity/SqliteTest1.h"
#include "../sqliteentity/SqliteTest2.h"

class SelectTest : public BaseTest {
    Q_OBJECT

private slots:
    void initTestCase();

    void uniqueSelectTest();
    void listSelectTest();
    void rawSelectTest();
    void funtionSelectTest();
    void countSelectTest();
    void selectFromSelectTest();
    void unionSelectTest();
    void funtionSubSelectTest();

    void cleanup();

    void cleanupTestCase();

private:
    SqliteTest1List data1;
    SqliteTest2List data2;
};