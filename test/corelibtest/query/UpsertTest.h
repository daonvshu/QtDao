#pragma once

#include "BaseTest.h"

class UpsertTest : public BaseTest {
    Q_OBJECT

public:
    using BaseTest::BaseTest;

private slots:
    void initTestCase();

    void setUpsertTest();

    void setUpsertBatchTest();

    void upsertObjectTest_data();
    void upsertObjectTest();

    void upsertObjectsTest_data();
    void upsertObjectsTest();

    void cleanup();

    void cleanupTestCase();
};
