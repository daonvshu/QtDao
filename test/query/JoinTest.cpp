#include "JoinTest.h"

#include <qtest.h>

#include "../sqliteentity/SqliteTest1.h"
#include "../sqliteentity/SqliteTest2.h"

#include "../../src/dao.h"

void JoinTest::initTestCase() {
    DbLoader::init(":/QtDao/test/sqliteentity/sqlite_cfg.xml");

    data1 << SqliteTest1(1, "abc", 10, "");
    data1 << SqliteTest1(2, "alice", 11, "alice1");
    data1 << SqliteTest1(3, "bob", 12, "bob boom");
    data1 << SqliteTest1(4, "client", 14, "1");
    data1 << SqliteTest1(5, "client", 12, "xxx");
    dao::_insert<SqliteTest1>().build().insert2(data1);

    data2 << SqliteTest2(0, "joker", 9999, -1, 30);
    data2 << SqliteTest2(0, "bob", 10, 0, "abc");
    data2 << SqliteTest2(0, "func", 10, -2, 50);
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
        QVariantList() << "bob" << "bob" << "bob group" << 12 << 10
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
        QVariantList() << 1 << "abc" << 10 << "" << "func" << 10 << -2 << 50 << 1 << 3 << "func group1" << 6
    );
}

void JoinTest::testJoinSelfTable() {
    
}

void JoinTest::cleanup() {
}

void JoinTest::cleanupTestCase() {
    ConnectionPool::release();
    DbLoader::getClient().dropDatabase();
}