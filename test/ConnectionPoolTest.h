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
    void testMultiThreadOpenConnection();
    void testReuseConnectionInOtherThread();

    void testAutoClose();

    void testGoneAway();

    void cleanup();

    void cleanupTestCase();

private:
    void loadConfigByEngineModel();
};
