#pragma once

#include <qobject.h>

#include "../BaseTest.h"

#include "../sqliteentity/SqliteTest1.h"
#include "../sqliteentity/SqliteTest2.h"

class InsertIntoSelectTest : public BaseTest {
    Q_OBJECT

private slots:
    void initTestCase();

    void testInsertIntoSelect();
    void testInsertIntoJoin();

    void cleanup();

    void cleanupTestCase();

private:
    SqliteTest1List data1;
};