#pragma once

#include <qobject.h>

#include "../sqliteentity/SqliteTest1.h"
#include "../sqliteentity/SqliteTest2.h"
#include "../sqliteentity/SqliteTest3.h"

class JoinTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();

    void testJoinTable();

    void cleanup();

    void cleanupTestCase();

private:
    SqliteTest1List data1;
    SqliteTest2List data2;
    SqliteTest3List data3;
};