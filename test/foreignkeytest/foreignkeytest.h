#pragma once

#include <qobject.h>

#include "databaseselector.h"

class ForeignKeyTest : public QObject, public DatabaseSelector {
    Q_OBJECT

public:
    ForeignKeyTest();

private slots:
    void initTestCase();

    void testInsert();

    void testUpdate();

    void testDelete();

    void testTransaction();

    void testVersionUpgrade();

    void testRunInThread();

    void cleanup();

    void cleanupTestCase();
};
