#pragma once

#include <qobject.h>

#include "../BaseTest.h"

#include "../sqliteentity/SqliteTest1.h"
#include "../sqliteentity/SqliteTest2.h"

class DeleteTest : public BaseTest {
    Q_OBJECT

private slots:
    void initTestCase();

    void filterDeleteTest();
    void objectDeleteTest();
    void truncateTest();

    void cleanup();

    void cleanupTestCase();

private:
    SqliteTest1List data1;
    SqliteTest2List data2;
};