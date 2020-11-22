#pragma once

#include <qobject.h>

class InsertTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();

    void setInsertTest();
    void setInsertBatchTest();
    void insertObjectTest();
    void insertObjectsTest();
    void insertObjects2Test();

    void cleanup();

    void cleanupTestCase();
};