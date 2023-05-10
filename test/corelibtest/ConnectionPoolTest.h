#pragma once

#include <qobject.h>

#include "EngineModelSelector.h"

class ConnectionPoolTest : public QObject, public EngineModelSelector {
    Q_OBJECT

public:
    using EngineModelSelector::EngineModelSelector;

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
