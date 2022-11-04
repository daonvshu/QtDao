#pragma once

#include <qobject.h>

#include "BaseTest.h"

class UpdateTest : public BaseTest {
    Q_OBJECT

public:
    using BaseTest::BaseTest;

private slots:
    void initTestCase();

    void setUpdateTest();
    void objectUpdateTest();

    void cleanup();

    void cleanupTestCase();

};