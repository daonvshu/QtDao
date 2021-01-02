#include "SelectTest.h"

#include <QtTest>

#include "../../src/dao.h"

void SelectTest::initTestCase() {
    DbLoader::init(SqliteConfig());

    data1 << SqliteTest1(1, "abc", 10, "");
    data1 << SqliteTest1(2, "alice", 11, "alice1");
    data1 << SqliteTest1(3, "bob", 12, "bob boom");
    data1 << SqliteTest1(4, "client", 14, "1");
    data1 << SqliteTest1(5, "client", 12, "xxx");
    dao::_insert<SqliteTest1>().build().insert2(data1);

    data2 << SqliteTest2("joker", 9999, -1, 30);
    data2 << SqliteTest2("bob", 12, 0, "abc");
    data2 << SqliteTest2("func", 10, -2, 50);
    data2 << SqliteTest2("func", 50, 0, 50);
    dao::_insert<SqliteTest2>().build().insert2(data2);
}

void SelectTest::uniqueSelectTest() {
    SqliteTest1::Fields sf1;
    SqliteTest1::Tool sft1;
    auto d1 = dao::_select<SqliteTest1>()
        .filter(sf1.name == "client", _or(sf1.number == 12, sf1.name == "bob"))
        .build().one();
    QVERIFY(d1.getId() != -1);
    QCOMPARE(sft1.getValueWithoutAutoIncrement(data1.at(4)), sft1.getValueWithoutAutoIncrement(d1));

    auto d2 = dao::_select<SqliteTest1>()
        .filter(sf1.id <= 3)
        .with(_orderBy(sf1.name.desc()), _limit(1))
        .build().unique();
    QVERIFY(d2.getId() != -1);
    QCOMPARE(sft1.getValueWithoutAutoIncrement(data1.at(2)), sft1.getValueWithoutAutoIncrement(d2));

    try {
        auto d3 = dao::_select<SqliteTest1>()
            .filter(sf1.number == 12)
            .throwable().build().unique();
        QFAIL("unique test should be fail!");
    }
    catch (DaoException&) {
    }

    auto d4 = dao::_select<SqliteTest1>()
        .filter(sf1.name == "joker")
        .build().unique();
    QVERIFY(d4.getId() == -1);
}

void SelectTest::listSelectTest() {
    SqliteTest1::Fields sf1;
    SqliteTest1::Tool sft1;
    auto d1 = dao::_select<SqliteTest1>()
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

    auto d2 = dao::_selectAll<SqliteTest1>();
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

    auto d3 = dao::_select<SqliteTest1>()
        .filter(sf1.id > 10)
        .build().list();
    QVERIFY(d3.isEmpty());
}

void SelectTest::rawSelectTest() {
    SqliteTest1::Fields sf1;
    dao::_select<SqliteTest1>()
        .filter(sf1.number == 14)
        .build().raw([&](QSqlQuery& query) {
        if (query.next()) {
            QVariantList result;
            result << query.value(sf1.id());
            result << query.value(sf1.name());
            result << query.value(sf1.number());
            result << query.value(sf1.hex());

            QCOMPARE(result, SqliteTest1::Tool::getValueWithoutAutoIncrement(data1.at(3)));
        }
    });
}

void SelectTest::funtionSelectTest() {
    SqliteTest1::Fields sf1;
    auto data = dao::_select<SqliteTest1>()
        .column(sf1.id, _fun("sum(%1) + ? as sumnumber").field(sf1.number).value(5), sf1.name)
        .filter(_or(sf1.name.like("a%"), _fun("%1 > case when %2 = ? then ? else ? end").field(sf1.number, sf1.name).value("client", 12, 10)))
        .build().one();
    QCOMPARE(data.__getExtra("sumnumber").toInt(), 52);
}

void SelectTest::countSelectTest() {
    SqliteTest1::Fields sf1;

    int count = dao::_count<SqliteTest1>().count();
    QCOMPARE(count, 5);
    
    count = dao::_count<SqliteTest1>()
        .filter(sf1.name.like("%l%")).count();
    QCOMPARE(count, 3);

    count = dao::_count<SqliteTest1>()
        .filter(sf1.number > 10)
        .count();
    QCOMPARE(count, 4);
}

void SelectTest::selectFromSelectTest() {
    SqliteTest1::Fields sf1;
    auto select = dao::_select<SqliteTest1>().filter(sf1.number > 10).build();
    try {
        auto select1 = dao::_select<SqliteTest1>()
            .from(select)
            .filter(sf1.name.like("%b%"))
            .throwable()
            .build();

        auto result = select1.unique();
        QCOMPARE(
            SqliteTest1::Tool::getValueWithoutAutoIncrement(result),
            SqliteTest1::Tool::getValueWithoutAutoIncrement(data1.at(2))
        );

        auto count = dao::_count<SqliteTest1>()
            .from(select1)
            .throwable()
            .filter(sf1.number != 0)
            .count();
        QCOMPARE(count, 1);
    }
    catch (DaoException& e) {
        QFAIL(("test select from select fail!" + e.reason).toUtf8());
    }
}

void SelectTest::unionSelectTest() {
    SqliteTest1::Fields sf1;
    SqliteTest2::Fields sf2;

    auto select1 = dao::_select<SqliteTest1>()
        .column(sf1.name, sf1.number)
        .filter(sf1.number >= 12)
        .with(_orderBy(sf1.number))
        .build();

    auto select2 = dao::_select<SqliteTest2>()
        .column(sf2.name, sf2.number)
        .filter(sf2.number >= 12)
        .unionSelect(select1)
        .build().list();

    QVariantList results;
    for (const auto& r : select2) {
        results << r.getName() << r.getNumber();
    }
    QVariantList expected;
    QList<QPair<QString, qreal>> expectList;
    for (const auto& d : data1) {
        if (d.getNumber() < 12)
            continue;
        expectList << qMakePair(d.getName(), d.getNumber());
    }
    for (const auto& d : data2) {
        if (d.getNumber() < 13)
            continue;
        expectList << qMakePair(d.getName(), d.getNumber());
    }
    std::sort(expectList.begin(), expectList.end());
    for (const auto& d : expectList) {
        expected << d.first << d.second;
    }
    QCOMPARE(results, expected);
}

void SelectTest::funtionSubSelectTest() {
    SqliteTest1::Fields sf1;
    SqliteTest2::Fields sf2;
    auto select = dao::_select<SqliteTest2>().column(sf2.number).filter(sf2.number < 15).build();
    auto data = dao::_select<SqliteTest1>()
        .filter(_fun("%1 in %2").field(sf1.number).from(select))
        .with(_orderBy(sf1.number))
        .build().list();
    QVariantList actual;
    for (const auto& d : data) {
        actual << SqliteTest1::Tool::getValueWithoutAutoIncrement(d);
    }
    QVariantList expected;
    for (const auto& d : data1) {
        if (d.getNumber() != 10 && d.getNumber() != 12)
            continue;
        expected << SqliteTest1::Tool::getValueWithoutAutoIncrement(d);
    }
    QCOMPARE(actual, expected);
}

void SelectTest::cleanup() {
    clearCacheAndPrintIfTestFail();
}

void SelectTest::cleanupTestCase() {
    ConnectionPool::release();
    DbLoader::getClient().dropDatabase();
}

