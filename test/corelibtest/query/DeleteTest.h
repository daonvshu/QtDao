#pragma once

#include <qobject.h>

#include "BaseTest.h"

class DeleteTest : public BaseTest {
    Q_OBJECT

public:
    using BaseTest::BaseTest;

private slots:
    void initTestCase();

    void filterDeleteTest();
    void objectDeleteTest();

    void truncateTest_data();
    void truncateTest();

    void cleanup();

    void cleanupTestCase();

};