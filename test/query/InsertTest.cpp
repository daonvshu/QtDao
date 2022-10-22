#include "InsertTest.h"

#include <QtTest>

#include "../sqliteentity/sqlitetest1.h"
#include "../sqliteentity/sqlitetest2.h"

#include "../mysqlentity/mysqltest1.h"
#include "../mysqlentity/mysqltest2.h"

#include "../sqlserverentity/sqlservertest1.h"
#include "../sqlserverentity/sqlservertest2.h"
#include "dbexception.h"

void InsertTest::initTestCase() {
    configDb();
}

template<typename E>
void runSetInsertTest() {
    typename E::Fields sf1;
    dao::_insert<E>()
        .set(sf1.name = "test", sf1.number = 1)
        .build().insert();

    auto query = BaseQuery::queryPrimitive(QString("select *from %1").arg(E::Info::getTableName()));
    QVariantList names, numbers;
    while (query.next()) {
        names << query.value("name");
        numbers << query.value("number");
    }
    QCOMPARE(names, QVariantList() << "test");
    QCOMPARE(numbers, QVariantList() << 1);
}

void InsertTest::setInsertTest() {
    if (engineModel == Engine_Sqlite) {
        runSetInsertTest<SqliteTest1>();
    } else if (engineModel == Engine_Mysql) {
        runSetInsertTest<MysqlTest1>();
    } else if (engineModel == Engine_SqlServer) {
        runSetInsertTest<SqlServerTest1>();
    }
}

template<typename E>
void runSetInsertBatchTest() {
    auto ids = QList<qint64>() << 10 << 11;  //auto increment id set value will not work 
    auto names = QStringList() << "name1" << "name2";
    auto numbers = QList<int>() << 4 << 50;

    typename E::Fields sf2;
    dao::_insert<E>()
        .set(sf2.id = ids, sf2.number = numbers, sf2.name = names) //set value with sequence independent
        .build().insert();

    auto query = BaseQuery::queryPrimitive(QString("select *from %1").arg(E::Info::getTableName()));
    QList<qint64> idsRes;
    QStringList namesRes;
    QList<int> numbersRes;
    while (query.next()) {
        idsRes << query.value("id").toLongLong();
        namesRes << query.value("name").toString();
        numbersRes << query.value("number").toInt();
    }
    QCOMPARE(idsRes, QList<qint64>() << 1 << 2);
    QCOMPARE(namesRes, names);
    QCOMPARE(numbersRes, numbers);
}

void InsertTest::setInsertBatchTest() {
    if (engineModel == Engine_Sqlite) {
        runSetInsertBatchTest<SqliteTest2>();
    } else if (engineModel == Engine_Mysql) {
        runSetInsertBatchTest<MysqlTest2>();
    } else if (engineModel == Engine_SqlServer) {
        runSetInsertBatchTest<SqlServerTest2>();
    }
}

void InsertTest::insertObjectTest_data() {
    if (engineModel == Engine_Sqlite) {
        QTest::addColumn<SqliteTest1>("test1");
        QTest::addColumn<SqliteTest1>("test2");
        QTest::addColumn<SqliteTest2>("test3");
        QTest::newRow("sqlite test data")
            << SqliteTest1(2, "test", 40, "object")
            << SqliteTest1(2, "test", 20, "abc")
            << SqliteTest2("testname", 23, 233, QDate(2020, 10, 20));
    } else if (engineModel == Engine_Mysql) {
        QTest::addColumn<MysqlTest1>("test1");
        QTest::addColumn<MysqlTest1>("test2");
        QTest::addColumn<MysqlTest2>("test3");
        QTest::newRow("mysql test data")
            << MysqlTest1(2, "test", 40, "object")
            << MysqlTest1(2, "test", 20, "abc")
            << MysqlTest2("testname", 23, 233);
    } else if (engineModel == Engine_SqlServer) {
        QTest::addColumn<SqlServerTest1>("test1");
        QTest::addColumn<SqlServerTest1>("test2");
        QTest::addColumn<SqlServerTest2>("test3");
        QTest::newRow("sqlserver test data")
            << SqlServerTest1(2, "test", 40, "object")
            << SqlServerTest1(2, "test", 20, "abc")
            << SqlServerTest2("testname", 23, 233);
    }
}

template<typename E1, typename E2>
void runInsertObjectTest() {
    QFETCH(E1, test1);
    dao::_insert<E1>().build().insert(test1);
    QCOMPARE(test1.getId(), 2);

    {
        auto query = BaseQuery::queryPrimitive(QString("select *from %1 where id = 2").arg(E1::Info::getTableName()));
        QVariantList data;
        if (query.next()) {
            for (const auto &f: E1::Info::getFieldsWithoutAutoIncrement()) {
                data << query.value(f);
            }
        }
        QCOMPARE(data, E1::Tool::getValueWithoutAutoIncrement(test1));
    }

    //test multi primary key insert
    QFETCH(E1, test2);
    try {
        dao::_insert<E1>().disableFatalMsg().build().insert(test2);
        QFAIL("insert should be fail!");
    }
    catch (DaoException&) {
    }

    QFETCH(E2, test3);
    dao::_insert<E2>().build().insert(test3);
    QCOMPARE(test3.getId(), 3);

    {
        auto query = BaseQuery::queryPrimitive(QString("select *from %1 where id = 3").arg(E2::Info::getTableName()));
        QVariantList data;
        if (query.next()) {
            for (const auto &f: E2::Info::getFieldsWithoutAutoIncrement()) {
                data << query.value(f);
            }
        }
        QCOMPARE(data, E2::Tool::getValueWithoutAutoIncrement(test3));
    }
}

void InsertTest::insertObjectTest() {
    if (engineModel == Engine_Sqlite) {
        runInsertObjectTest<SqliteTest1, SqliteTest2>();
    } else if (engineModel == Engine_Mysql) {
        runInsertObjectTest<MysqlTest1, MysqlTest2>();
    } else if (engineModel == Engine_SqlServer) {
        runInsertObjectTest<SqlServerTest1, SqlServerTest2>();
    }
}

void InsertTest::insertObjectsTest_data() {
    if (engineModel == Engine_Sqlite) {
        QList<SqliteTest2> test;
        test << SqliteTest2("name1", 1, 11, 111);
        test << SqliteTest2("name2", 2, 22, 22.2);
        test << SqliteTest2("name3", 3, 33, "333");

        QTest::addColumn<QList<SqliteTest2>>("test");
        QTest::newRow("sqlite test data") << test;
    } else if (engineModel == Engine_Mysql) {
        QList<MysqlTest2> test;
        test << MysqlTest2("name1", 1, 11);
        test << MysqlTest2("name2", 2, 22);
        test << MysqlTest2("name3", 3, 33);

        QTest::addColumn<QList<MysqlTest2>>("test");
        QTest::newRow("mysql test data") << test;
    } else if (engineModel == Engine_SqlServer) {
        QList<SqlServerTest2> test;
        test << SqlServerTest2("name1", 1, 11);
        test << SqlServerTest2("name2", 2, 22);
        test << SqlServerTest2("name3", 3, 33);

        QTest::addColumn<QList<SqlServerTest2>>("test");
        QTest::newRow("sqlserver test data") << test;
    }
}

template<typename E>
void runInsertObjectsTest() {
    QFETCH(QList<E>, test);
    dao::_insert<E>().build().insert(test);

    auto query = BaseQuery::queryPrimitive(QString("select *from %1 where id in (4, 5, 6)").arg(E::Info::getTableName()));
    QList<QVariantList> data;
    while (query.next()) {
        QVariantList d;
        for (const auto& f : E::Info::getFieldsWithoutAutoIncrement()) {
            d << query.value(f);
        }
        data << d;
    }
    QList<QVariantList> excepted;
    for (const auto& t : test) {
        excepted << E::Tool::getValueWithoutAutoIncrement(t);
    }
    QCOMPARE(data, excepted);
}

void InsertTest::insertObjectsTest() {
    if (engineModel == Engine_Sqlite) {
        runInsertObjectsTest<SqliteTest2>();
    } else if (engineModel == Engine_Mysql) {
        runInsertObjectsTest<MysqlTest2>();
    } else if (engineModel == Engine_SqlServer) {
        runInsertObjectsTest<SqlServerTest2>();
    }
}

void InsertTest::insertObjects2Test_data() {
    if (engineModel == Engine_Sqlite) {
        QList<SqliteTest2> test;
        test << SqliteTest2("name4", 4, 44, 444);
        test << SqliteTest2("name5", 5, 55, 55.2);
        test << SqliteTest2("name6", 6, 66, "666");

        QTest::addColumn<QList<SqliteTest2>>("test");
        QTest::newRow("sqlite test data") << test;
    } else if (engineModel == Engine_Mysql) {
        QList<MysqlTest2> test;
        test << MysqlTest2("name4", 4, 44);
        test << MysqlTest2("name5", 5, 55);
        test << MysqlTest2("name6", 6, 66);

        QTest::addColumn<QList<MysqlTest2>>("test");
        QTest::newRow("mysql test data") << test;
    } else if (engineModel == Engine_SqlServer) {
        QList<SqlServerTest2> test;
        test << SqlServerTest2("name4", 4, 44);
        test << SqlServerTest2("name5", 5, 55);
        test << SqlServerTest2("name6", 6, 66);

        QTest::addColumn<QList<SqlServerTest2>>("test");
        QTest::newRow("sqlserver test data") << test;
    }
}

template<typename E>
void runInsertObjects2Test() {
    QFETCH(QList<E>, test);
    dao::_insert<E>().build().insert2(test);

    auto query = BaseQuery::queryPrimitive(QString("select *from %1 where id in (7, 8, 9)").arg(E::Info::getTableName()));
    QList<QVariantList> data;
    while (query.next()) {
        QVariantList d;
        for (const auto& f : E::Info::getFieldsWithoutAutoIncrement()) {
            d << query.value(f);
        }
        data << d;
    }
    QList<QVariantList> excepted;
    for (const auto& t : test) {
        excepted << E::Tool::getValueWithoutAutoIncrement(t);
    }
    QCOMPARE(data, excepted);
}

void InsertTest::insertObjects2Test() {
    if (engineModel == Engine_Sqlite) {
        runInsertObjects2Test<SqliteTest2>();
    } else if (engineModel == Engine_Mysql) {
        runInsertObjects2Test<MysqlTest2>();
    } else if (engineModel == Engine_SqlServer) {
        runInsertObjects2Test<SqlServerTest2>();
    }
}

void InsertTest::insertOrReplaceTest_data() {
    if (engineModel == Engine_Sqlite) {
        QTest::addColumn<SqliteTest2>("entity");
        QTest::addRow("sqlite test data") << SqliteTest2("testinsertorreplace", -1, -2, "666");
    } else if (engineModel == Engine_Mysql) {
        QTest::addColumn<MysqlTest2>("entity");
        QTest::addRow("mysql test data") << MysqlTest2("testinsertorreplace", -1, -2);
    }
}

template<typename E>
void runInsertOrReplaceTest() {
    QFETCH(E, entity);
    dao::_insert<E>().build().insertOrReplace(entity);

    entity.setNumber2(100);
    dao::_insert<E>().build().insertOrReplace(entity);
    auto query = BaseQuery::queryPrimitive(QString("select * from %1 where name = 'testinsertorreplace'").arg(E::Info::getTableName()));
    int count = 0;
    int number2 = 0;
    while (query.next()) {
        count++;
        number2 = query.value("number2").toInt();
    }
    QCOMPARE(count, 1);
    QCOMPARE(number2, 100);
}

void InsertTest::insertOrReplaceTest() {
    PASSSQLSERVER;
    if (engineModel == Engine_Sqlite) {
        runInsertOrReplaceTest<SqliteTest2>();
    } else if (engineModel == Engine_Mysql) {
        runInsertOrReplaceTest<MysqlTest2>();
    }
}

void InsertTest::insertOrIgnoreTest_data() {
    if (engineModel == Engine_Sqlite) {
        QTest::addColumn<SqliteTest2>("entity");
        QTest::addRow("sqlite test data") << SqliteTest2("testinsertorreplace", -1, -2, "666");
    } else if (engineModel == Engine_Mysql) {
        QTest::addColumn<MysqlTest2>("entity");
        QTest::addRow("mysql test data") << MysqlTest2("testinsertorreplace", -1, -2);
    }
}

template<typename E>
void runInsertOrIgnoreTest() {
    QFETCH(E, entity);
    dao::_insert<E>().build().insertOrIgnore(entity);

    entity.setNumber2(150);
    dao::_insert<E>().build().insertOrIgnore(entity);
    auto query = BaseQuery::queryPrimitive(QString("select * from %1 where name = 'testinsertorreplace'").arg(E::Info::getTableName()));
    int count = 0;
    int number2 = 0;
    while (query.next()) {
        count++;
        number2 = query.value("number2").toInt();
    }
    QCOMPARE(count, 1);
    QCOMPARE(number2, 100);
}

void InsertTest::insertOrIgnoreTest() {
    PASSSQLSERVER;
    if (engineModel == Engine_Sqlite) {
        runInsertOrIgnoreTest<SqliteTest2>();
    } else if (engineModel == Engine_Mysql) {
        runInsertOrIgnoreTest<MysqlTest2>();
    }
}

void InsertTest::testTranscation_data() {
    if (engineModel == Engine_Sqlite) {
        QTest::addColumn<SqliteTest2>("entity");
        QTest::addRow("sqlite test data") << SqliteTest2("test transcation", 10000, 10000, "666");
    } else if (engineModel == Engine_Mysql) {
        QTest::addColumn<MysqlTest2>("entity");
        QTest::addRow("mysql test data") << MysqlTest2("test transcation", 10000, 10000);
    } else if (engineModel == Engine_SqlServer) {
        QTest::addColumn<SqlServerTest2>("entity");
        QTest::addRow("sqlserver test data") << SqlServerTest2("test transcation", 10000, 10000);
    }
}

template<typename E>
void runTestTranscation() {
    dao::transcation();
    QFETCH(E, entity);
    dao::_insert<E>().build().insert(entity);
    try {
        dao::_insert<E>().disableFatalMsg().build().insert(entity);
        dao::commit();
    }
    catch (DaoException&) {
        dao::rollback();
    }
    typename E::Fields sf;
    int count = dao::_count<E>().filter(sf.name == "test transcation").count();
    QCOMPARE(count, 0);
}

void InsertTest::testTranscation() {
    if (engineModel == Engine_Sqlite) {
        runTestTranscation<SqliteTest2>();
    } else if (engineModel == Engine_Mysql) {
        runTestTranscation<MysqlTest2>();
    } else if (engineModel == Engine_SqlServer) {
        runTestTranscation<SqlServerTest2>();
    }
}

void InsertTest::testMysqlMyISAMTranscation_data() {
    PASSSQLITE;
    PASSSQLSERVER;
    QTest::addColumn<MysqlTest1>("data1");
    QTest::addColumn<MysqlTest1>("data2");
    QTest::newRow("mysql test data")
        << MysqlTest1(10, "abc", 2, "")
        << MysqlTest1(10, QString(), 2, "");
}

void InsertTest::testMysqlMyISAMTranscation() {
    PASSSQLITE;
    PASSSQLSERVER;
    QFETCH(MysqlTest1, data1);
    QFETCH(MysqlTest1, data2);
    dao::transcation();
    dao::_insert<MysqlTest1>().build().insert(data1);
    try {
        dao::_insert<MysqlTest1>().disableFatalMsg().build().insert(data2); //null of name will case error
        dao::commit();
    } catch (DaoException&) {
        dao::rollback();
    }
    MysqlTest1::Fields sf;
    int count = dao::_count<MysqlTest1>().filter(sf.id == 10).count();
    QCOMPARE(count, 1); //MyISAM engine not support transcation
}

void InsertTest::cleanup() {
    clearCacheAndPrintIfTestFail();
}

void InsertTest::cleanupTestCase() {
    ConnectionPool::release();
    globalConfig->getClient()->dropDatabase();
}

