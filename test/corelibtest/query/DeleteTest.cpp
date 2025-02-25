#include "DeleteTest.h"

#include <QtTest>

#include "dao.h"

#include "entity/sqliteentity/sqlitetest1.h"
#include "entity/sqliteentity/sqlitetest2.h"

#include "entity/mysqlentity/mysqltest1.h"
#include "entity/mysqlentity/mysqltest2.h"

#include "entity/sqlserverentity/sqlservertest1.h"
#include "entity/sqlserverentity/sqlservertest2.h"

#include "entity/psqlentity/psqltest1.h"
#include "entity/psqlentity/psqltest2.h"

void DeleteTest::initTestCase() {
    configDb();

    if (targetDb == TestTargetDb::Target_Sqlite) {
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
    } else if (targetDb == TestTargetDb::Target_Mysql) {
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
    } else if (targetDb == TestTargetDb::Target_SqlServer) {
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
    } else if (targetDb == TestTargetDb::Target_PSql) {
        PSqlTest1List data1;
        PSqlTest2List data2;

        data1 << PSqlTest1(1, "abc", 10, "");
        data1 << PSqlTest1(2, "alice", 11, "alice1");
        data1 << PSqlTest1(3, "bob", 12, "bob boom");
        data1 << PSqlTest1(4, "client", 14, "1");
        data1 << PSqlTest1(5, "client", 12, "xxx");
        dao::_insert<PSqlTest1>().build().insert2(data1);

        data2 << PSqlTest2("joker", 9999, -1);
        data2 << PSqlTest2("bob", 10, 0);
        data2 << PSqlTest2("func", 10, -2);
        data2 << PSqlTest2("func", 50, 0);
        dao::_insert<PSqlTest2>().build().insert2(data2);
    }
}

template<typename E>
void runFilterDeleteTest() {
    typename E::Fields sf1;
    dao::_delete<E>().filter(sf1.name.like("a%")).build().deleteBy();
    auto result1 = dao::_select<E>().filter(sf1.name.in(QStringList() << "abc" << "alice")).build().list();
    QVERIFY(result1.isEmpty());

    QStringList target;
    target << "bob" << "client";
    dao::_delete<E>().filter(sf1.name == target).build().deleteBatch();
    auto result2 = dao::_select<E>().filter(sf1.name.in(target)).build().list();
    QVERIFY(result2.isEmpty());
}

void DeleteTest::filterDeleteTest() {
    if (targetDb == TestTargetDb::Target_Sqlite) {
        runFilterDeleteTest<SqliteTest1>();
    } else if (targetDb == TestTargetDb::Target_Mysql) {
        runFilterDeleteTest<MysqlTest1>();
    } else if (targetDb == TestTargetDb::Target_SqlServer) {
        runFilterDeleteTest<SqlServerTest1>();
    } else if (targetDb == TestTargetDb::Target_PSql) {
        runFilterDeleteTest<PSqlTest1>();
    }
}

template<typename E>
void runObjectDeleteTest() {
    typename E::Fields sf2;
    auto result1 = dao::_select<E>().filter(sf2.name == "joker").build().unique();
    dao::_delete<E>().build().deleteBy(result1);
    result1 = dao::_select<E>().filter(sf2.name == "joker").build().unique();
    QVERIFY(result1.id == -1);

    auto result2 = dao::_selectAll<E>();
    dao::_delete<E>().build().deleteBatch(result2);
    result2 = dao::_selectAll<E>();
    QVERIFY(result2.isEmpty());
}

void DeleteTest::objectDeleteTest() {
    if (targetDb == TestTargetDb::Target_Sqlite) {
        runObjectDeleteTest<SqliteTest2>();
    } else if (targetDb == TestTargetDb::Target_Mysql) {
        runObjectDeleteTest<MysqlTest2>();
    } else if (targetDb == TestTargetDb::Target_SqlServer) {
        runObjectDeleteTest<SqlServerTest2>();
    } else if (targetDb == TestTargetDb::Target_PSql) {
        runObjectDeleteTest<PSqlTest2>();
    }
}

void DeleteTest::truncateTest_data() {
    if (targetDb == TestTargetDb::Target_Sqlite) {
        SqliteTest2List data2;
        data2 << SqliteTest2("func2", 10, -2, 50);
        data2 << SqliteTest2("func2", 50, 0, 50);

        auto entity = SqliteTest2("func2", 50, 0, 50);

        QTest::addColumn<SqliteTest2List>("data2");
        QTest::addColumn<SqliteTest2>("entity");
        QTest::newRow("sqlite test data") << data2 << entity;

    } else if (targetDb == TestTargetDb::Target_Mysql) {
        MysqlTest2List data2;
        data2 << MysqlTest2("func2", 10, -2);
        data2 << MysqlTest2("func2", 50, 0);

        auto entity = MysqlTest2("func2", 50, 0);

        QTest::addColumn<MysqlTest2List>("data2");
        QTest::addColumn<MysqlTest2>("entity");
        QTest::newRow("mysql test data") << data2 << entity;

    } else if (targetDb == TestTargetDb::Target_SqlServer) {
        SqlServerTest2List data2;
        data2 << SqlServerTest2("func2", 10, -2);
        data2 << SqlServerTest2("func2", 50, 0);

        auto entity = SqlServerTest2("func2", 50, 0);

        QTest::addColumn<SqlServerTest2List>("data2");
        QTest::addColumn<SqlServerTest2>("entity");
        QTest::newRow("mysql test data") << data2 << entity;

    } else if (targetDb == TestTargetDb::Target_PSql) {
        PSqlTest2List data2;
        data2 << PSqlTest2("func2", 10, -2);
        data2 << PSqlTest2("func2", 50, 0);

        auto entity = PSqlTest2("func2", 50, 0);

        QTest::addColumn<PSqlTest2List>("data2");
        QTest::addColumn<PSqlTest2>("entity");
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
    QVERIFY(entity.id == 1);
}

void DeleteTest::truncateTest() {
    if (targetDb == TestTargetDb::Target_Sqlite) {
        runTruncateTest<SqliteTest2>();
    } else if (targetDb == TestTargetDb::Target_Mysql) {
        runTruncateTest<MysqlTest2>();
    } else if (targetDb == TestTargetDb::Target_SqlServer) {
        runTruncateTest<SqlServerTest2>();
    } else if (targetDb == TestTargetDb::Target_PSql) {
        runTruncateTest<PSqlTest2>();
    }
}

void DeleteTest::cleanup() {
    clearCacheAndPrintIfTestFail();
}

void DeleteTest::cleanupTestCase() {
    ConfigManager::getConfig()->getClient()->dropDatabase();
}

