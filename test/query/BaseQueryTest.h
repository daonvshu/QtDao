#pragma once

#include <qobject.h>

#include "../BaseTest.h"

class BaseQueryTest : public BaseTest {
    Q_OBJECT

private slots:
    void initTestCase();

    void testPrimitiveQuery(); 
    void testPrimitiveQueryWithValue();
    void testPrimitiveQueryFail();

    void cleanup();

    void cleanupTestCase();
};

