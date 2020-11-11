#pragma once

#include <qobject.h>

class BaseQueryTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();

    void testPrimitiveQuery(); 
    void testPrimitiveQueryWithValue();
    void testPrimitiveQueryFail();

    void cleanup();

    void cleanupTestCase();
};

