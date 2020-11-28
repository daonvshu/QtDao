#include "SelectTest.h"

#include <qtest.h>

#include "../../src/dao.h"

void SelectTest::initTestCase() {
    DbLoader::init(":/QtDao/test/sqliteentity/sqlite_cfg.xml");
    DbLoader::getClient().testConnect();

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

void SelectTest::cleanup() {

}

void SelectTest::cleanupTestCase() {
    ConnectionPool::release();
    DbLoader::getClient().dropDatabase();
}
