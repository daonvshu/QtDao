#pragma once

#include "../BaseTest.h"

class LoggingTest : public BaseTest {
    Q_OBJECT

public:
    using BaseTest::BaseTest;

private slots:
    void initTestCase();

    void singleStatementTest();

    void statementValueTest();

    void batchQueryTest();

    void rulerTest();

    void cleanup();

    void cleanupTestCase();
};
