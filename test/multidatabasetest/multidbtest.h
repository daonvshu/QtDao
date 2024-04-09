#pragma once

#include <qobject.h>

class MultiDbTest : public QObject {
    Q_OBJECT

public:
    using QObject::QObject;

private slots:
    void initTestCase();

    void cleanup();

    void cleanupTestCase();

    void setupTest();

    void sqliteNormalQueryTest();

    void differentDbQueryTest();

    void multiThreadTest();
};
