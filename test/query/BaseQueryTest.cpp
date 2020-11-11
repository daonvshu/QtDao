#include "BaseQueryTest.h"

#include "DbLoader.h"
#include "ConnectionPool.h"

#include "dbclients/SqliteClient.h"

#include "query/BaseQuery.h"

#include <qtest.h>

void BaseQueryTest::initTestCase() {
    DbLoader::loadConfig(":/QtDao/entity/sqlite_cfg.xml");
    DbLoader::getClient().testConnect();
}

void BaseQueryTest::testPrimitiveQuery() {
    BaseQuery::queryPrimitive("select 10 + 20", [&](QSqlQuery& query) {
        if (query.next()) {
            int result = query.value(0).toInt();
            QCOMPARE(result, 30);
        } else {
            QFAIL("primitive query fail!");
        }
    }, [&](QString err) {
        QFAIL(err.toLatin1());
    });
}

void BaseQueryTest::testPrimitiveQueryWithValue() {
    BaseQuery::queryPrimitive("select ? + ?", QVariantList() << 10 << 20, [&](QSqlQuery& query) {
        if (query.next()) {
            int result = query.value(0).toInt();
            QCOMPARE(result, 30);
        } else {
            QFAIL("primitive query fail!");
        }
    }, [&](QString err) {
        QFAIL(err.toLatin1());
    });
}

void BaseQueryTest::testPrimitiveQueryFail() {
    BaseQuery::queryPrimitive("select?+?", [&](QSqlQuery& query) {
        QFAIL("primitive query should fail!");
    }, [&](QString err) {});
}

void BaseQueryTest::cleanup() {
}

void BaseQueryTest::cleanupTestCase() {
    ConnectionPool::release();
    DbLoader::getClient().dropDatabase();
}
