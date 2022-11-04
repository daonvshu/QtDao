#pragma once

#include <qobject.h>

#include "EngineModelSelector.h"

class ConnectorTest : public QObject, public EngineModelSelector {
    Q_OBJECT

public:
    using EngineModelSelector::EngineModelSelector;

private slots:
    void initTestCase();

    void cleanup();

    void conditionConnectortest();
    void constraintConditionTest();
    void functionConnectorTest();
    void havingStatementTest();

    void cleanupTestCase();
};

