#include "UpdateTest.h"

#include <QtTest>

#include "dao.h"

#include "sqliteentity/sqlitetest1.h"
#include "sqliteentity/sqlitetest2.h"

#include "mysqlentity/mysqltest1.h"
#include "mysqlentity/mysqltest2.h"

#include "sqlserverentity/sqlservertest1.h"
#include "sqlserverentity/sqlservertest2.h"

void UpdateTest::initTestCase() {
    configDb();

    if (engineModel == Engine_Sqlite) {
        SqliteTest1List data1;
        SqliteTest2List data2;

        data1 << SqliteTest1(1, "abc", 10, "");
        data1 << SqliteTest1(2, "alice", 11, "alice1");
        data1 << SqliteTest1(3, "bob", 12, "bob boom");
        data1 << SqliteTest1(4, "client", 14, "1");
        data1 << SqliteTest1(5, "client", 12, "xxx");
        dao::_insert<SqliteTest1>().build().insert2(data1);

        data2 << SqliteTest2("joker", 9999, -1, 30);
        data2 << SqliteTest2("bob", 10, 0, "abc");
        data2 << SqliteTest2("func", 10, -2, 50);
        data2 << SqliteTest2("func", 50, 0, 50);
        dao::_insert<SqliteTest2>().build().insert2(data2);
    } else if (engineModel == Engine_Mysql) {
        MysqlTest1List data1;
        MysqlTest2List data2;

        data1 << MysqlTest1(1, "abc", 10, "");
        data1 << MysqlTest1(2, "alice", 11, "alice1");
        data1 << MysqlTest1(3, "bob", 12, "bob boom");
        data1 << MysqlTest1(4, "client", 14, "1");
        data1 << MysqlTest1(5, "client", 12, "xxx");
        dao::_insert<MysqlTest1>().build().insert2(data1);

        data2 << MysqlTest2("joker", 9999, -1);
        data2 << MysqlTest2("bob", 10, 0);
        data2 << MysqlTest2("func", 10, -2);
        data2 << MysqlTest2("func", 50, 0);
        dao::_insert<MysqlTest2>().build().insert2(data2);
    } else if (engineModel == Engine_SqlServer) {
        SqlServerTest1List data1;
        SqlServerTest2List data2;

        data1 << SqlServerTest1(1, "abc", 10, "");
        data1 << SqlServerTest1(2, "alice", 11, "alice1");
        data1 << SqlServerTest1(3, "bob", 12, "bob boom");
        data1 << SqlServerTest1(4, "client", 14, "1");
        data1 << SqlServerTest1(5, "client", 12, "xxx");
        dao::_insert<SqlServerTest1>().build().insert2(data1);

        data2 << SqlServerTest2("joker", 9999, -1);
        data2 << SqlServerTest2("bob", 10, 0);
        data2 << SqlServerTest2("func", 10, -2);
        data2 << SqlServerTest2("func", 50, 0);
        dao::_insert<SqlServerTest2>().build().insert2(data2);
    }
}

template<typename E>
void runSetUpdateTest() {
    typename E::Fields sf1;
    typename E::Tool tool1;
    bool success = dao::_update<E>()
        .set(sf1.number = 18, sf1.hex = "test update")
        .filter(sf1.name == "bob")
        .build().update();
    QVERIFY(success);
    auto result = dao::_select<E>().filter(sf1.number == 18).build().unique();
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
    success = dao::_update<E>()
        .set(sf1.number = numbersSet, sf1.hex = hexSet)
        .filter(sf1.name == targets)
        .build().updateBatch();
    QVERIFY(success);
    auto results = dao::_select<E>().filter(sf1.name.in(targets)).build().list();
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

void UpdateTest::setUpdateTest() {
    if (engineModel == Engine_Sqlite) {
        runSetUpdateTest<SqliteTest1>();
    } else if (engineModel == Engine_Mysql) {
        runSetUpdateTest<MysqlTest1>();
    } else if (engineModel == Engine_SqlServer) {
        runSetUpdateTest<SqlServerTest1>();
    }
}

template<typename E>
void runObjectUpdateTest() {
    typename E::Fields sf2;
    auto t2test = dao::_select<E>().filter(sf2.name == "bob").build().unique();
    t2test.setNumber(50);
    bool success = dao::_update<E>().build().update(t2test);
    QVERIFY(success);
    auto t2result = dao::_select<E>().filter(sf2.name == "bob").build().unique();
    QCOMPARE(
        E::Tool::getValueWithoutAutoIncrement(t2result),
        E::Tool::getValueWithoutAutoIncrement(t2test)
    );

    auto t2stest = dao::_select<E>().filter(sf2.number < 100).with(_orderBy(sf2.id)).build().list();
    for (auto& t2 : t2stest) {
        t2.setNumber2(10000);
    }
    success = dao::_update<E>().build().updateBatch(t2stest);
    QVERIFY(success);
    auto t2results = dao::_select<E>().filter(sf2.number2 == 10000).with(_orderBy(sf2.id)).build().list();
    QCOMPARE(t2results.size(), 3);
    QList<QVariantList> actualValues, expectValues;
    for (const auto& t : t2results) {
        actualValues << E::Tool::getValueWithoutAutoIncrement(t);
    }
    for (const auto& t : t2stest) {
        expectValues << E::Tool::getValueWithoutAutoIncrement(t);
    }
    QCOMPARE(actualValues, expectValues);
}

void UpdateTest::objectUpdateTest() {
    if (engineModel == Engine_Sqlite) {
        runObjectUpdateTest<SqliteTest2>();
    } else if (engineModel == Engine_Mysql) {
        runObjectUpdateTest<MysqlTest2>();
    } else if (engineModel == Engine_SqlServer) {
        runObjectUpdateTest<SqlServerTest2>();
    }
}

void UpdateTest::cleanup() {
    clearCacheAndPrintIfTestFail();
}

void UpdateTest::cleanupTestCase() {
    ConnectionPool::release();
    globalConfig->getClient()->dropDatabase();
}

