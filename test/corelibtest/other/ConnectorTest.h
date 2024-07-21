#pragma once

#include <qobject.h>

#include "databaseselector.h"

class ConnectorTest : public QObject, public DatabaseSelector {
    Q_OBJECT

public:
    using DatabaseSelector::DatabaseSelector;

private slots:
    void initTestCase();

    void cleanup();

    void conditionConnectortest();
    void constraintConditionTest();
    void functionConnectorTest();
    void havingStatementTest();

    void cleanupTestCase();
};

