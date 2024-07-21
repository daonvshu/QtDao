#pragma once

#include <qobject.h>

#include "databaseselector.h"

class ConnectionPoolTest : public QObject, public DatabaseSelector {
    Q_OBJECT

public:
    using DatabaseSelector::DatabaseSelector;

private slots:
    void initTestCase();

    void testConnect();
    void testReuseConnection();
    void testSerialThread();
    void testParallelThread();
    void testSerialThreadPool();
    void testParallelThreadPool();

    void testGoneAway();

    void cleanup();

    void cleanupTestCase();
};
