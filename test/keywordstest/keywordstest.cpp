#include "keywordstest.h"

#include "dao.h"

#include <qtest.h>

#include "database.h"

using namespace dao;

void KeywordsTest::initTestCase() {
}

void KeywordsTest::testStep() {
    try {
        if (TEST_DB == QLatin1String("sqlite")) {
            dao::_config<dao::ConfigSqliteBuilder>()
                    .version(1)
                    .databaseName("sqlite_keywords_test")
                    .initializeDatabase();
        } else if (TEST_DB == QLatin1String("mysql")) {
            dao::_config<dao::ConfigMysqlBuilder>()
                    .version(1)
                    .databaseName("mysql_keywords_test")
                    .host("localhost")
                    .port(3306)
                    .user("root")
                    .password("root")
                    .initializeDatabase();
        } else if (TEST_DB == QLatin1String("sqlserver")) {
            dao::_config<dao::ConfigSqlServerBuilder>()
                    .version(1)
                    .databaseName("sqlserver_keywords_test")
                    .host("localhost")
                    .user("sa")
                    .password("root")
                    .initializeDatabase();
        }
    } catch (dao::DaoException& e) {
        Q_UNUSED(e)
        auto validDrivers = QSqlDatabase::drivers();
        Q_UNUSED(validDrivers)
        qFatal("setup database fail!");
    }

    dao::_truncate<Database>();
}

QPair<QList<int>, QStringList> readAll() {
    auto query = BaseQuery::queryPrimitive(QString("select *from %1").arg(Database::Info::getTableName()));
    QList<int> columns;
    QStringList groups;
    while (query.next()) {
        columns << query.value("column").toInt();
        groups << query.value("group").toString();
    }
    return qMakePair(columns, groups);
}

void KeywordsTest::testInsert() {
    DatabaseList databaseList {
        Database(1, "group_num1"),
        Database(2, "group_ku2"),
    };
    dao::_insert<Database>().build().insert(databaseList);

    auto allData = readAll();
    QCOMPARE(allData.first, QList<int>() << 1 << 2);
    QCOMPARE(allData.second, QStringList() << "group_num1" << "group_ku2");
}

void KeywordsTest::testSelect() {
    Database::Fields df;
    auto result = dao::_select<Database>()
        .column(df.group, df.column)
        .filter(df.group.like("group_%"))
        .with(_orderBy(df.group))
        .build().list();

    QList<int> columns;
    QStringList groups;
    for (const auto& r : result) {
        columns << r.getColumn();
        groups << r.getGroup();
    }
    QCOMPARE(columns, QList<int>() << 2 << 1);
    QCOMPARE(groups, QStringList() << "group_ku2" << "group_num1");
}

void KeywordsTest::testUpdate() {
    Database::Fields df;
    dao::_update<Database>()
        .set(df.column = 10, df.group = "groups")
        .filter(df.column == 1)
        .build().update();

    auto allData = readAll();
    QCOMPARE(allData.first, QList<int>() << 10 << 2);
    QCOMPARE(allData.second, QStringList() << "groups" << "group_ku2");
}

void KeywordsTest::testJoin() {
    class DatabaseTmp : public dao::self<Database> {};
    DatabaseTmp::Fields dtf;
    Database::Fields df;

    dao::_join<Database, DatabaseTmp>()
        .from<Database>()
        .innerJoin<DatabaseTmp>().on(dtf.column == df.column)
        .filter(df.column > 0)
        .build().list();
}

void KeywordsTest::cleanup() {

}

void KeywordsTest::cleanupTestCase() {
    dao::globalConfig->getClient()->dropDatabase();
}
