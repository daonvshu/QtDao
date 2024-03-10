#pragma once

#include <qobject.h>

#include "BaseTest.h"

class InsertTest : public BaseTest {
    Q_OBJECT

public:
    using BaseTest::BaseTest;

private slots:
    void initTestCase();

    void setInsertTest();
    void setInsertBatchTest();

    void insertObjectTest_data();
    void insertObjectTest();

    void insertObjectsTest_data();
    void insertObjectsTest();

    void insertObjects2Test_data();
    void insertObjects2Test();

    void insertOrReplaceTest_data();
    void insertOrReplaceTest();

    void insertOrIgnoreTest_data();
    void insertOrIgnoreTest();

    void testTransaction_data();
    void testTransaction();

    void testMysqlMyISAMTransaction_data();
    void testMysqlMyISAMTransaction();

    void cleanup();

    void cleanupTestCase();
};