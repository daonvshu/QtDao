#pragma once

#include <qobject.h>

#include "EngineModelSelector.h"

class ConnectionPoolTest : public QObject, public EngineModelSelector {
    Q_OBJECT

public:
    using EngineModelSelector::EngineModelSelector;

private slots:
    void initTestCase();

    void testSqliteConnect();
    void testReuseConnection();
    void testMultiThreadOpenConnection();
    void testReuseConnectionInOtherThread();

    void testAutoClose();

    void cleanup();

    void cleanupTestCase();

};
