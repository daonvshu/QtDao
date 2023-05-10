#include "BaseQueryTest.h"

#include "connectionpool.h"
#include "dbexception.h"

#include <QtTest>

void BaseQueryTest::initTestCase() {
    configDb();
    globalConfig->getClient()->testConnect();
}

void BaseQueryTest::testPrimitiveQuery() {
    try {
        auto query = BaseQuery::queryPrimitive("select 10 + 20");
        if (query.next()) {
            int result = query.value(0).toInt();
            QCOMPARE(result, 30);
        } else {
            QFAIL("primitive query fail!");
        }
    }
    catch (DaoException& e) {
        QFAIL(e.reason.toLatin1());
    }
}

void BaseQueryTest::testPrimitiveQueryWithValue() {
    try {
        auto query = BaseQuery::queryPrimitive("select ? + ?", QVariantList() << 10 << 20);
        if (query.next()) {
            int result = query.value(0).toInt();
            QCOMPARE(result, 30);
        } else {
            QFAIL("primitive query fail!");
        }
    }
    catch (DaoException& e) {
        QFAIL(e.reason.toLatin1());
    }
}

void BaseQueryTest::testPrimitiveQueryFail() {
    try {
        BaseQuery::queryPrimitive("select?+?", QVariantList(), nullptr, false);
        QFAIL("primitive query should fail!");
    }
    catch (DaoException&) {
    }
}

void BaseQueryTest::cleanup() {
    clearCacheAndPrintIfTestFail();
}

void BaseQueryTest::cleanupTestCase() {
    globalConfig->getClient()->dropDatabase();
}

