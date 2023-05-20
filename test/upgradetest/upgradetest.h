#pragma once

#include <qobject.h>

class UpgradeTest : public QObject {
    Q_OBJECT

public:
    using QObject::QObject;

private slots:
    void initTestCase();

    void upgradeTest();

    void fieldTest();

    void dataTest();

    void cleanup();

    void cleanupTestCase();
};
