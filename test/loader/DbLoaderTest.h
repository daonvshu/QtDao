#pragma once

#include <qobject.h>

#include "../EngineModelSelector.h"

class DbLoaderTest : public QObject, public EngineModelSelector {
    Q_OBJECT

public:
    using EngineModelSelector::EngineModelSelector;

private slots:
    void initTestCase();

    void sqliteloadConfigTest();
    void sqliteUpgradeTest();

    void cleanup();

    void cleanupTestCase();
};

