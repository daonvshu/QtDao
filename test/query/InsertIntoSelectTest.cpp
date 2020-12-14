#include "InsertIntoSelectTest.h"

#include <qtest.h>

#include "../../src/dao.h"

void InsertIntoSelectTest::initTestCase() {
    DbLoader::init(SqliteConfig());

    data1 << SqliteTest1(1, "abc", 10, "");
    data1 << SqliteTest1(2, "alice", 11, "alice1");
    data1 << SqliteTest1(3, "bob", 12, "bob boom");
    data1 << SqliteTest1(4, "client", 14, "1");
    data1 << SqliteTest1(5, "client", 12, "xxx");
    dao::_insert<SqliteTest1>().build().insert2(data1);

    clearCacheAndPrintIfTestFail();
}

void InsertIntoSelectTest::testInsertIntoSelect() {
    SqliteTest1::Fields sf1;
    SqliteTest2::Fields sf2;

    bool success = dao::_insertIntoSelect<SqliteTest2>()
        .column(sf2.name, sf2.number)
        .from(
            dao::_select<SqliteTest1>().column(sf1.name, sf1.number).filter(sf1.number < 12).build()
        )
        .build().insert();
    QVERIFY(success);
    if (success) {
        auto test2 = dao::_selectAll<SqliteTest2>();
        QVariantList data;
        for (const auto& r : test2) {
            data << r.getName() << r.getNumber();
        }
        QVariantList expected;
        for (const auto& r : data1) {
            if (r.getNumber() < 12) {
                expected << r.getName() << r.getNumber();
            }
        }
        QCOMPARE(data, expected);
    }
}

void InsertIntoSelectTest::testInsertIntoJoin() {
    SqliteTest1::Fields sf1;
    SqliteTest2::Fields sf2;

    auto join = dao::_join<SqliteTest1, SqliteTest2>()
        .column(sf1.name, _fun("%1 + 10 as number").field(sf1.number))
        .from<SqliteTest1>()
        .innerJoin<SqliteTest2>().on(sf2.name == sf1.name)
        .build();
    bool success = dao::_insertIntoSelect<SqliteTest2>()
        .column(sf2.name, sf2.number)
        .from(join)
        .build().insert();
    QVERIFY(success);
    QVERIFY(success);
    if (success) {
        auto test2 = dao::_selectAll<SqliteTest2>();
        QVariantList data;
        for (const auto& r : test2) {
            data << r.getName() << r.getNumber();
        }
        QVariantList expected;
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

void InsertIntoSelectTest::cleanup() {
    clearCacheAndPrintIfTestFail();
}

void InsertIntoSelectTest::cleanupTestCase() {
    ConnectionPool::release();
    DbLoader::getClient().dropDatabase();
}
