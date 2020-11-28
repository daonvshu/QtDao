#pragma once

#include <qobject.h>

class ConnectorTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();

    void cleanup();

    void conditionConnectortest();
    void constraintConditionTest();
    void functionConnectorTest();

    void cleanupTestCase();
};

