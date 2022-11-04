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

    void testTranscation_data();
    void testTranscation();

    void testMysqlMyISAMTranscation_data();
    void testMysqlMyISAMTranscation();

    void cleanup();

    void cleanupTestCase();
};