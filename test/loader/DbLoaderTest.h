#pragma once

#include <qobject.h>

class DbLoaderTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();

    void sqliteloadConfigTest();
    void sqliteUpgradeTest();

    void cleanup();

    void cleanupTestCase();
};

