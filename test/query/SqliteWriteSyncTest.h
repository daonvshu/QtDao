#pragma once

#include <qobject.h>

#include "../BaseTest.h"

class SqliteWriteSyncTest : public BaseTest {
    Q_OBJECT

public:
    using BaseTest::BaseTest;

private slots:
    void initTestCase();

    void testInsertWriteLock();
    void testTranscationWriteLock();

    void cleanup();

    void cleanupTestCase();
};