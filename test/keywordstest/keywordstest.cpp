#include "keywordstest.h"

#include "dao.h"

#include <qtest.h>

#include "sqlite/database.h"
#include "mysql/database.h"
#include "sqlserver/database.h"
#include "psql/database.h"

#include "utils/testconfigloader.h"

using namespace dao;

KeywordsTest::KeywordsTest()
    : DatabaseSelector(TestConfigLoader::instance().config().getTestTargetDb())
{}

void KeywordsTest::initTestCase() {
}

void KeywordsTest::testStep() {
    setupDatabase("keywords_test", 1);
}

template<typename T>
QPair<QList<int>, QStringList> readAll() {
    auto query = BaseQuery::queryPrimitive(QString("select *from %1 order by \"group\"").arg(T::Info::getTableName()));
    QList<int> columns;
    QStringList groups;
    while (query.next()) {
        columns << query.value("column").toInt();
        groups << query.value("group").toString();
    }
    return qMakePair(columns, groups);
}

#define runTestImpl(func) \
const auto& targetDb = TestConfigLoader::instance().config().getTestTargetDb(); \
if (targetDb == TestTargetDb::Target_Sqlite) { \
    func<TsSqlite::Database>(); \
} else if (targetDb == TestTargetDb::Target_Mysql) { \
    func<TsMysql::Database>(); \
} else if (targetDb == TestTargetDb::Target_SqlServer) { \
    func<TsSqlServer::Database>(); \
} else if (targetDb == TestTargetDb::Target_PSql) { \
    func<TsPSql::Database>(); \
}

template<typename Database>
void testInsertImpl() {
    QList<Database> databaseList {
        Database(1, "group_num1"),
        Database(2, "group_ku2"),
    };
    dao::_insert<Database>().build().insert(databaseList);

    auto allData = readAll<Database>();
    QCOMPARE(allData.first, QList<int>() << 2 << 1);
    QCOMPARE(allData.second, QStringList() << "group_ku2" << "group_num1");
}

void KeywordsTest::testInsert() {
    runTestImpl(testInsertImpl)
}

template<typename Database>
void testSelectImpl() {
    typename Database::Fields df;
    auto result = dao::_select<Database>()
        .column(df.group, df.column)
        .filter(df.group.like("group_%"))
        .with(_orderBy(df.group))
        .build().list();

    QList<int> columns;
    QStringList groups;
    for (const auto& r : result) {
        columns << r.column;
        groups << r.group;
    }
    QCOMPARE(columns, QList<int>() << 2 << 1);
    QCOMPARE(groups, QStringList() << "group_ku2" << "group_num1");
}

void KeywordsTest::testSelect() {
    runTestImpl(testSelectImpl)
}

template<typename Database>
void testUpdateImpl() {
    typename Database::Fields df;
    dao::_update<Database>()
        .set(df.column = 10, df.group = "groups")
        .filter(df.column == 1)
        .build().update();

    auto allData = readAll<Database>();
    QCOMPARE(allData.first, QList<int>() << 2 << 10);
    QCOMPARE(allData.second, QStringList()<< "group_ku2" << "groups" );
}

void KeywordsTest::testUpdate() {
    runTestImpl(testUpdateImpl)
}

template<typename Database>
void testJoinImpl() {
    class DatabaseTmp : public dao::self<Database> {};
    typename DatabaseTmp::Fields dtf;
    typename Database::Fields df;

    dao::_join<Database, DatabaseTmp>()
        .template from<Database>()
        .template innerJoin<DatabaseTmp>().on(dtf.column == df.column)
        .filter(df.column > 0)
        .build().list();
}

void KeywordsTest::testJoin() {
    runTestImpl(testJoinImpl)
}

void KeywordsTest::upgradeTest() {
    setupDatabase("keywords_test", 2);
}

void KeywordsTest::cleanup() {

}

void KeywordsTest::cleanupTestCase() {
    dao::ConfigManager::getConfig()->getClient()->dropDatabase();
}
