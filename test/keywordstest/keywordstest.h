#pragma once

#include <qobject.h>

#include "databaseselector.h"

class KeywordsTest : public QObject, public DatabaseSelector {
    Q_OBJECT

public:
    KeywordsTest();

private slots:
    void initTestCase();

    void testStep();

    void testInsert();

    void testSelect();

    void testUpdate();

    void testJoin();

    void upgradeTest();

    void cleanup();

    void cleanupTestCase();
};
