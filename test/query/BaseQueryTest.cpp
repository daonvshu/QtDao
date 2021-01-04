#include "BaseQueryTest.h"

#include "../../src/DbLoader.h"
#include "../../src/ConnectionPool.h"

#include "../../src/query/BaseQuery.h"

#include <QtTest>

void BaseQueryTest::initTestCase() {
    if (engineModel == Engine_Sqlite) {
        DbLoader::loadConfig(SqliteConfig());
    } else if (engineModel == Engine_Mysql) {
        DbLoader::init(MysqlConfig());
    }
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

    try {
        auto query = BaseQuery::queryPrimitiveThrowable("select 10 + 20");
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

    try {
        auto query = BaseQuery::queryPrimitiveThrowable("select ? + ?", QVariantList() << 10 << 20);
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
    BaseQuery::queryPrimitive("select?+?", [&](QSqlQuery& query) {
        Q_UNUSED(query);
        QFAIL("primitive query should fail!");
    }, [&](QString err) {
        Q_UNUSED(err);
    });

    try {
        BaseQuery::queryPrimitiveThrowable("select?+?");
        QFAIL("primitive query should fail!");
    }
    catch (DaoException&) {
    }
}

void BaseQueryTest::cleanup() {
    clearCacheAndPrintIfTestFail();
}

void BaseQueryTest::cleanupTestCase() {
    ConnectionPool::release();
    DbLoader::getClient().dropDatabase();
}

