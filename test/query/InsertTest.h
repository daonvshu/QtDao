#pragma once

#include <qobject.h>

#include "../BaseTest.h"

class InsertTest : public BaseTest {
    Q_OBJECT

private slots:
    void initTestCase();

    void setInsertTest();
    void setInsertBatchTest();
    void insertObjectTest();
    void insertObjectsTest();
    void insertObjects2Test();
    void insertOrReplaceTest();

    void cleanup();

    void cleanupTestCase();
};