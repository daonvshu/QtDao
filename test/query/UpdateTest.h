#pragma once

#include <qobject.h>

#include "../sqliteentity/SqliteTest1.h"
#include "../sqliteentity/SqliteTest2.h"

class UpdateTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();

    void setUpdateTest();
    void objectUpdateTest();

    void cleanup();

    void cleanupTestCase();

private:
    SqliteTest1List data1;
    SqliteTest2List data2;
};