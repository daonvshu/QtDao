﻿#include "DeleteTest.h"

#include <QtTest>

#include "../../src/dao.h"

#include "../sqliteentity/SqliteTest1.h"
#include "../sqliteentity/SqliteTest2.h"

#include "../mysqlentity/MysqlTest1.h"
#include "../mysqlentity/MysqlTest2.h"

void DeleteTest::initTestCase() {
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
    }
}

template<typename E>
void runFilterDeleteTest() {
    typename E::Fields sf1;
    bool success = dao::_delete<E>().filter(sf1.name.like("a%")).build().deleteBy();
    QVERIFY(success);
    auto result1 = dao::_select<E>().filter(sf1.name.in(QStringList() << "abc" << "alice")).build().list();
    QVERIFY(result1.isEmpty());

    QStringList target;
    target << "bob" << "client";
    success = dao::_delete<E>().filter(sf1.name == target).build().deleteBatch();
    QVERIFY(success);
    auto result2 = dao::_select<E>().filter(sf1.name.in(target)).build().list();
    QVERIFY(result2.isEmpty());
}

void DeleteTest::filterDeleteTest() {
    if (engineModel == Engine_Sqlite) {
        runFilterDeleteTest<SqliteTest1>();
    } else if (engineModel == Engine_Mysql) {
        runFilterDeleteTest<MysqlTest1>();
    }
}

template<typename E>
void runObjectDeleteTest() {
    typename E::Fields sf2;
    auto result1 = dao::_select<E>().filter(sf2.name == "joker").build().unique();
    bool success = dao::_delete<E>().build().deleteBy(result1);
    QVERIFY(success);
    result1 = dao::_select<E>().filter(sf2.name == "joker").build().unique();
    QVERIFY(result1.getId() == -1);

    auto result2 = dao::_selectAll<E>();
    success = dao::_delete<E>().build().deleteBatch(result2);
    QVERIFY(success);
    result2 = dao::_selectAll<E>();
    QVERIFY(result2.isEmpty());
}

void DeleteTest::objectDeleteTest() {
    if (engineModel == Engine_Sqlite) {
        runObjectDeleteTest<SqliteTest2>();
    } else if (engineModel == Engine_Mysql) {
        runObjectDeleteTest<MysqlTest2>();
    }
}

void DeleteTest::truncateTest_data() {
    if (engineModel == Engine_Sqlite) {
        SqliteTest2List data2;
        data2 << SqliteTest2("func2", 10, -2, 50);
        data2 << SqliteTest2("func2", 50, 0, 50);

        auto entity = SqliteTest2("func2", 50, 0, 50);

        QTest::addColumn<SqliteTest2List>("data2");
        QTest::addColumn<SqliteTest2>("entity");
        QTest::newRow("sqlite test data") << data2 << entity;

    } else if (engineModel == Engine_Mysql) {
        MysqlTest2List data2;
        data2 << MysqlTest2("func2", 10, -2);
        data2 << MysqlTest2("func2", 50, 0);

        auto entity = MysqlTest2("func2", 50, 0);

        QTest::addColumn<MysqlTest2List>("data2");
        QTest::addColumn<MysqlTest2>("entity");
        QTest::newRow("mysql test data") << data2 << entity;

    }
}

template<typename E>
void runTruncateTest() {
    QFETCH(QList<E>, data2);
    dao::_insert<E>().build().insert2(data2);

    dao::_truncate<E>();
    auto select = dao::_selectAll<E>();
    QVERIFY(select.isEmpty());

    QFETCH(E, entity);
    dao::_insert<E>().build().insert(entity);
    QVERIFY(entity.getId() == 1);
}

void DeleteTest::truncateTest() {
    if (engineModel == Engine_Sqlite) {
        runTruncateTest<SqliteTest2>();
    } else if (engineModel == Engine_Mysql) {
        runTruncateTest<MysqlTest2>();
    }
}

void DeleteTest::cleanup() {
    clearCacheAndPrintIfTestFail();
}

void DeleteTest::cleanupTestCase() {
    ConnectionPool::release();
    DbLoader::getClient().dropDatabase();
}

