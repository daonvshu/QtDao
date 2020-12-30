#include "JoinTest.h"

#include <QtTest>

#include "../sqliteentity/SqliteTest1.h"
#include "../sqliteentity/SqliteTest2.h"

#include "../../src/dao.h"

void JoinTest::initTestCase() {
    DbLoader::init(SqliteConfig());

    data1 << SqliteTest1(1, "abc", 10, "");
    data1 << SqliteTest1(2, "alice", 11, "alice1");
    data1 << SqliteTest1(3, "bob", 12, "bob boom");
    data1 << SqliteTest1(4, "client", 14, "1");
    data1 << SqliteTest1(5, "client", 12, "xxx");
    dao::_insert<SqliteTest1>().build().insert2(data1);

    data2 << SqliteTest2(0, "joker", 9999, -1, 30);
    data2 << SqliteTest2(0, "bob", 9, 9999, "abc");
    data2 << SqliteTest2(0, "func", 10, 9, 50);
    data2 << SqliteTest2(0, "func", 50, 10, 50);
    dao::_insert<SqliteTest2>().build().insert2(data2);

    data3 << SqliteTest3(0, 3, 2, "bob group", 2);
    data3 << SqliteTest3(0, 4, 1, "client group1", 3);
    data3 << SqliteTest3(0, 5, 1, "client group2", 3);
    data3 << SqliteTest3(0, 1, 3, "func group1", 6);
    data3 << SqliteTest3(0, 1, 4, "func group2", 7);
    dao::_insert<SqliteTest3>().build().insert2(data3);
}

void JoinTest::testJoinTable() {
    SqliteTest1::Fields sf1;
    SqliteTest2::Fields sf2;
    SqliteTest3::Fields sf3;

    auto result = dao::_join<SqliteTest1, SqliteTest2, SqliteTest3>()
        .column(sf2.name, sf1.name, sf3.name, sf1.number, sf2.number)
        .from<SqliteTest1>()
        .innerJoin<SqliteTest2>().on(sf2.id == sf3.tbi2)
        .innerJoin<SqliteTest3>().on(sf3.tbi1 == sf1.id)
        .build().list();
    QVariantList data;
    for (const auto& r : result) {
        auto s1 = std::get<0>(r);
        auto s2 = std::get<1>(r);
        auto s3 = std::get<2>(r);
        data << s2.getName() << s1.getName() << s3.getName() << s1.getNumber() << s2.getNumber();
    }
    QCOMPARE(
        data,
        QVariantList() << "bob" << "bob" << "bob group" << 12 << 9
            << "joker" << "client" << "client group1" << 14 << 9999
            << "joker" << "client" << "client group2" << 12 << 9999
            <<"func" << "abc" << "func group1" << 10 << 10
            <<"func" << "abc" << "func group2" << 10 << 50
    );

    auto result2 = dao::_join<SqliteTest1, SqliteTest2, SqliteTest3>()
        .column(sf2.name, _fun("sum(%1) as sumn").field(sf1.number))
        .from<SqliteTest1>()
        .innerJoin<SqliteTest2>().on(sf2.id == sf3.tbi2)
        .innerJoin<SqliteTest3>().on(sf3.tbi1 == sf1.id)
        .with(_groupBy(sf2.name), _orderBy(sf2.name))
        .build().list();
    data.clear();
    for (const auto& r : result2) {
        data << std::get<1>(r).getName();
        data << std::get<0>(r).__getExtra("sumn");
    }
    QCOMPARE(
        data,
        QVariantList() << "bob" << 12 << "func" << 20 << "joker" << 26
    );

    auto result3 = dao::_join<SqliteTest1, SqliteTest2, SqliteTest3>()
        .from<SqliteTest1>()
        .innerJoin<SqliteTest2>().on(sf2.id == sf3.tbi2)
        .innerJoin<SqliteTest3>().on(sf3.tbi1 == sf1.id, sf3.name == "func group1")
        .build().list();
    data.clear();
    for (const auto& r : result3) {
        data << SqliteTest1::Tool::getValueWithoutAutoIncrement(std::get<0>(r));
        data << SqliteTest2::Tool::getValueWithoutAutoIncrement(std::get<1>(r));
        data << SqliteTest3::Tool::getValueWithoutAutoIncrement(std::get<2>(r));
    }
    QCOMPARE(
        data,
        QVariantList() << 1 << "abc" << 10 << "" << "func" << 10 << 9 << 50 << 1 << 3 << "func group1" << 6
    );
}

void JoinTest::testJoinSelfTable() {
    class SqliteTest2Tmp : public dao::self<SqliteTest2> {};
    SqliteTest2::Fields sf1;
    SqliteTest2Tmp::Fields sf2;
    auto result = dao::_join<SqliteTest2, SqliteTest2Tmp>()
        .column(sf1.name, sf2.name)
        .from<SqliteTest2>()
        .innerJoin<SqliteTest2Tmp>().on(sf2.number2 == sf1.number)
        .build().list();
    QVariantList data;
    for (const auto& r : result) {
        data << std::get<0>(r).getName();
        data << std::get<1>(r).getName();
    }
    QCOMPARE(
        data,
        QVariantList() << "bob" << "func" << "func" << "func" << "joker" << "bob"
    );

    class SqliteTest2Tmp2 : public dao::self<SqliteTest2Tmp> {};
    SqliteTest2Tmp2::Fields sf3;
    auto result2 = dao::_join<SqliteTest2, SqliteTest2Tmp, SqliteTest2Tmp2>()
        .column(sf1.name, sf2.name, sf3.name)
        .from<SqliteTest2>()
        .with(_orderBy(sf1.name.desc()))
        .innerJoin<SqliteTest2Tmp>().on(sf2.number2 == sf1.number)
        .innerJoin<SqliteTest2Tmp2>().on(sf3.number2 == sf2.number)
        .build().list();
    data.clear();
    for (const auto& r : result2) {
        data << std::get<0>(r).getName();
        data << std::get<1>(r).getName();
        data << std::get<2>(r).getName();
    }
    QCOMPARE(
        data,
        QVariantList() << "joker" << "bob" << "func" << "bob" << "func" << "func"
    );
}

void JoinTest::testSelectFromJoin() {
    SqliteTest1::Fields sf1;
    SqliteTest2::Fields sf2;
    SqliteTest3::Fields sf3;

    auto join1 = dao::_join<SqliteTest1, SqliteTest2, SqliteTest3>()
        .column(sf2.name, sf1.name, sf3.name, sf1.number, sf2.number)
        .from<SqliteTest1>()
        .innerJoin<SqliteTest2>().on(sf2.id == sf3.tbi2)
        .innerJoin<SqliteTest3>().on(sf3.tbi1 == sf1.id)
        .build();

    try {
        auto count1 = dao::_count<SqliteTest1>()
            .from(join1)
            .filter(sf1.number > 10)
            .count();
        QCOMPARE(count1, 3);
    }
    catch (DaoException& e) {
        QFAIL(("test join in select fail: " + e.reason).toUtf8());
    }
}

void JoinTest::testJoinFromSelect() {
    SqliteTest1::Fields sf1;
    SqliteTest2::Fields sf2;
    SqliteTest3::Fields sf3;

    auto select = dao::_select<SqliteTest1>().filter(sf1.name == "client").build();

    auto join = dao::_join<SqliteTest1, SqliteTest2, SqliteTest3>()
        .column(sf2.name, sf1.name, sf3.name, sf1.number, sf2.number)
        .from(select)
        .innerJoin<SqliteTest2>().on(sf2.id == sf3.tbi2)
        .innerJoin<SqliteTest3>().on(sf3.tbi1 == sf1.id)
        .build().list();

    QVariantList data;
    for (const auto& r : join) {
        auto s1 = std::get<0>(r);
        auto s2 = std::get<1>(r);
        auto s3 = std::get<2>(r);
        data << s2.getName() << s1.getName() << s3.getName() << s1.getNumber() << s2.getNumber();
    }
    QCOMPARE(
        data,
        QVariantList()
        << "joker" << "client" << "client group1" << 14 << 9999
        << "joker" << "client" << "client group2" << 12 << 9999
    );
}

void JoinTest::testJoinOnSelect() {
    SqliteTest1::Fields sf1;
    SqliteTest2::Fields sf2;
    SqliteTest3::Fields sf3;

    auto select = dao::_select<SqliteTest2>().filter(sf2.name == "joker").build();

    auto join = dao::_join<SqliteTest1, SqliteTest2, SqliteTest3>()
        .column(sf2.name, sf1.name, sf3.name, sf1.number, sf2.number)
        .from<SqliteTest1>()
        .innerJoin(select).on(sf2.id == sf3.tbi2)
        .innerJoin<SqliteTest3>().on(sf3.tbi1 == sf1.id)
        .build().list();

    QVariantList data;
    for (const auto& r : join) {
        auto s1 = std::get<0>(r);
        auto s2 = std::get<1>(r);
        auto s3 = std::get<2>(r);
        data << s2.getName() << s1.getName() << s3.getName() << s1.getNumber() << s2.getNumber();
    }
    QCOMPARE(
        data,
        QVariantList()
        << "joker" << "client" << "client group1" << 14 << 9999
        << "joker" << "client" << "client group2" << 12 << 9999
    );
}

void JoinTest::testSelectUnionJoin() {
    SqliteTest1::Fields sf1;
    SqliteTest2::Fields sf2;
    SqliteTest3::Fields sf3;

    auto join = dao::_join<SqliteTest1, SqliteTest2, SqliteTest3>()
        .column(sf1.name, sf2.number)
        .from<SqliteTest1>()
        .innerJoin<SqliteTest2>().on(sf2.id == sf3.tbi2)
        .innerJoin<SqliteTest3>().on(sf3.tbi1 == sf1.id)
        .build();

    auto select = dao::_select<SqliteTest1>()
        .column(sf1.name, sf1.number)
        .filter(sf1.name == "client")
        .unionSelect(join, true)
        .build().list();

    QVariantList results;
    for (const auto& r : select) {
        results << r.getName() << r.getNumber();
    }
    QCOMPARE(
        results,
        QVariantList() << "client" << 14
            << "client" << 12
            << "bob" << 9
            << "client" << 9999
            << "client" << 9999
            << "abc" << 10
            << "abc" << 50
    );
}

void JoinTest::testJoinUnionSelect() {
    SqliteTest1::Fields sf1;
    SqliteTest2::Fields sf2;
    SqliteTest3::Fields sf3;

    auto select = dao::_select<SqliteTest1>()
        .column(sf1.name, sf1.number)
        .filter(sf1.name == "client")
        .with(_orderBy(sf1.name, sf1.number))
        .build();

    auto join = dao::_join<SqliteTest1, SqliteTest2, SqliteTest3>()
        .column(sf1.name, sf2.number)
        .from<SqliteTest1>()
        .innerJoin<SqliteTest2>().on(sf2.id == sf3.tbi2)
        .innerJoin<SqliteTest3>().on(sf3.tbi1 == sf1.id)
        .unionSelect(select)
        .build().list();

    QVariantList results;
    for (const auto& r : join) {
        auto s1 = std::get<0>(r);
        auto s2 = std::get<1>(r);
        auto s3 = std::get<2>(r);
        results << s1.getName() << s2.getNumber();
    }
    QCOMPARE(
        results,
        QVariantList()
        << "abc" << 10
        << "abc" << 50
        << "bob" << 9
        << "client" << 12
        << "client" << 14
        << "client" << 9999
    );
}

void JoinTest::testJoinUnionJoin() {
    SqliteTest1::Fields sf1;
    SqliteTest2::Fields sf2;
    SqliteTest3::Fields sf3;

    auto join1 = dao::_join<SqliteTest1, SqliteTest2, SqliteTest3>()
        .column(sf1.name, sf2.number)
        .from<SqliteTest1>()
        .filter(sf1.name == "client")
        .with(_orderBy(sf1.name, sf2.number))
        .innerJoin<SqliteTest2>().on(sf2.id == sf3.tbi2)
        .innerJoin<SqliteTest3>().on(sf3.tbi1 == sf1.id)
        .build();

    auto join2 = dao::_join<SqliteTest1, SqliteTest2, SqliteTest3>()
        .column(sf1.name, sf2.number)
        .from<SqliteTest1>()
        .filter(sf1.number >= 12)
        .innerJoin<SqliteTest2>().on(sf2.id == sf3.tbi2)
        .innerJoin<SqliteTest3>().on(sf3.tbi1 == sf1.id)
        .unionSelect(join1)
        .build().list();

    QVariantList results;
    for (const auto& r : join2) {
        auto s1 = std::get<0>(r);
        auto s2 = std::get<1>(r);
        auto s3 = std::get<2>(r);
        results << s1.getName() << s2.getNumber();
    }
    QCOMPARE(
        results,
        QVariantList()
        << "bob" << 9
        << "client" << 9999
    );
}

void JoinTest::recursiveQueryTest() {
    class TmpTest2 : public dao::self<SqliteTest2> {};

    SqliteTest1::Fields sf1;
    SqliteTest2::Fields sf2;
    SqliteTest3::Fields sf3;
    TmpTest2::Fields sfs2;

    auto init = dao::_select<SqliteTest2>().filter(sf2.number == 50).build();
    auto recur = dao::_join<SqliteTest2, TmpTest2>()
            .columnAll<SqliteTest2>()
            .from<SqliteTest2>()
            .innerJoin<TmpTest2>().on(sfs2.number2 == sf2.number)
            .build();
    auto recursive = dao::_recursive()
        .tmp<TmpTest2>()
        .initialSelect(init)
        .recursiveSelect(recur);

    auto select = dao::_select<TmpTest2>().from(recursive)
        .column(sfs2.number).with(_orderBy(sfs2.number)).build().list();
    QVariantList numbers;
    for (const auto& r : select) {
        numbers << r.getNumber();
    }
    QCOMPARE(numbers, QVariantList() << 9 << 10 << 50 << 9999);

    auto join1 = dao::_join<TmpTest2, SqliteTest1, SqliteTest3>()
        .column(sfs2.name, sf1.name, sf3.name, sf1.number, sfs2.number)
        .from(recursive)
        .innerJoin<SqliteTest1>().on(sf1.id == sf3.tbi1)
        .innerJoin<SqliteTest3>().on(sf3.tbi2 == sfs2.id, sf3.name.like("client%"))
        .build().list();

    QVariantList data;
    for (const auto& r : join1) {
        auto s2 = std::get<0>(r);
        auto s1 = std::get<1>(r);
        auto s3 = std::get<2>(r);
        data << s2.getName() << s1.getName() << s3.getName() << s1.getNumber() << s2.getNumber();
    }
    QCOMPARE(
        data,
        QVariantList()
        << "joker" << "client" << "client group1" << 14 << 9999
        << "joker" << "client" << "client group2" << 12 << 9999
    );

    auto join2 = dao::_join<SqliteTest1, TmpTest2, SqliteTest3>()
        .column(sfs2.name, sf1.name, sf3.name, sf1.number, sfs2.number)
        .from<SqliteTest1>()
        .innerJoin(recursive).on(sfs2.id == sf3.tbi2)
        .innerJoin<SqliteTest3>().on(sf3.tbi1 == sf1.id, sf3.name.like("client%"))
        .build().list();

    data.clear();
    for (const auto& r : join2) {
        auto s1 = std::get<0>(r);
        auto s2 = std::get<1>(r);
        auto s3 = std::get<2>(r);
        data << s2.getName() << s1.getName() << s3.getName() << s1.getNumber() << s2.getNumber();
    }
    QCOMPARE(
        data,
        QVariantList()
        << "joker" << "client" << "client group1" << 14 << 9999
        << "joker" << "client" << "client group2" << 12 << 9999
    );
}

void JoinTest::functionSubJoinTest() {
    SqliteTest1::Fields sf1;
    SqliteTest2::Fields sf2;
    SqliteTest3::Fields sf3;

    auto select = dao::_select<SqliteTest2>().filter(sf2.name == "joker").build();

    auto join = dao::_join<SqliteTest1, SqliteTest2, SqliteTest3>()
        .column(sf1.number)
        .from<SqliteTest1>()
        .innerJoin(select).on(sf2.id == sf3.tbi2)
        .innerJoin<SqliteTest3>().on(sf3.tbi1 == sf1.id)
        .build();

    auto result = dao::_select<SqliteTest1>()
        .filter(_fun("%1 in %2").field(sf1.number).from(join))
        .build().list();

    QVariantList actual;
    for (const auto& d : result) {
        actual << SqliteTest1::Tool::getValueWithoutAutoIncrement(d);
    }
    QVariantList expected;
    for (const auto& d : data1) {
        if (d.getNumber() != 12 && d.getNumber() != 14)
            continue;
        expected << SqliteTest1::Tool::getValueWithoutAutoIncrement(d);
    }
    QCOMPARE(actual, expected);
}

void JoinTest::cleanup() {
    clearCacheAndPrintIfTestFail();
}

void JoinTest::cleanupTestCase() {
    ConnectionPool::release();
    DbLoader::getClient().dropDatabase();
}

