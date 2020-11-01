#pragma once

#include <qobject.h>

class ConnectionPoolTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();

    void testSqliteConnect();
    void testReuseConnection();
    void testMultiThreadOpenConnection();
    void testReuseConnectionInOtherThread();

    void cleanup();

    void cleanupTestCase();

};
