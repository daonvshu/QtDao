#include "BaseQueryTest.h"

#include "DbLoader.h"

#include "dbclients/SqliteClient.h"

#include "query/BaseQuery.h"

#include <qtest.h>

class BaseQueryTestException : public DbExceptionHandler {
public:
    void execFail(const QString& lastErr) {
        QFAIL(lastErr.toLatin1());
    }
};

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
    });
}

void BaseQueryTest::cleanup() {
}

void BaseQueryTest::cleanupTestCase() {
    DbLoader::getClient().dropDatabase();
}
