#include "DeleteTest.h"

#include <QtTest>

#include "../../src/dao.h"

void DeleteTest::initTestCase() {
    DbLoader::init(SqliteConfig());

    data1 << SqliteTest1(1, "abc", 10, "");
    data1 << SqliteTest1(2, "alice", 11, "alice1");
    data1 << SqliteTest1(3, "bob", 12, "bob boom");
    data1 << SqliteTest1(4, "client", 14, "1");
    data1 << SqliteTest1(5, "client", 12, "xxx");
    dao::_insert<SqliteTest1>().build().insert2(data1);

    data2 << SqliteTest2(0, "joker", 9999, -1, 30);
    data2 << SqliteTest2(0, "bob", 10, 0, "abc");
    data2 << SqliteTest2(0, "func", 10, -2, 50);
    data2 << SqliteTest2(0, "func", 50, 0, 50);
    dao::_insert<SqliteTest2>().build().insert2(data2);
}

void DeleteTest::filterDeleteTest() {
    SqliteTest1::Fields sf1;
    bool success = dao::_delete<SqliteTest1>().filter(sf1.name.like("a%")).build().deleteBy();
    QVERIFY(success);
    auto result1 = dao::_select<SqliteTest1>().filter(sf1.name.in(QStringList() << "abc" << "alice")).build().list();
    QVERIFY(result1.isEmpty());

    QStringList target;
    target << "bob" << "client";
    success = dao::_delete<SqliteTest1>().filter(sf1.name == target).build().deleteBatch();
    QVERIFY(success);
    auto result2 = dao::_select<SqliteTest1>().filter(sf1.name.in(target)).build().list();
    QVERIFY(result2.isEmpty());
}

void DeleteTest::objectDeleteTest() {
    SqliteTest2::Fields sf2;
    auto result1 = dao::_select<SqliteTest2>().filter(sf2.name == "joker").build().unique();
    bool success = dao::_delete<SqliteTest2>().build().deleteBy(result1);
    QVERIFY(success);
    result1 = dao::_select<SqliteTest2>().filter(sf2.name == "joker").build().unique();
    QVERIFY(result1.getId() == -1);

    auto result2 = dao::_selectAll<SqliteTest2>();
    success = dao::_delete<SqliteTest2>().build().deleteBatch(result2);
    QVERIFY(success);
    result2 = dao::_selectAll<SqliteTest2>();
    QVERIFY(result2.isEmpty());
}

void DeleteTest::truncateTest() {
    data2.clear();
    data2 << SqliteTest2(0, "func2", 10, -2, 50);
    data2 << SqliteTest2(0, "func2", 50, 0, 50);
    dao::_insert<SqliteTest2>().build().insert2(data2);

    dao::_truncate<SqliteTest2>();
    auto select = dao::_selectAll<SqliteTest2>();
    QVERIFY(select.isEmpty());

    auto entity = SqliteTest2(0, "func2", 50, 0, 50);
    dao::_insert<SqliteTest2>().build().insert(entity);
    QVERIFY(entity.getId() == 1);
}

void DeleteTest::cleanup() {
    clearCacheAndPrintIfTestFail();
}

void DeleteTest::cleanupTestCase() {
    ConnectionPool::release();
    DbLoader::getClient().dropDatabase();
}

