#include "SelectTest.h"

#include <QtTest>

#include "dao.h"
#include "dbexception.h"

void SelectTest::initTestCase() {
    configDb();

    if (engineModel == Engine_Sqlite) {
        sqliteData1 << SqliteTest1(1, "abc", 10, "");
        sqliteData1 << SqliteTest1(2, "alice", 11, "alice1");
        sqliteData1 << SqliteTest1(3, "bob", 12, "bob boom");
        sqliteData1 << SqliteTest1(4, "client", 14, "1");
        sqliteData1 << SqliteTest1(5, "client", 12, "xxx");
        dao::_insert<SqliteTest1>().build().insert2(sqliteData1);

        sqliteData2 << SqliteTest2("joker", 9999, -1, 30);
        sqliteData2 << SqliteTest2("bob", 12, 0, "abc");
        sqliteData2 << SqliteTest2("func", 10, -2, 50);
        sqliteData2 << SqliteTest2("func", 50, 0, 50);
        dao::_insert<SqliteTest2>().build().insert2(sqliteData2);
    } else if (engineModel == Engine_Mysql) {
        mysqlData1 << MysqlTest1(1, "abc", 10, "");
        mysqlData1 << MysqlTest1(2, "alice", 11, "alice1");
        mysqlData1 << MysqlTest1(3, "bob", 12, "bob boom");
        mysqlData1 << MysqlTest1(4, "client", 14, "1");
        mysqlData1 << MysqlTest1(5, "client", 12, "xxx");
        dao::_insert<MysqlTest1>().build().insert2(mysqlData1);

        mysqlData2 << MysqlTest2("joker", 9999, -1);
        mysqlData2 << MysqlTest2("bob", 12, 0);
        mysqlData2 << MysqlTest2("func", 10, -2);
        mysqlData2 << MysqlTest2("func", 50, 0);
        dao::_insert<MysqlTest2>().build().insert2(mysqlData2);
    } else if (engineModel == Engine_SqlServer) {
        sqlserverData1 << SqlServerTest1(1, "abc", 10, "");
        sqlserverData1 << SqlServerTest1(2, "alice", 11, "alice1");
        sqlserverData1 << SqlServerTest1(3, "bob", 12, "bob boom");
        sqlserverData1 << SqlServerTest1(4, "client", 14, "1");
        sqlserverData1 << SqlServerTest1(5, "client", 12, "xxx");
        dao::_insert<SqlServerTest1>().build().insert2(sqlserverData1);

        sqlserverData2 << SqlServerTest2("joker", 9999, -1);
        sqlserverData2 << SqlServerTest2("bob", 12, 0);
        sqlserverData2 << SqlServerTest2("func", 10, -2);
        sqlserverData2 << SqlServerTest2("func", 50, 0);
        dao::_insert<SqlServerTest2>().build().insert2(sqlserverData2);
    }
}

void SelectTest::uniqueSelectTest_data() {
    if (engineModel == Engine_Sqlite) {
        QTest::addColumn<SqliteTest1List>("data1");
        QTest::newRow("sqlite test data1") << sqliteData1;
    } else if (engineModel == Engine_Mysql) {
        QTest::addColumn<MysqlTest1List>("data1");
        QTest::newRow("mysql test data1") << mysqlData1;
    } else if (engineModel == Engine_SqlServer) {
        QTest::addColumn<SqlServerTest1List>("data1");
        QTest::newRow("sqlserver test data1") << sqlserverData1;
    }
}

template<typename E>
void runUniqueSelectTest(EngineModel model) {
    QFETCH(QList<E>, data1);
    typename E::Fields sf1;
    typename E::Tool sft1;
    auto d1 = dao::_select<E>()
        .filter(sf1.name == "client", _or(sf1.number == 12, sf1.name == "bob"))
        .build().one();
    QVERIFY(d1.id != -1);
    QCOMPARE(sft1.getValueWithoutAutoIncrement(data1.at(4)), sft1.getValueWithoutAutoIncrement(d1));

    auto v1 = dao::_select<E>()
            .filter(sf1.id == 2)
            .build()
            .one(sf1.name);
    QCOMPARE(v1, "alice");

    auto v2 = dao::_select<E>()
            .filter(sf1.id == 2)
            .build()
            .one(sf1.name, sf1.number);
    QCOMPARE(v2.first, "alice");
    QCOMPARE(v2.second, 11);

    auto v3 = dao::_select<E>()
            .filter(sf1.id == 2)
            .build()
            .one(sf1.id, sf1.name, sf1.number, sf1.hex);
    QCOMPARE(std::get<0>(v3), 2);
    QCOMPARE(std::get<1>(v3), "alice");
    QCOMPARE(std::get<2>(v3), 11);
    QCOMPARE(std::get<3>(v3), "alice1");

    E d2;
    if (model == Engine_SqlServer) {
        d2 = dao::_select<E>()
            .filter(sf1.id <= 3)
            .with(_orderBy(sf1.name.desc()))
            .build().top(1).unique();
    } else {
        d2 = dao::_select<E>()
            .filter(sf1.id <= 3)
            .with(_orderBy(sf1.name.desc()), _limit(1))
            .build().unique();
    }
    QVERIFY(d2.id != -1);
    QCOMPARE(sft1.getValueWithoutAutoIncrement(data1.at(2)), sft1.getValueWithoutAutoIncrement(d2));

    try {
        auto d3 = dao::_select<E>()
            .filter(sf1.number == 12)
            .build().unique();
        QFAIL("unique test should be fail!");
    }
    catch (DaoException&) {
    }

    auto d4 = dao::_select<E>()
        .filter(sf1.name == "joker")
        .build().unique();
    QVERIFY(d4.id == -1);

    auto uv1 = dao::_select<E>()
            .filter(sf1.id == 2)
            .build()
            .unique(sf1.name);
    QCOMPARE(uv1, "alice");

    auto uv2 = dao::_select<E>()
            .filter(sf1.id == 2)
            .build()
            .unique(sf1.name, sf1.number);
    QCOMPARE(uv2.first, "alice");
    QCOMPARE(uv2.second, 11);

    auto uv3 = dao::_select<E>()
            .filter(sf1.id == 2)
            .build()
            .unique(sf1.id, sf1.name, sf1.number, sf1.hex);
    QCOMPARE(std::get<0>(uv3), 2);
    QCOMPARE(std::get<1>(uv3), "alice");
    QCOMPARE(std::get<2>(uv3), 11);
    QCOMPARE(std::get<3>(uv3), "alice1");
}

void SelectTest::uniqueSelectTest() {
    if (engineModel == Engine_Sqlite) {
        runUniqueSelectTest<SqliteTest1>(engineModel);
    } else if (engineModel == Engine_Mysql) {
        runUniqueSelectTest<MysqlTest1>(engineModel);
    } else if (engineModel == Engine_SqlServer) {
        runUniqueSelectTest<SqlServerTest1>(engineModel);
    }
}

void SelectTest::listSelectTest_data() {
    if (engineModel == Engine_Sqlite) {
        QTest::addColumn<SqliteTest1List>("data1");
        QTest::newRow("sqlite test data1") << sqliteData1;
    } else if (engineModel == Engine_Mysql) {
        QTest::addColumn<MysqlTest1List>("data1");
        QTest::newRow("mysql test data1") << mysqlData1;
    } else if (engineModel == Engine_SqlServer) {
        QTest::addColumn<SqlServerTest1List>("data1");
        QTest::newRow("sqlserver test data1") << sqlserverData1;
    }
}

template<typename E>
void runListSelectTest() {
    QFETCH(QList<E>, data1);
    typename E::Fields sf1;
    typename E::Tool sft1;
    auto d1 = dao::_select<E>()
        .filter(sf1.number == 12)
        .build().list();
    QCOMPARE(d1.size(), 2);
    QList<QVariantList> actualValues;
    for (const auto& d : d1) {
        actualValues << sft1.getValueWithoutAutoIncrement(d);
    }
    QList<QVariantList> expectValue;
    expectValue << sft1.getValueWithoutAutoIncrement(data1.at(2));
    expectValue << sft1.getValueWithoutAutoIncrement(data1.at(4));
    QCOMPARE(actualValues, expectValue);

    auto v1 = dao::_select<E>()
            .filter(sf1.number == 12)
            .build()
            .list(sf1.name);
    QList<QString> expectNames = { "bob", "client" };
    QCOMPARE(v1, expectNames);

    auto v2 = dao::_select<E>()
            .filter(sf1.number == 12)
            .build()
            .list(sf1.name, sf1.number);
    QList<QPair<QString, qreal>> expectV2;
    expectV2 << qMakePair(data1.at(2).name, data1.at(2).number);
    expectV2 << qMakePair(data1.at(4).name, data1.at(4).number);
    QCOMPARE(v2, expectV2);

    auto v3 = dao::_select<E>()
            .filter(sf1.number == 12)
            .build()
            .list(sf1.id, sf1.name, sf1.number, sf1.hex);
    QList<std::tuple<qint64, QString, qreal, QByteArray>> expectV3;
    expectV3 << std::make_tuple(data1.at(2).id, data1.at(2).name, data1.at(2).number, data1.at(2).hex);
    expectV3 << std::make_tuple(data1.at(4).id, data1.at(4).name, data1.at(4).number, data1.at(4).hex);
    QCOMPARE(v3, expectV3);

    auto d2 = dao::_selectAll<E>();
    QCOMPARE(d2.size(), data1.size());
    actualValues.clear();
    for (const auto& d : d2) {
        actualValues << sft1.getValueWithoutAutoIncrement(d);
    }
    expectValue.clear();
    for (const auto& d : data1) {
        expectValue << sft1.getValueWithoutAutoIncrement(d);
    }
    QCOMPARE(actualValues, expectValue);

    auto d3 = dao::_select<E>()
        .filter(sf1.id > 10)
        .build().list();
    QVERIFY(d3.isEmpty());
}

void SelectTest::listSelectTest() {
    if (engineModel == Engine_Sqlite) {
        runListSelectTest<SqliteTest1>();
    } else if (engineModel == Engine_Mysql) {
        runListSelectTest<MysqlTest1>();
    } else if (engineModel == Engine_SqlServer) {
        runListSelectTest<SqlServerTest1>();
    }
}

void SelectTest::rawSelectTest_data() {
    if (engineModel == Engine_Sqlite) {
        QTest::addColumn<SqliteTest1List>("data1");
        QTest::newRow("sqlite test data1") << sqliteData1;
    } else if (engineModel == Engine_Mysql) {
        QTest::addColumn<MysqlTest1List>("data1");
        QTest::newRow("mysql test data1") << mysqlData1;
    } else if (engineModel == Engine_SqlServer) {
        QTest::addColumn<SqlServerTest1List>("data1");
        QTest::newRow("sqlserver test data1") << sqlserverData1;
    }
}

template<typename E>
void runRawSelectTest() {
    QFETCH(QList<E>, data1);
    typename E::Fields sf1;
    dao::_select<E>()
        .filter(sf1.number == 14)
        .build().raw([&](QSqlQuery& query) {
        if (query.next()) {
            QVariantList result;
            result << query.value(sf1.id());
            result << query.value(sf1.name());
            result << query.value(sf1.number());
            result << query.value(sf1.hex());

            QCOMPARE(result, E::Tool::getValueWithoutAutoIncrement(data1.at(3)));
        }
    });
}

void SelectTest::rawSelectTest() {
    if (engineModel == Engine_Sqlite) {
        runRawSelectTest<SqliteTest1>();
    } else if (engineModel == Engine_Mysql) {
        runRawSelectTest<MysqlTest1>();
    } else if (engineModel == Engine_SqlServer) {
        runRawSelectTest<SqlServerTest1>();
    }
}

template<typename E>
void runFuntionSelectTest() {
    typename E::Fields sf1;
    auto data = dao::_select<E>()
        .column(_fun("sum(%1) + ? as sumnumber").field(sf1.number).value(5))
        .filter(_or(sf1.name.like("a%"), _fun("%1 > case when %2 = ? then ? else ? end").field(sf1.number, sf1.name).value("client", 12, 10)))
        .build().one();
    QCOMPARE(data.template __getExtra<int>("sumnumber"), 52);
}

void SelectTest::funtionSelectTest() {
    if (engineModel == Engine_Sqlite) {
        runFuntionSelectTest<SqliteTest1>();
    } else if (engineModel == Engine_Mysql) {
        runFuntionSelectTest<MysqlTest1>();
    } else if (engineModel == Engine_SqlServer) {
        runFuntionSelectTest<SqlServerTest1>();
    }
}

template<typename E>
void runCountSelectTest() {
    typename E::Fields sf1;

    int count = dao::_count<E>().count();
    QCOMPARE(count, 5);
    
    count = dao::_count<E>()
        .filter(false, sf1.name.like("%a%"))
        .filter(sf1.name.like("%l%")).count();
    QCOMPARE(count, 3);

    count = dao::_count<E>()
        .filter(sf1.number > 10)
        .count();
    QCOMPARE(count, 4);
}

void SelectTest::countSelectTest() {
    if (engineModel == Engine_Sqlite) {
        runCountSelectTest<SqliteTest1>();
    } else if (engineModel == Engine_Mysql) {
        runCountSelectTest<MysqlTest1>();
    } else if (engineModel == Engine_SqlServer) {
        runCountSelectTest<SqlServerTest1>();
    }
}

void SelectTest::selectFromSelectTest_data() {
    if (engineModel == Engine_Sqlite) {
        QTest::addColumn<SqliteTest1List>("data1");
        QTest::newRow("sqlite test data1") << sqliteData1;
    } else if (engineModel == Engine_Mysql) {
        QTest::addColumn<MysqlTest1List>("data1");
        QTest::newRow("mysql test data1") << mysqlData1;
    } else if (engineModel == Engine_SqlServer) {
        QTest::addColumn<SqlServerTest1List>("data1");
        QTest::newRow("sqlserver test data1") << sqlserverData1;
    }
}

template<typename E>
void runSelectFromSelectTest() {
    QFETCH(QList<E>, data1);
    typename E::Fields sf1;
    auto select = dao::_select<E>().filter(sf1.number > 10).build();
    try {
        auto select1 = dao::_select<E>()
            .from(select)
            .filter(sf1.name.like("%b%"))
            .build();

        auto result = select1.unique();
        QCOMPARE(
            E::Tool::getValueWithoutAutoIncrement(result),
            E::Tool::getValueWithoutAutoIncrement(data1.at(2))
        );

        auto count = dao::_count<E>()
            .from(select1)
            .filter(sf1.number != 0)
            .count();
        QCOMPARE(count, 1);
    }
    catch (DaoException& e) {
        QFAIL(("test select from select fail!" + e.reason).toLocal8Bit());
    }
}

void SelectTest::selectFromSelectTest() {
    if (engineModel == Engine_Sqlite) {
        runSelectFromSelectTest<SqliteTest1>();
    } else if (engineModel == Engine_Mysql) {
        runSelectFromSelectTest<MysqlTest1>();
    } else if (engineModel == Engine_SqlServer) {
        runSelectFromSelectTest<SqlServerTest1>();
    }
}

void SelectTest::unionSelectTest_data() {
    if (engineModel == Engine_Sqlite) {
        QTest::addColumn<SqliteTest1List>("data1");
        QTest::addColumn<SqliteTest2List>("data2");
        QTest::newRow("sqlite test data1") << sqliteData1 << sqliteData2;
    } else if (engineModel == Engine_Mysql) {
        QTest::addColumn<MysqlTest1List>("data1");
        QTest::addColumn<MysqlTest2List>("data2");
        QTest::newRow("mysql test data1") << mysqlData1 << mysqlData2;
    } else if (engineModel == Engine_SqlServer) {
        QTest::addColumn<SqlServerTest1List>("data1");
        QTest::addColumn<SqlServerTest2List>("data2");
        QTest::newRow("sqlserver test data1") << sqlserverData1 << sqlserverData2;
    }
}

template<typename E1, typename E2>
void runUnionSelectTest() {
    QFETCH(QList<E1>, data1);
    QFETCH(QList<E2>, data2);

    typename E1::Fields sf1;
    typename E2::Fields sf2;

    auto select1 = dao::_select<E1>()
        .column(sf1.name, sf1.number)
        .filter(sf1.number >= 12)
        .with(_orderBy(sf1.name, sf1.number))
        .build();

    auto select2 = dao::_select<E2>()
        .column(sf2.name, sf2.number)
        .filter(sf2.number >= 12)
        .unionSelect(select1)
        .build().list();

    QVariantList results;
    for (const auto& r : select2) {
        results << r.name << r.number;
    }
    QVariantList expected;
    QList<QPair<QString, qreal>> expectList;
    for (const auto& d : data1) {
        if (d.number < 12)
            continue;
        expectList << qMakePair(d.name, d.number);
    }
    for (const auto& d : data2) {
        if (d.number < 13)
            continue;
        expectList << qMakePair(d.name, d.number);
    }
    std::sort(expectList.begin(), expectList.end());
    for (const auto& d : expectList) {
        expected << d.first << d.second;
    }
    QCOMPARE(results, expected);
}

void SelectTest::unionSelectTest() {
    if (engineModel == Engine_Sqlite) {
        runUnionSelectTest<SqliteTest1, SqliteTest2>();
    } else if (engineModel == Engine_Mysql) {
        runUnionSelectTest<MysqlTest1, MysqlTest2>();
    } else if (engineModel == Engine_SqlServer) {
#if QT_VERSION_MAJOR < 6
        runUnionSelectTest<SqlServerTest1, SqlServerTest2>();
#else
        qDebug() << "ignore union test in qt6";
#endif
    }
}

void SelectTest::funtionSubSelectTest_data() {
    if (engineModel == Engine_Sqlite) {
        QTest::addColumn<SqliteTest1List>("data1");
        QTest::newRow("sqlite test data1") << sqliteData1;
    } else if (engineModel == Engine_Mysql) {
        QTest::addColumn<MysqlTest1List>("data1");
        QTest::newRow("mysql test data1") << mysqlData1;
    } else if (engineModel == Engine_SqlServer) {
        QTest::addColumn<SqlServerTest1List>("data1");
        QTest::newRow("sqlserver test data1") << sqlserverData1;
    }
}

template<typename E1, typename E2>
void runFuntionSubSelectTest() {
    QFETCH(QList<E1>, data1);
    typename E1::Fields sf1;
    typename E2::Fields sf2;
    auto select = dao::_select<E2>().column(sf2.number).filter(sf2.number < 15).build();
    auto data = dao::_select<E1>()
        .filter(_fun("%1 in %2").field(sf1.number).from(select))
        .with(_orderBy(sf1.number))
        .build().list();
    QVariantList actual;
    for (const auto& d : data) {
        actual << E1::Tool::getValueWithoutAutoIncrement(d);
    }
    QVariantList expected;
    for (const auto& d : data1) {
        if (d.number != 10 && d.number != 12)
            continue;
        expected << E1::Tool::getValueWithoutAutoIncrement(d);
    }
    QCOMPARE(actual, expected);
}

void SelectTest::funtionSubSelectTest() {
    if (engineModel == Engine_Sqlite) {
        runFuntionSubSelectTest<SqliteTest1, SqliteTest2>();
    } else if (engineModel == Engine_Mysql) {
        runFuntionSubSelectTest<MysqlTest1, MysqlTest2>();
    } else if (engineModel == Engine_SqlServer) {
        runFuntionSubSelectTest<SqlServerTest1, SqlServerTest2>();
    }
}

void SelectTest::explainTest() {
    if (engineModel == Engine_Sqlite) {
        SqliteTest1::Fields sf1;
        auto d1 = dao::_select<SqliteTest1>()
            .filter(sf1.name == "client", _or(sf1.number == 12, sf1.name == "bob"))
            .build().explain<SqliteExplainInfo>();
        QVERIFY(!d1.isEmpty());

        auto d2 = dao::_select<SqliteTest1>()
            .filter(sf1.hex == "abcde")
            .build().explain<SqliteExplainQueryPlanInfo>();
        QVERIFY(!d2.isEmpty());
    } else if (engineModel == Engine_Mysql) {
        MysqlTest3::Fields mf;
        auto d = dao::_select<MysqlTest3>()
            .filter(
                mf.tbi1 == 1,
                mf.name == "abc",
                mf.size == 1
            ).build().explain<MysqlExplainInfo>();
        QVERIFY(!d.isEmpty());
    } else if (engineModel == Engine_SqlServer) {
        SqlServerTest2::Fields sf;
        auto d = dao::_select<SqlServerTest2>()
            .filter(
                sf.id >= 1,
                sf.name == "func",
                sf.number != 0
            ).build().explain<SqlServerExplainInfo>();
        QVERIFY(!d.isEmpty());
    }
}

void SelectTest::cleanup() {
    clearCacheAndPrintIfTestFail();
}

void SelectTest::cleanupTestCase() {
    globalConfig->getClient()->dropDatabase();
}

