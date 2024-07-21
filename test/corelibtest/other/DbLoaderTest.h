#pragma once

#include <qobject.h>

#include "databaseselector.h"

class DbLoaderTest : public QObject, public DatabaseSelector {
    Q_OBJECT

public:
    using DatabaseSelector::DatabaseSelector;

private slots:
    void initTestCase();

    void loadConfigTest();
    void upgradeTest();

    void cleanup();

    void cleanupTestCase();
};

