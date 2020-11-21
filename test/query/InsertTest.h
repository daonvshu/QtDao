#pragma once

#include <qobject.h>

class InsertTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();

    void setInsertTest();
    void setInsertBatchTest();

    void cleanup();

    void cleanupTestCase();
};