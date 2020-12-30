#include "UpdateTest.h"

#include <QtTest>

#include "../../src/dao.h"

void UpdateTest::initTestCase() {
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

void UpdateTest::setUpdateTest() {
    SqliteTest1::Fields sf1;
    SqliteTest1::Tool tool1;
    bool success = dao::_update<SqliteTest1>()
        .set(sf1.number = 18, sf1.hex = "test update")
        .filter(sf1.name == "bob")
        .build().update();
    QVERIFY(success);
    auto result = dao::_select<SqliteTest1>().filter(sf1.number == 18).build().unique();
    QVariantList resultData = tool1.getValueWithoutAutoIncrement(result);
    QCOMPARE(resultData,
        QVariantList() << 3 << "bob" << 18 << "test update"
    );

    QList<qreal> numbersSet;
    numbersSet << 21 << 22;
    QList<QByteArray> hexSet;
    hexSet << "test1" << "test2";
    QStringList targets;
    targets << "bob" << "client";
    success = dao::_update<SqliteTest1>()
        .set(sf1.number = numbersSet, sf1.hex = hexSet)
        .filter(sf1.name == targets)
        .build().updateBatch();
    QVERIFY(success);
    auto results = dao::_select<SqliteTest1>().filter(sf1.name.in(targets)).build().list();
    QList<QVariantList> resultsData;
    for (const auto& r : results) {
        resultsData << tool1.getValueWithoutAutoIncrement(r);
    }
    QList<QVariantList> expectValues;
    expectValues << (QVariantList() << 3 << "bob" << 21 << "test1");
    expectValues << (QVariantList() << 4 << "client" << 22 << "test2");
    expectValues << (QVariantList() << 5 << "client" << 22 << "test2");
    QCOMPARE(resultsData, expectValues);
}

void UpdateTest::objectUpdateTest() {
    SqliteTest2::Fields sf2;
    auto t2test = dao::_select<SqliteTest2>().filter(sf2.name == "bob").build().unique();
    t2test.setNumber(50);
    bool success = dao::_update<SqliteTest2>().build().update(t2test);
    QVERIFY(success);
    auto t2result = dao::_select<SqliteTest2>().filter(sf2.name == "bob").build().unique();
    QCOMPARE(
        SqliteTest2::Tool::getValueWithoutAutoIncrement(t2result),
        SqliteTest2::Tool::getValueWithoutAutoIncrement(t2test)
    );

    auto t2stest = dao::_select<SqliteTest2>().filter(sf2.number < 100).with(_orderBy(sf2.id)).build().list();
    for (auto& t2 : t2stest) {
        t2.setVarianttype("testt2s");
    }
    success = dao::_update<SqliteTest2>().build().updateBatch(t2stest);
    QVERIFY(success);
    auto t2results = dao::_select<SqliteTest2>().filter(sf2.varianttype == "testt2s").with(_orderBy(sf2.id)).build().list();
    QList<QVariantList> actualValues, expectValues;
    for (const auto& t : t2results) {
        actualValues << SqliteTest2::Tool::getValueWithoutAutoIncrement(t);
    }
    for (const auto& t : t2stest) {
        expectValues << SqliteTest2::Tool::getValueWithoutAutoIncrement(t);
    }
    QCOMPARE(actualValues, expectValues);
}

void UpdateTest::cleanup() {
    clearCacheAndPrintIfTestFail();
}

void UpdateTest::cleanupTestCase() {
    ConnectionPool::release();
    DbLoader::getClient().dropDatabase();
}

