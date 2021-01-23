#include "InsertIntoSelectTest.h"

#include <QtTest>

#include "dao.h"

void InsertIntoSelectTest::initTestCase() {
    configDb();

    if (engineModel == Engine_Sqlite) {
        sqliteData1 << SqliteTest1(1, "abc", 10, "");
        sqliteData1 << SqliteTest1(2, "alice", 11, "alice1");
        sqliteData1 << SqliteTest1(3, "bob", 12, "bob boom");
        sqliteData1 << SqliteTest1(4, "client", 14, "1");
        sqliteData1 << SqliteTest1(5, "client", 12, "xxx");
        dao::_insert<SqliteTest1>().build().insert2(sqliteData1);
    } else if (engineModel == Engine_Mysql) {
        mysqlData1 << MysqlTest1(1, "abc", 10, "");
        mysqlData1 << MysqlTest1(2, "alice", 11, "alice1");
        mysqlData1 << MysqlTest1(3, "bob", 12, "bob boom");
        mysqlData1 << MysqlTest1(4, "client", 14, "1");
        mysqlData1 << MysqlTest1(5, "client", 12, "xxx");
        dao::_insert<MysqlTest1>().build().insert2(mysqlData1);
    }

    clearCacheAndPrintIfTestFail();
}

void InsertIntoSelectTest::testInsertIntoSelect_data() {
    if (engineModel == Engine_Sqlite) {
        QTest::addColumn<SqliteTest1List>("data1");
        QTest::newRow("sqlite test data") << sqliteData1;
    } else if (engineModel == Engine_Mysql) {
        QTest::addColumn<MysqlTest1List>("data1");
        QTest::newRow("mysql test data") << mysqlData1;
    }
}

template<typename E1, typename E2>
void runTestInsertIntoSelect() {
    typename E1::Fields sf1;
    typename E2::Fields sf2;

    auto select = dao::_select<E1>().column(sf1.name, sf1.number).filter(sf1.number < 12).build();
    bool success = dao::_insertIntoSelect<E2>()
        .column(sf2.name, sf2.number)
        .from (select)
        .build().insert();
    QVERIFY(success);
    if (success) {
        auto test2 = dao::_selectAll<E2>();
        QVariantList data;
        for (const auto& r : test2) {
            data << r.getName() << r.getNumber();
        }
        QVariantList expected;
        QFETCH(QList<E1>, data1);
        for (const auto& r : data1) {
            if (r.getNumber() < 12) {
                expected << r.getName() << r.getNumber();
            }
        }
        QCOMPARE(data, expected);
    }
}

void InsertIntoSelectTest::testInsertIntoSelect() {
    if (engineModel == Engine_Sqlite) {
        runTestInsertIntoSelect<SqliteTest1, SqliteTest2>();
    } else if (engineModel == Engine_Mysql) {
        runTestInsertIntoSelect<MysqlTest1, MysqlTest2>();
    }
}

void InsertIntoSelectTest::testInsertIntoJoin_data() {
    if (engineModel == Engine_Sqlite) {
        QTest::addColumn<SqliteTest1List>("data1");
        QTest::newRow("sqlite test data") << sqliteData1;
    } else if (engineModel == Engine_Mysql) {
        QTest::addColumn<MysqlTest1List>("data1");
        QTest::newRow("mysql test data") << mysqlData1;
    }
}

template<typename E1, typename E2>
void runTestInsertIntoJoin() {
    typename E1::Fields sf1;
    typename E2::Fields sf2;

    auto join = dao::_join<E1, E2>()
        .column(sf1.name, _fun("%1 + 10 as number").field(sf1.number))
        .template from<E1>()
        .template innerJoin<E2>().on(sf2.name == sf1.name)
        .build();
    bool success = dao::_insertIntoSelect<E2>()
        .column(sf2.name, sf2.number)
        .from(join)
        .build().insert();
    QVERIFY(success);
    QVERIFY(success);
    if (success) {
        auto test2 = dao::_selectAll<E2>();
        QVariantList data;
        for (const auto& r : test2) {
            data << r.getName() << r.getNumber();
        }
        QVariantList expected;
        QFETCH(QList<E1>, data1);
        for (int i = 0; i <= 10; i += 10) {
            for (const auto& r : data1) {
                if (r.getNumber() < 12) {
                    expected << r.getName() << (r.getNumber() + i);
                }
            }
        }
        QCOMPARE(data, expected);
    }
}

void InsertIntoSelectTest::testInsertIntoJoin() {
    if (engineModel == Engine_Sqlite) {
        runTestInsertIntoJoin<SqliteTest1, SqliteTest2>();
    } else if (engineModel == Engine_Mysql) {
        runTestInsertIntoJoin<MysqlTest1, MysqlTest2>();
    }
}

void InsertIntoSelectTest::cleanup() {
    clearCacheAndPrintIfTestFail();
}

void InsertIntoSelectTest::cleanupTestCase() {
    ConnectionPool::release();
    DbLoader::getClient().dropDatabase();
}
