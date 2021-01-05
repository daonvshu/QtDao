#include "JoinTest.h"

#include <QtTest>

#include "../sqliteentity/SqliteTest1.h"
#include "../sqliteentity/SqliteTest2.h"

#include "../../src/dao.h"

void JoinTest::initTestCase() {
    configDb();

    if (engineModel == Engine_Sqlite) {
        sqliteData1 << SqliteTest1(1, "abc", 10, "");
        sqliteData1 << SqliteTest1(2, "alice", 11, "alice1");
        sqliteData1 << SqliteTest1(3, "bob", 12, "bob boom");
        sqliteData1 << SqliteTest1(4, "client", 14, "1");
        sqliteData1 << SqliteTest1(5, "client", 12, "xxx");
        dao::_insert<SqliteTest1>().build().insert2(sqliteData1);

        sqliteData2 << SqliteTest2("joker", 9999, -1, 30);
        sqliteData2 << SqliteTest2("bob", 9, 9999, "abc");
        sqliteData2 << SqliteTest2("func", 10, 9, 50);
        sqliteData2 << SqliteTest2("func", 50, 10, 50);
        dao::_insert<SqliteTest2>().build().insert2(sqliteData2);

        sqliteData3 << SqliteTest3(3, 2, "bob group", 2);
        sqliteData3 << SqliteTest3(4, 1, "client group1", 3);
        sqliteData3 << SqliteTest3(5, 1, "client group2", 3);
        sqliteData3 << SqliteTest3(1, 3, "func group1", 6);
        sqliteData3 << SqliteTest3(1, 4, "func group2", 7);
        dao::_insert<SqliteTest3>().build().insert2(sqliteData3);
    } else if (engineModel == Engine_Mysql) {
        mysqlData1 << MysqlTest1(1, "abc", 10, "");
        mysqlData1 << MysqlTest1(2, "alice", 11, "alice1");
        mysqlData1 << MysqlTest1(3, "bob", 12, "bob boom");
        mysqlData1 << MysqlTest1(4, "client", 14, "1");
        mysqlData1 << MysqlTest1(5, "client", 12, "xxx");
        dao::_insert<MysqlTest1>().build().insert2(mysqlData1);

        mysqlData2 << MysqlTest2("joker", 9999, -1);
        mysqlData2 << MysqlTest2("bob", 9, 9999);
        mysqlData2 << MysqlTest2("func", 10, 9);
        mysqlData2 << MysqlTest2("func", 50, 10);
        dao::_insert<MysqlTest2>().build().insert2(mysqlData2);

        mysqlData3 << MysqlTest3(3, 2, "bob group", 2);
        mysqlData3 << MysqlTest3(4, 1, "client group1", 3);
        mysqlData3 << MysqlTest3(5, 1, "client group2", 3);
        mysqlData3 << MysqlTest3(1, 3, "func group1", 6);
        mysqlData3 << MysqlTest3(1, 4, "func group2", 7);
        dao::_insert<MysqlTest3>().build().insert2(mysqlData3);
    }
    clearCacheAndPrintIfTestFail();
}

template<typename E1, typename E2, typename E3>
void runTestJoinTable() {
    typename E1::Fields sf1;
    typename E2::Fields sf2;
    typename E3::Fields sf3;

    auto result = dao::_join<E1, E3, E2>()
        .column(sf2.name, sf1.name, sf3.name, sf1.number, sf2.number)
        .template from<E1>()
        .template innerJoin<E3>().on(sf3.tbi1 == sf1.id)
        .template innerJoin<E2>().on(sf2.id == sf3.tbi2)
        .build().list();
    QVariantList data;
    for (const auto& r : result) {
        auto s1 = std::get<0>(r);
        auto s3 = std::get<1>(r);
        auto s2 = std::get<2>(r);
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
}

void JoinTest::testJoinTable() {
    if (engineModel == Engine_Sqlite) {
        runTestJoinTable<SqliteTest1, SqliteTest2, SqliteTest3>();
    } else if (engineModel == Engine_Mysql) {
        runTestJoinTable<MysqlTest1, MysqlTest2, MysqlTest3>();
    }
}

template<typename E1, typename E2, typename E3>
void runTestJoinTableUseWith() {
    typename E1::Fields sf1;
    typename E2::Fields sf2;
    typename E3::Fields sf3;

    auto result2 = dao::_join<E1, E3, E2>()
        .column(sf2.name, _fun("sum(%1) as sumn").field(sf1.number))
        .template from<E1>()
        .template innerJoin<E3>().on(sf3.tbi1 == sf1.id)
        .template innerJoin<E2>().on(sf2.id == sf3.tbi2)
        .with(_groupBy(sf2.name), _orderBy(sf2.name))
        .build().list();
    QVariantList data;
    for (const auto& r : result2) {
        data << std::get<2>(r).getName();
        data << std::get<0>(r).__getExtra("sumn");
    }
    QCOMPARE(
        data,
        QVariantList() << "bob" << 12 << "func" << 20 << "joker" << 26
    );
}

void JoinTest::testJoinTableUseWith() {
    if (engineModel == Engine_Sqlite) {
        runTestJoinTableUseWith<SqliteTest1, SqliteTest2, SqliteTest3>();
    } else if (engineModel == Engine_Mysql) {
        runTestJoinTableUseWith<MysqlTest1, MysqlTest2, MysqlTest3>();
    }
}

template<typename E1, typename E2, typename E3>
void runTestJoinTableFilterOn() {
    typename E1::Fields sf1;
    typename E2::Fields sf2;
    typename E3::Fields sf3;

    auto result3 = dao::_join<E1, E3, E2>()
        .template from<E1>()
        .template innerJoin<E3>().on(sf3.tbi1 == sf1.id, sf3.name == "func group1")
        .template innerJoin<E2>().on(sf2.id == sf3.tbi2)
        .build().list();
    QVariantList data;
    for (const auto& r : result3) {
        data << E1::Tool::getValueWithoutAutoIncrement(std::get<0>(r));
        auto e1 = std::get<2>(r);
        data << e1.getName() << e1.getNumber() << e1.getNumber2();
        auto e2 = std::get<1>(r);
        data << e2.getTbi1() << e2.getTbi2() << e2.getName() << e2.getSize();
    }
    QCOMPARE(
        data,
        QVariantList() << 1 << "abc" << 10 << "" << "func" << 10 << 9 << 1 << 3 << "func group1" << 6
    );
}

void JoinTest::testJoinTableFilterOn() {
    if (engineModel == Engine_Sqlite) {
        runTestJoinTableFilterOn<SqliteTest1, SqliteTest2, SqliteTest3>();
    } else if (engineModel == Engine_Mysql) {
        runTestJoinTableFilterOn<MysqlTest1, MysqlTest2, MysqlTest3>();
    }
}

template<typename E>
void runTestJoinSelfTable() {
    class E2 : public dao::self<E> {};
    typename E::Fields sf1;
    typename E2::Fields sf2;
    auto result = dao::_join<E, E2>()
        .column(sf1.name, sf2.name)
        .template from<E>()
        .template innerJoin<E2>().on(sf2.number2 == sf1.number)
        .with(_orderBy(sf1.name))
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

    class E3 : public dao::self<E2> {};
    typename E3::Fields sf3;
    auto result2 = dao::_join<E, E2, E3>()
        .column(sf1.name, sf2.name, sf3.name)
        .template from<E>()
        .with(_orderBy(sf1.name.desc()))
        .template innerJoin<E2>().on(sf2.number2 == sf1.number)
        .template innerJoin<E3>().on(sf3.number2 == sf2.number)
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

void JoinTest::testJoinSelfTable() {
    if (engineModel == Engine_Sqlite) {
        runTestJoinSelfTable<SqliteTest2>();
    } else if (engineModel == Engine_Mysql) {
        runTestJoinSelfTable<MysqlTest2>();
    }
}

template<typename E1, typename E2, typename E3>
void runTestSelectFromJoin() {
    typename E1::Fields sf1;
    typename E2::Fields sf2;
    typename E3::Fields sf3;

    auto join1 = dao::_join<E1, E3, E2>()
        .column(sf3.name, sf1.number)
        .template from<E1>()
        .template innerJoin<E3>().on(sf3.tbi1 == sf1.id)
        .template innerJoin<E2>().on(sf2.id == sf3.tbi2)
        .build();

    try {
        auto count1 = dao::_count<E1>()
            .from(join1)
            .filter(sf1.number > 10)
            .count();
        QCOMPARE(count1, 3);
    }
    catch (DaoException& e) {
        QFAIL(("test join in select fail: " + e.reason).toUtf8());
    }
}

void JoinTest::testSelectFromJoin() {
    if (engineModel == Engine_Sqlite) {
        runTestSelectFromJoin<SqliteTest1, SqliteTest2, SqliteTest3>();
    } else if (engineModel == Engine_Mysql) {
        runTestSelectFromJoin<MysqlTest1, MysqlTest2, MysqlTest3>();
    }
}

template<typename E1, typename E2, typename E3>
void runTestJoinFromSelect() {
    typename E1::Fields sf1;
    typename E2::Fields sf2;
    typename E3::Fields sf3;

    auto select = dao::_select<E1>().filter(sf1.name == "client").build();

    auto join = dao::_join<E1, E3, E2>()
        .column(sf2.name, sf1.name, sf3.name, sf1.number, sf2.number)
        .from(select)
        .template innerJoin<E3>().on(sf3.tbi1 == sf1.id)
        .template innerJoin<E2>().on(sf2.id == sf3.tbi2)
        .build().list();

    QVariantList data;
    for (const auto& r : join) {
        auto s1 = std::get<0>(r);
        auto s2 = std::get<2>(r);
        auto s3 = std::get<1>(r);
        data << s2.getName() << s1.getName() << s3.getName() << s1.getNumber() << s2.getNumber();
    }
    QCOMPARE(
        data,
        QVariantList()
        << "joker" << "client" << "client group1" << 14 << 9999
        << "joker" << "client" << "client group2" << 12 << 9999
    );
}

void JoinTest::testJoinFromSelect() {
    if (engineModel == Engine_Sqlite) {
        runTestJoinFromSelect<SqliteTest1, SqliteTest2, SqliteTest3>();
    } else if (engineModel == Engine_Mysql) {
        runTestJoinFromSelect<MysqlTest1, MysqlTest2, MysqlTest3>();
    }
}

template<typename E1, typename E2, typename E3>
void runTestJoinOnSelect() {
    typename E1::Fields sf1;
    typename E2::Fields sf2;
    typename E3::Fields sf3;

    auto select = dao::_select<E2>().filter(sf2.name == "joker").build();

    auto join = dao::_join<E1, E3, E2>()
        .column(sf2.name, sf1.name, sf3.name, sf1.number, sf2.number)
        .template from<E1>()
        .template innerJoin<E3>().on(sf3.tbi1 == sf1.id)
        .innerJoin(select).on(sf2.id == sf3.tbi2)
        .build().list();

    QVariantList data;
    for (const auto& r : join) {
        auto s1 = std::get<0>(r);
        auto s2 = std::get<2>(r);
        auto s3 = std::get<1>(r);
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
    if (engineModel == Engine_Sqlite) {
        runTestJoinOnSelect<SqliteTest1, SqliteTest2, SqliteTest3>();
    } else if (engineModel == Engine_Mysql) {
        runTestJoinOnSelect<MysqlTest1, MysqlTest2, MysqlTest3>();
    }
}

template<typename E1, typename E2, typename E3>
void runTestSelectUnionJoin() {
    typename E1::Fields sf1;
    typename E2::Fields sf2;
    typename E3::Fields sf3;

    auto join = dao::_join<E1, E3, E2>()
        .column(sf1.name, sf2.number)
        .template from<E1>()
        .template innerJoin<E3>().on(sf3.tbi1 == sf1.id)
        .template innerJoin<E2>().on(sf2.id == sf3.tbi2)
        .build();

    auto select = dao::_select<E1>()
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

void JoinTest::testSelectUnionJoin() {
    if (engineModel == Engine_Sqlite) {
        runTestSelectUnionJoin<SqliteTest1, SqliteTest2, SqliteTest3>();
    } else if (engineModel == Engine_Mysql) {
        runTestSelectUnionJoin<MysqlTest1, MysqlTest2, MysqlTest3>();
    }
}

template<typename E1, typename E2, typename E3>
void runTestJoinUnionSelect() {
    typename E1::Fields sf1;
    typename E2::Fields sf2;
    typename E3::Fields sf3;

    auto select = dao::_select<E1>()
        .column(sf1.name, sf1.number)
        .filter(sf1.name == "client")
        .build();

    auto join = dao::_join<E1, E3, E2>()
        .column(sf1.name, sf2.number)
        .template from<E1>()
        .template innerJoin<E3>().on(sf3.tbi1 == sf1.id)
        .template innerJoin<E2>().on(sf2.id == sf3.tbi2)
        .unionSelect(select)
        .with(_orderBy(sf1.name, sf2.number))
        .build().list();

    QVariantList results;
    for (const auto& r : join) {
        auto s1 = std::get<0>(r);
        auto s2 = std::get<2>(r);
        auto s3 = std::get<1>(r);
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

void JoinTest::testJoinUnionSelect() {
    if (engineModel == Engine_Sqlite) {
        runTestJoinUnionSelect<SqliteTest1, SqliteTest2, SqliteTest3>();
    } else if (engineModel == Engine_Mysql) {
        runTestJoinUnionSelect<MysqlTest1, MysqlTest2, MysqlTest3>();
    }
}

template<typename E1, typename E2, typename E3>
void runTestJoinUnionJoin() {
    typename E1::Fields sf1;
    typename E2::Fields sf2;
    typename E3::Fields sf3;

    auto join1 = dao::_join<E1, E3, E2>()
        .column(sf1.name, sf2.number)
        .template from<E1>()
        .filter(sf1.name == "client")
        .template innerJoin<E3>().on(sf3.tbi1 == sf1.id)
        .template innerJoin<E2>().on(sf2.id == sf3.tbi2)
        .build();

    auto join2 = dao::_join<E1, E3, E2>()
        .column(sf1.name, sf2.number)
        .template from<E1>()
        .filter(sf1.number >= 12)
        .template innerJoin<E3>().on(sf3.tbi1 == sf1.id)
        .template innerJoin<E2>().on(sf2.id == sf3.tbi2)
        .unionSelect(join1)
        .with(_orderBy(sf1.name.desc(), sf2.number))
        .build().list();

    QVariantList results;
    for (const auto& r : join2) {
        auto s1 = std::get<0>(r);
        auto s2 = std::get<2>(r);
        auto s3 = std::get<1>(r);
        results << s1.getName() << s2.getNumber();
    }
    QCOMPARE(
        results,
        QVariantList()
        << "client" << 9999
        << "bob" << 9
    );
}

void JoinTest::testJoinUnionJoin() {
    if (engineModel == Engine_Sqlite) {
        runTestJoinUnionJoin<SqliteTest1, SqliteTest2, SqliteTest3>();
    } else if (engineModel == Engine_Mysql) {
        runTestJoinUnionJoin<MysqlTest1, MysqlTest2, MysqlTest3>();
    }
}

void JoinTest::recursiveQueryTest() {
    PASSMYSQL;
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

void JoinTest::functionSubJoinTest_data() {
    if (engineModel == Engine_Sqlite) {
        QTest::addColumn<SqliteTest1List>("data1");
        QTest::newRow("sqlite test data") << sqliteData1;
    } else if (engineModel == Engine_Mysql) {
        QTest::addColumn<MysqlTest1List>("data1");
        QTest::newRow("mysql test data") << mysqlData1;
    }
}

template<typename E1, typename E2, typename E3>
void runFunctionSubJoinTest() {
    typename E1::Fields sf1;
    typename E2::Fields sf2;
    typename E3::Fields sf3;

    auto select = dao::_select<E2>().filter(sf2.name == "joker").build();

    auto join = dao::_join<E1, E3, E2>()
        .column(sf1.number)
        .template from<E1>()
        .template innerJoin<E3>().on(sf3.tbi1 == sf1.id)
        .innerJoin(select).on(sf2.id == sf3.tbi2)
        .build();

    auto result = dao::_select<E1>()
        .filter(_fun("%1 in %2").field(sf1.number).from(join))
        .build().list();

    QVariantList actual;
    for (const auto& d : result) {
        actual << E1::Tool::getValueWithoutAutoIncrement(d);
    }
    QVariantList expected;
    QFETCH(QList<E1>, data1);
    for (const auto& d : data1) {
        if (d.getNumber() != 12 && d.getNumber() != 14)
            continue;
        expected << E1::Tool::getValueWithoutAutoIncrement(d);
    }
    QCOMPARE(actual, expected);
}

void JoinTest::functionSubJoinTest() {
    if (engineModel == Engine_Sqlite) {
        runFunctionSubJoinTest<SqliteTest1, SqliteTest2, SqliteTest3>();
    } else if (engineModel == Engine_Mysql) {
        runFunctionSubJoinTest<MysqlTest1, MysqlTest2, MysqlTest3>();
    }
}

void JoinTest::cleanup() {
    clearCacheAndPrintIfTestFail();
}

void JoinTest::cleanupTestCase() {
    ConnectionPool::release();
    DbLoader::getClient().dropDatabase();
}

