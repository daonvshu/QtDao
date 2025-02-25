﻿#include "JoinTest.h"

#include <QtTest>

#include "dao.h"
#include "dbexception.h"

void JoinTest::initTestCase() {
    configDb();

    if (targetDb == TestTargetDb::Target_Sqlite) {
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
    } else if (targetDb == TestTargetDb::Target_Mysql) {
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
    } else if (targetDb == TestTargetDb::Target_SqlServer) {
        sqlserverData1 << SqlServerTest1(1, "abc", 10, "");
        sqlserverData1 << SqlServerTest1(2, "alice", 11, "alice1");
        sqlserverData1 << SqlServerTest1(3, "bob", 12, "bob boom");
        sqlserverData1 << SqlServerTest1(4, "client", 14, "1");
        sqlserverData1 << SqlServerTest1(5, "client", 12, "xxx");
        dao::_insert<SqlServerTest1>().build().insert2(sqlserverData1);

        sqlserverData2 << SqlServerTest2("joker", 9999, -1);
        sqlserverData2 << SqlServerTest2("bob", 9, 9999);
        sqlserverData2 << SqlServerTest2("func", 10, 9);
        sqlserverData2 << SqlServerTest2("func", 50, 10);
        dao::_insert<SqlServerTest2>().build().insert2(sqlserverData2);

        sqlserverData3 << SqlServerTest3(3, 2, "bob group", 2);
        sqlserverData3 << SqlServerTest3(4, 1, "client group1", 3);
        sqlserverData3 << SqlServerTest3(5, 1, "client group2", 3);
        sqlserverData3 << SqlServerTest3(1, 3, "func group1", 6);
        sqlserverData3 << SqlServerTest3(1, 4, "func group2", 7);
        dao::_insert<SqlServerTest3>().build().insert2(sqlserverData3);
    } else if (targetDb == TestTargetDb::Target_PSql) {
        psqlData1 << PSqlTest1(1, "abc", 10, "");
        psqlData1 << PSqlTest1(2, "alice", 11, "alice1");
        psqlData1 << PSqlTest1(3, "bob", 12, "bob boom");
        psqlData1 << PSqlTest1(4, "client", 14, "1");
        psqlData1 << PSqlTest1(5, "client", 12, "xxx");
        dao::_insert<PSqlTest1>().build().insert2(psqlData1);

        psqlData2 << PSqlTest2("joker", 9999, -1);
        psqlData2 << PSqlTest2("bob", 9, 9999);
        psqlData2 << PSqlTest2("func", 10, 9);
        psqlData2 << PSqlTest2("func", 50, 10);
        dao::_insert<PSqlTest2>().build().insert2(psqlData2);

        psqlData3 << PSqlTest3(3, 2, "bob group", 2);
        psqlData3 << PSqlTest3(4, 1, "client group1", 3);
        psqlData3 << PSqlTest3(5, 1, "client group2", 3);
        psqlData3 << PSqlTest3(1, 3, "func group1", 6);
        psqlData3 << PSqlTest3(1, 4, "func group2", 7);
        dao::_insert<PSqlTest3>().build().insert2(psqlData3);
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
        data << s2.name << s1.name << s3.name << s1.number << s2.number;
    }
    QCOMPARE(
        data,
        QVariantList() << "bob" << "bob" << "bob group" << 12 << 9
            << "joker" << "client" << "client group1" << 14 << 9999
            << "joker" << "client" << "client group2" << 12 << 9999
            <<"func" << "abc" << "func group1" << 10 << 10
            <<"func" << "abc" << "func group2" << 10 << 50
    );

    auto v1 = dao::_join<E1, E3, E2>()
            .column(sf2.name, sf1.name, sf3.name, sf1.number, sf2.number)
            .template from<E1>()
            .template innerJoin<E3>().on(sf3.tbi1 == sf1.id)
            .template innerJoin<E2>().on(sf2.id == sf3.tbi2)
            .build().list(sf3.name);

    QStringList expectV1 = {"bob group", "client group1", "client group2", "func group1", "func group2"};
    QCOMPARE(v1, expectV1);

    auto v2 = dao::_join<E1, E3, E2>()
            .column(sf2.name, sf1.name, sf3.name, sf2.number)
            .template from<E1>()
            .template innerJoin<E3>().on(sf3.tbi1 == sf1.id)
            .template innerJoin<E2>().on(sf2.id == sf3.tbi2)
            .filter(sf1.number > 10)
            .build().list(sf1.name, sf2.number);

    QList<QPair<QString, int>> expectV2;
    expectV2 << qMakePair(QString("bob"), 9)
             << qMakePair(QString("client"), 9999)
             << qMakePair(QString("client"), 9999);
    QCOMPARE(v2, expectV2);

    auto v3 = dao::_join<E1, E3, E2>()
            .column(sf2.name, sf1.name, sf3.name, sf2.number)
            .template from<E1>()
            .template innerJoin<E3>().on(sf3.tbi1 == sf1.id)
            .template innerJoin<E2>().on(sf2.id == sf3.tbi2)
            .filter(sf1.number > 10)
            .build().list(sf2.name, sf1.name, sf3.name, sf2.number);

    auto expectV3 = QVariantList() << "bob" << "bob" << "bob group" << 9
                   << "joker" << "client" << "client group1" << 9999
                   << "joker" << "client" << "client group2" << 9999;
    QVariantList dataV3;
    for (const auto& d : v3) {
        dataV3 << std::get<0>(d) << std::get<1>(d) << std::get<2>(d) << std::get<3>(d);
    }
    QCOMPARE(dataV3, expectV3);
}

void JoinTest::testJoinTable() {
    if (targetDb == TestTargetDb::Target_Sqlite) {
        runTestJoinTable<SqliteTest1, SqliteTest2, SqliteTest3>();
    } else if (targetDb == TestTargetDb::Target_Mysql) {
        runTestJoinTable<MysqlTest1, MysqlTest2, MysqlTest3>();
    } else if (targetDb == TestTargetDb::Target_SqlServer) {
        runTestJoinTable<SqlServerTest1, SqlServerTest2, SqlServerTest3>();
    } else if (targetDb == TestTargetDb::Target_PSql) {
        runTestJoinTable<PSqlTest1, PSqlTest2, PSqlTest3>();
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
        data << std::get<2>(r).name;
        data << std::get<0>(r).template __getExtra<int>("sumn");
    }
    QCOMPARE(
        data,
        QVariantList() << "bob" << 12 << "func" << 20 << "joker" << 26
    );
}

void JoinTest::testJoinTableUseWith() {
    if (targetDb == TestTargetDb::Target_Sqlite) {
        runTestJoinTableUseWith<SqliteTest1, SqliteTest2, SqliteTest3>();
    } else if (targetDb == TestTargetDb::Target_Mysql) {
        runTestJoinTableUseWith<MysqlTest1, MysqlTest2, MysqlTest3>();
    } else if (targetDb == TestTargetDb::Target_SqlServer) {
        runTestJoinTableUseWith<SqlServerTest1, SqlServerTest2, SqlServerTest3>();
    } else if (targetDb == TestTargetDb::Target_PSql) {
        runTestJoinTableUseWith<PSqlTest1, PSqlTest2, PSqlTest3>();
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
        data << e1.name << e1.number << e1.number2;
        auto e2 = std::get<1>(r);
        data << e2.tbi1 << e2.tbi2 << e2.name << e2.size;
    }
    QCOMPARE(
        data,
        QVariantList() << 1 << "abc" << 10 << QByteArray("") << "func" << 10 << 9 << 1 << 3 << "func group1" << 6
    );
}

void JoinTest::testJoinTableFilterOn() {
    if (targetDb == TestTargetDb::Target_Sqlite) {
        runTestJoinTableFilterOn<SqliteTest1, SqliteTest2, SqliteTest3>();
    } else if (targetDb == TestTargetDb::Target_Mysql) {
        runTestJoinTableFilterOn<MysqlTest1, MysqlTest2, MysqlTest3>();
    } else if (targetDb == TestTargetDb::Target_SqlServer) {
        runTestJoinTableFilterOn<SqlServerTest1, SqlServerTest2, SqlServerTest3>();
    } else if (targetDb == TestTargetDb::Target_PSql) {
        runTestJoinTableFilterOn<PSqlTest1, PSqlTest2, PSqlTest3>();
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
        data << std::get<0>(r).name;
        data << std::get<1>(r).name;
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
        data << std::get<0>(r).name;
        data << std::get<1>(r).name;
        data << std::get<2>(r).name;
    }
    QCOMPARE(
        data,
        QVariantList() << "joker" << "bob" << "func" << "bob" << "func" << "func"
    );
}

void JoinTest::testJoinSelfTable() {
    if (targetDb == TestTargetDb::Target_Sqlite) {
        runTestJoinSelfTable<SqliteTest2>();
    } else if (targetDb == TestTargetDb::Target_Mysql) {
        runTestJoinSelfTable<MysqlTest2>();
    } else if (targetDb == TestTargetDb::Target_SqlServer) {
        runTestJoinSelfTable<SqlServerTest2>();
    } else if (targetDb == TestTargetDb::Target_PSql) {
        runTestJoinSelfTable<PSqlTest2>();
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
        QFAIL(("test join in select fail: " + e.reason).toLocal8Bit());
    }
}

void JoinTest::testSelectFromJoin() {
    if (targetDb == TestTargetDb::Target_Sqlite) {
        runTestSelectFromJoin<SqliteTest1, SqliteTest2, SqliteTest3>();
    } else if (targetDb == TestTargetDb::Target_Mysql) {
        runTestSelectFromJoin<MysqlTest1, MysqlTest2, MysqlTest3>();
    } else if (targetDb == TestTargetDb::Target_SqlServer) {
        runTestSelectFromJoin<SqlServerTest1, SqlServerTest2, SqlServerTest3>();
    } else if (targetDb == TestTargetDb::Target_PSql) {
        runTestSelectFromJoin<PSqlTest1, PSqlTest2, PSqlTest3>();
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
        data << s2.name << s1.name << s3.name << s1.number << s2.number;
    }
    QCOMPARE(
        data,
        QVariantList()
        << "joker" << "client" << "client group1" << 14 << 9999
        << "joker" << "client" << "client group2" << 12 << 9999
    );
}

void JoinTest::testJoinFromSelect() {
    if (targetDb == TestTargetDb::Target_Sqlite) {
        runTestJoinFromSelect<SqliteTest1, SqliteTest2, SqliteTest3>();
    } else if (targetDb == TestTargetDb::Target_Mysql) {
        runTestJoinFromSelect<MysqlTest1, MysqlTest2, MysqlTest3>();
    } else if (targetDb == TestTargetDb::Target_SqlServer) {
        runTestJoinFromSelect<SqlServerTest1, SqlServerTest2, SqlServerTest3>();
    } else if (targetDb == TestTargetDb::Target_PSql) {
        runTestJoinFromSelect<PSqlTest1, PSqlTest2, PSqlTest3>();
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
        data << s2.name << s1.name << s3.name << s1.number << s2.number;
    }
    QCOMPARE(
        data,
        QVariantList()
        << "joker" << "client" << "client group1" << 14 << 9999
        << "joker" << "client" << "client group2" << 12 << 9999
    );
}

void JoinTest::testJoinOnSelect() {
    if (targetDb == TestTargetDb::Target_Sqlite) {
        runTestJoinOnSelect<SqliteTest1, SqliteTest2, SqliteTest3>();
    } else if (targetDb == TestTargetDb::Target_Mysql) {
        runTestJoinOnSelect<MysqlTest1, MysqlTest2, MysqlTest3>();
    } else if (targetDb == TestTargetDb::Target_SqlServer) {
        runTestJoinOnSelect<SqlServerTest1, SqlServerTest2, SqlServerTest3>();
    } else if (targetDb == TestTargetDb::Target_PSql) {
        runTestJoinOnSelect<PSqlTest1, PSqlTest2, PSqlTest3>();
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
        results << r.name << r.number;
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
    if (targetDb == TestTargetDb::Target_Sqlite) {
        runTestSelectUnionJoin<SqliteTest1, SqliteTest2, SqliteTest3>();
    } else if (targetDb == TestTargetDb::Target_Mysql) {
        runTestSelectUnionJoin<MysqlTest1, MysqlTest2, MysqlTest3>();
    } else if (targetDb == TestTargetDb::Target_SqlServer) {
#if QT_VERSION_MAJOR < 6
        runTestSelectUnionJoin<SqlServerTest1, SqlServerTest2, SqlServerTest3>();
#else
        qDebug() << "ignore union test in qt6";
#endif
    } else if (targetDb == TestTargetDb::Target_PSql) {
        runTestSelectUnionJoin<PSqlTest1, PSqlTest2, PSqlTest3>();
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
        results << s1.name << s2.number;
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
    if (targetDb == TestTargetDb::Target_Sqlite) {
        runTestJoinUnionSelect<SqliteTest1, SqliteTest2, SqliteTest3>();
    } else if (targetDb == TestTargetDb::Target_Mysql) {
        runTestJoinUnionSelect<MysqlTest1, MysqlTest2, MysqlTest3>();
    } else if (targetDb == TestTargetDb::Target_SqlServer) {
#if QT_VERSION_MAJOR < 6
        runTestJoinUnionSelect<SqlServerTest1, SqlServerTest2, SqlServerTest3>();
#else
        qDebug() << "ignore union test in qt6";
#endif
    } else if (targetDb == TestTargetDb::Target_PSql) {
        runTestJoinUnionSelect<PSqlTest1, PSqlTest2, PSqlTest3>();
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
        results << s1.name << s2.number;
    }
    QCOMPARE(
        results,
        QVariantList()
        << "client" << 9999
        << "bob" << 9
    );
}

void JoinTest::testJoinUnionJoin() {
    if (targetDb == TestTargetDb::Target_Sqlite) {
        runTestJoinUnionJoin<SqliteTest1, SqliteTest2, SqliteTest3>();
    } else if (targetDb == TestTargetDb::Target_Mysql) {
        runTestJoinUnionJoin<MysqlTest1, MysqlTest2, MysqlTest3>();
    } else if (targetDb == TestTargetDb::Target_SqlServer) {
#if QT_VERSION_MAJOR < 6
        runTestJoinUnionJoin<SqlServerTest1, SqlServerTest2, SqlServerTest3>();
#else
        qDebug() << "ignore union test in qt6";
#endif
    } else if (targetDb == TestTargetDb::Target_PSql) {
        runTestJoinUnionJoin<PSqlTest1, PSqlTest2, PSqlTest3>();
    }
}

template<typename E1, typename E2, typename E3>
void runRecursiveQueryTest() {
    class TmpTest2 : public dao::self<E2> {};

    typename E1::Fields sf1;
    typename E2::Fields sf2;
    typename E3::Fields sf3;
    typename TmpTest2::Fields sfs2;

    auto init = dao::_select<E2>().filter(sf2.number == 50).build();
    auto recur = dao::_join<E2, TmpTest2>()
        .template columnAll<E2>()
        .template from<E2>()
        .template innerJoin<TmpTest2>().on(sf2.number == sfs2.number2)
        .build();
    auto recursive = dao::_recursive(true)
        .tmp<TmpTest2>()
        .initialSelect(init)
        .recursiveSelect(recur);

    auto select = dao::_select<TmpTest2>().from(recursive)
        .column(sfs2.number).with(_orderBy(sfs2.number)).build().list();
    QVariantList numbers;
    for (const auto& r : select) {
        numbers << r.number;
    }
    QCOMPARE(numbers, QVariantList() << 9 << 10 << 50 << 9999);

    auto join1 = dao::_join<TmpTest2, E3, E1>()
        .column(sfs2.name, sf1.name, sf3.name, sf1.number, sfs2.number)
        .from(recursive)
        .template innerJoin<E3>().on(sf3.tbi2 == sfs2.id, sf3.name.like("client%"))
        .template innerJoin<E1>().on(sf1.id == sf3.tbi1)
        .build().list();

    QVariantList data;
    for (const auto& r : join1) {
        auto s2 = std::get<0>(r);
        auto s3 = std::get<1>(r);
        auto s1 = std::get<2>(r);
        data << s2.name << s1.name << s3.name << s1.number << s2.number;
    }
    QCOMPARE(
        data,
        QVariantList()
        << "joker" << "client" << "client group1" << 14 << 9999
        << "joker" << "client" << "client group2" << 12 << 9999
    );

    auto join2 = dao::_join<E1, E3, TmpTest2>()
        .column(sfs2.name, sf1.name, sf3.name, sf1.number, sfs2.number)
        .template from<E1>()
        .template innerJoin<E3>().on(sf3.tbi1 == sf1.id, sf3.name.like("client%"))
        .innerJoin(recursive).on(sfs2.id == sf3.tbi2)
        .build().list();

    data.clear();
    for (const auto& r : join2) {
        auto s1 = std::get<0>(r);
        auto s3 = std::get<1>(r);
        auto s2 = std::get<2>(r);
        data << s2.name << s1.name << s3.name << s1.number << s2.number;
    }
    QCOMPARE(
        data,
        QVariantList()
        << "joker" << "client" << "client group1" << 14 << 9999
        << "joker" << "client" << "client group2" << 12 << 9999
    );
}

void JoinTest::recursiveQueryTest() {
    PASSMYSQL;
    if (targetDb == TestTargetDb::Target_Sqlite) {
        runRecursiveQueryTest<SqliteTest1, SqliteTest2, SqliteTest3>();
    } else if (targetDb == TestTargetDb::Target_SqlServer) {
        runRecursiveQueryTest<SqlServerTest1, SqlServerTest2, SqlServerTest3>();
    } else if (targetDb == TestTargetDb::Target_PSql) {
        runRecursiveQueryTest<PSqlTest1, PSqlTest2, PSqlTest3>();
    }
}

void JoinTest::functionSubJoinTest_data() {
    if (targetDb == TestTargetDb::Target_Sqlite) {
        QTest::addColumn<SqliteTest1List>("data1");
        QTest::newRow("sqlite test data") << sqliteData1;
    } else if (targetDb == TestTargetDb::Target_Mysql) {
        QTest::addColumn<MysqlTest1List>("data1");
        QTest::newRow("mysql test data") << mysqlData1;
    } else if (targetDb == TestTargetDb::Target_SqlServer) {
        QTest::addColumn<SqlServerTest1List>("data1");
        QTest::newRow("sqlserver test data") << sqlserverData1;
    } else if (targetDb == TestTargetDb::Target_PSql) {
        QTest::addColumn<PSqlTest1List>("data1");
        QTest::newRow("psql test data") << psqlData1;
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
        if (d.number != 12 && d.number != 14)
            continue;
        expected << E1::Tool::getValueWithoutAutoIncrement(d);
    }
    QCOMPARE(actual, expected);
}

void JoinTest::functionSubJoinTest() {
    if (targetDb == TestTargetDb::Target_Sqlite) {
        runFunctionSubJoinTest<SqliteTest1, SqliteTest2, SqliteTest3>();
    } else if (targetDb == TestTargetDb::Target_Mysql) {
        runFunctionSubJoinTest<MysqlTest1, MysqlTest2, MysqlTest3>();
    } else if (targetDb == TestTargetDb::Target_SqlServer) {
        runFunctionSubJoinTest<SqlServerTest1, SqlServerTest2, SqlServerTest3>();
    } else if (targetDb == TestTargetDb::Target_PSql) {
        runFunctionSubJoinTest<PSqlTest1, PSqlTest2, PSqlTest3>();
    }
}

template<typename E1, typename E2, typename E3>
Join<E1, E3, E2> getExplainPrepareSelect() {
    typename E1::Fields sf1;
    typename E2::Fields sf2;
    typename E3::Fields sf3;

    auto select = dao::_select<E2>().filter(sf2.name == "joker").build();

    auto join = dao::_join<E1, E3, E2>()
        .column(sf1.number)
        .template from<E1>()
        .template innerJoin<E3>().on(sf3.tbi1 == sf1.id)
        .innerJoin(select).on(sf2.id == sf3.tbi2)
        .filter(sf3.size > 0)
        .build();

    return join;
}

void JoinTest::explainTest() {
    if (targetDb == TestTargetDb::Target_Sqlite) {
        auto d1 = getExplainPrepareSelect<SqliteTest1, SqliteTest2, SqliteTest3>()
            .explain<SqliteExplainInfo>();
        QVERIFY(!d1.isEmpty());

        auto d2 = getExplainPrepareSelect<SqliteTest1, SqliteTest2, SqliteTest3>()
            .explain<SqliteExplainQueryPlanInfo>();
        QVERIFY(!d2.isEmpty());
    } else if (targetDb == TestTargetDb::Target_Mysql) {
        auto d = getExplainPrepareSelect<MysqlTest1, MysqlTest2, MysqlTest3>()
            .explain<MysqlExplainInfo>();
        QVERIFY(!d.isEmpty());
    } else if (targetDb == TestTargetDb::Target_SqlServer) {
        auto d = getExplainPrepareSelect<SqlServerTest1, SqlServerTest2, SqlServerTest3>()
            .explain<SqlServerExplainInfo>();
        QVERIFY(!d.isEmpty());
    } else if (targetDb == TestTargetDb::Target_PSql) {
        auto d = getExplainPrepareSelect<PSqlTest1, PSqlTest2, PSqlTest3>()
            .explain<PSqlExplainInfo>();
        QVERIFY(!d.isEmpty());
    }
}

void JoinTest::cleanup() {
    clearCacheAndPrintIfTestFail();
}

void JoinTest::cleanupTestCase() {
    ConfigManager::getConfig()->getClient()->dropDatabase();
}

