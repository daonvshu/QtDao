#include "UpsertTest.h"

#include <QtTest>

#include "entity/sqliteentity/sqlitetest1.h"
#include "entity/sqliteentity/sqlitetest2.h"

#include "entity/mysqlentity/mysqltest1.h"
#include "entity/mysqlentity/mysqltest2.h"

#include "entity/sqlserverentity/sqlservertest1.h"
#include "entity/sqlserverentity/sqlservertest2.h"

void UpsertTest::initTestCase() {
    configDb();
}

template<typename E>
void runSetUpsertTest() {
    typename E::Fields sf2;
    dao::_insert<E>()
            .set(sf2.name = "testName1", sf2.number = 20, sf2.number2 = 10)
            .build().insert();

    const auto& compareResult = [] (const QVariantList& expectNames, const QVariantList& expectNumbers
            , const QVariantList& expectNumber2s) {
        auto query = BaseQuery::queryPrimitive(QString("select *from %1").arg(E::Info::getTableName()));
        QVariantList names, numbers, number2s;
        while (query.next()) {
            names << query.value("name");
            numbers << query.value("number");
            number2s << query.value("number2");
        }
        QCOMPARE(names, expectNames);
        QCOMPARE(numbers, expectNumbers);
        QCOMPARE(number2s, expectNumber2s);
    };

    //not conflict with insert
    {
        dao::_insertOrUpdate<E>()
                .set(sf2.name = "testName2", sf2.number = 20, sf2.number2 = 50)
                .conflictColumns(sf2.name, sf2.number)
                .updateColumns(sf2.number2)
                .build().insert();

        compareResult(QVariantList() << "testName1" << "testName2", QVariantList() << 20 << 20,
                      QVariantList() << 10 << 50);
    }

    //conflict by unique index (name, number) with update field 'number2'
    {
        dao::_insertOrUpdate<E>()
                .set(sf2.name = "testName1", sf2.number = 20, sf2.number2 = 30)
                .conflictColumns(sf2.name, sf2.number)
                .updateColumns(sf2.number2)
                .build().insert();

        compareResult(QVariantList() << "testName1" << "testName2", QVariantList() << 20 << 20,
                      QVariantList() << 30 << 50);
    }

    //test optional update columns
    {
        dao::_insertOrUpdate<E>()
                .set(sf2.name = "testName1", sf2.number = 20, sf2.number2 = 60)
                .conflictColumns(sf2.name, sf2.number)
                .build().insert();

        compareResult(QVariantList() << "testName1" << "testName2", QVariantList() << 20 << 20,
                      QVariantList() << 60 << 50);
    }

    dao::_truncate<E>();
}

void UpsertTest::setUpsertTest() {
    if (targetDb == TestTargetDb::Target_Sqlite) {
        runSetUpsertTest<SqliteTest2>();
    } else if (targetDb == TestTargetDb::Target_Mysql) {
        runSetUpsertTest<MysqlTest2>();
    } else if (targetDb == TestTargetDb::Target_SqlServer) {
        runSetUpsertTest<SqlServerTest2>();
    }
}

template<typename E>
void runSetUpsertBatchTest() {
    auto ids = QList<qint64>() << 10 << 11;  //auto increment id set value will not work 
    auto names = QStringList() << "name1" << "name2";
    auto numbers = QList<int>() << 4 << 50;
    auto number2s = QList<int>() << 10 << 20;

    typename E::Fields sf2;
    dao::_insert<E>()
            .set(sf2.id = ids, sf2.number = numbers, sf2.name = names, sf2.number2 = number2s) //set value with sequence independent
            .build().insert();

    const auto& compareResult = [] (int idSize, const QStringList& expectNames, const QList<int>& expectNumbers,
            const QList<int>& expectNumber2s) {
        auto query = BaseQuery::queryPrimitive(QString("select *from %1").arg(E::Info::getTableName()));
        QList<qint64> idsRes;
        QStringList namesRes;
        QList<int> numbersRes;
        QList<int> number2sRes;
        while (query.next()) {
            idsRes << query.value("id").toLongLong();
            namesRes << query.value("name").toString();
            numbersRes << query.value("number").toInt();
            number2sRes << query.value("number2").toInt();
        }
        QVERIFY(idsRes.size() == idSize);
        QCOMPARE(namesRes, expectNames);
        QCOMPARE(numbersRes, expectNumbers);
        QCOMPARE(number2sRes, expectNumber2s);
    };


    numbers.clear();
    number2s.clear();

    numbers << 4 << 60; //col1 conflict with update; col2 not conflict with insert;
    number2s << 100 << 200;

    dao::_insertOrUpdate<E>()
            .set(sf2.number = numbers, sf2.name = names, sf2.number2 = number2s)
            .conflictColumns(sf2.name, sf2.number)
            .updateColumns(sf2.number2)
            .build().insert();

    compareResult(3, QStringList() << "name1" << "name2" << "name2",
                  QList<int>() << 4 << 50 << 60, QList<int>() << 100 << 20 << 200);

    //test optional update columns
    numbers.clear();
    number2s.clear();

    numbers << 4 << 70; //col1 conflict with update; col2 not conflict with insert;
    number2s << 1000 << 2000;

    dao::_insertOrUpdate<E>()
            .set(sf2.number = numbers, sf2.name = names, sf2.number2 = number2s)
            .conflictColumns(sf2.name, sf2.number)
            .build().insert();

    compareResult(4, QStringList() << "name1" << "name2" << "name2" << "name2",
                  QList<int>() << 4 << 50 << 60 << 70, QList<int>() << 1000 << 20 << 200 << 2000);

    dao::_truncate<E>();
}

void UpsertTest::setUpsertBatchTest() {
    if (targetDb == TestTargetDb::Target_Sqlite) {
        runSetUpsertBatchTest<SqliteTest2>();
    } else if (targetDb == TestTargetDb::Target_Mysql) {
        runSetUpsertBatchTest<MysqlTest2>();
    } else if (targetDb == TestTargetDb::Target_SqlServer) {
        runSetUpsertBatchTest<SqlServerTest2>();
    }
}

void UpsertTest::upsertObjectTest_data() {
    if (targetDb == TestTargetDb::Target_Sqlite) {
        QTest::addColumn<SqliteTest2>("test1");
        QTest::addColumn<SqliteTest2>("test2");
        QTest::addColumn<SqliteTest2>("test3");
        QTest::newRow("sqlite test data")
                << SqliteTest2("name1", 23, 233, QString("2022-10-20"))
                << SqliteTest2("name2", 23, 2333, 1)
                << SqliteTest2("name2", 23, 433, "aws");
    } else if (targetDb == TestTargetDb::Target_Mysql) {
        QTest::addColumn<MysqlTest2>("test1");
        QTest::addColumn<MysqlTest2>("test2");
        QTest::addColumn<MysqlTest2>("test3");
        QTest::newRow("mysql test data")
                << MysqlTest2("name1", 23, 233)
                << MysqlTest2("name2", 23, 2333)
                << MysqlTest2("name2", 23, 433);
    } else if (targetDb == TestTargetDb::Target_SqlServer) {
        QTest::addColumn<SqlServerTest2>("test1");
        QTest::addColumn<SqlServerTest2>("test2");
        QTest::addColumn<SqlServerTest2>("test3");
        QTest::newRow("sqlserver test data")
                << SqlServerTest2("name1", 23, 233)
                << SqlServerTest2("name2", 23, 2333)
                << SqlServerTest2("name2", 23, 433);
    }
}

template<typename E>
void bindUpdateColumn(UpsertBuilder<E>& builder) {
    typename E::Fields sf2;
    builder.updateColumns(sf2.number2);
}

template<>
void bindUpdateColumn(UpsertBuilder<SqliteTest2>& builder) {
    SqliteTest2::Fields sf2;
    builder.updateColumns(sf2.number2, sf2.varianttype);
}

template<typename E>
void runUpsertObjectTest() {
    typename E::Fields sf2;

    QFETCH(E, test1);
    dao::_insert<E>().build().insert(test1);

    {
        auto builder = dao::_insertOrUpdate<E>()
                .conflictColumns(sf2.name, sf2.number);
        bindUpdateColumn(builder);

        QFETCH(E, test2); //not conflict
        builder.build().insert(test2);

        QFETCH(E, test3); //conflict
        builder.build().insert(test3);

        //test only keep test1,test3
        auto entities = dao::_selectAll<E>();
        QVERIFY(entities.size() == 2);
        QCOMPARE(entities.at(0), test1);
        QCOMPARE(entities.at(1), test3);
    }

    //test optional update columns
    {
        dao::_delete<E>().filter(sf2.name == "name2").build().deleteBy();

        auto builder = dao::_insertOrUpdate<E>()
                .conflictColumns(sf2.name, sf2.number);

        QFETCH(E, test2); //not conflict
        builder.build().insert(test2);

        QFETCH(E, test3); //conflict
        builder.build().insert(test3);

        //test only keep test1,test3
        auto entities = dao::_selectAll<E>();
        QVERIFY(entities.size() == 2);
        QCOMPARE(entities.at(0), test1);
        QCOMPARE(entities.at(1), test3);
    }

    dao::_truncate<E>();
}

void UpsertTest::upsertObjectTest() {
    if (targetDb == TestTargetDb::Target_Sqlite) {
        runUpsertObjectTest<SqliteTest2>();
    } else if (targetDb == TestTargetDb::Target_Mysql) {
        runUpsertObjectTest<MysqlTest2>();
    } else if (targetDb == TestTargetDb::Target_SqlServer) {
        runUpsertObjectTest<SqlServerTest2>();
    }
}

void UpsertTest::upsertObjectsTest_data() {
    if (targetDb == TestTargetDb::Target_Sqlite) {
        QList<SqliteTest2> test;
        test << SqliteTest2("name1", 23, 233, QString("2022-10-20"));
        test << SqliteTest2("name2", 23, 2333, 1);
        test << SqliteTest2("name2", 23, 433, "aws");

        QTest::addColumn<QList<SqliteTest2>>("test");
        QTest::newRow("sqlite test data") << test;
    } else if (targetDb == TestTargetDb::Target_Mysql) {
        QList<MysqlTest2> test;
        test << MysqlTest2("name1", 23, 233);
        test << MysqlTest2("name2", 23, 2333);
        test << MysqlTest2("name2", 23, 433);

        QTest::addColumn<QList<MysqlTest2>>("test");
        QTest::newRow("mysql test data") << test;
    } else if (targetDb == TestTargetDb::Target_SqlServer) {
        QList<SqlServerTest2> test;
        test << SqlServerTest2("name1", 23, 233);
        test << SqlServerTest2("name2", 23, 2333);
        test << SqlServerTest2("name2", 23, 433);

        QTest::addColumn<QList<SqlServerTest2>>("test");
        QTest::newRow("sqlserver test data") << test;
    }
}

template<typename E>
void runUpsertObjectsTest() {
    QFETCH(QList<E>, test);

    typename E::Fields sf2;

    {
        auto builder = dao::_insertOrUpdate<E>()
                .conflictColumns(sf2.name, sf2.number);
        bindUpdateColumn(builder);

        builder.build().insert(test);

        //test only keep test1,test3
        auto entities = dao::_selectAll<E>();
        QVERIFY(entities.size() == 2);
        QCOMPARE(entities.at(0), test.at(0));
        QCOMPARE(entities.at(1), test.at(2));
    }

    //test optional update columns
    {
        dao::_truncate<E>();

        dao::_insertOrUpdate<E>()
            .conflictColumns(sf2.name, sf2.number)
            .build().insert(test);

        //test only keep test1,test3
        auto entities = dao::_selectAll<E>();
        QVERIFY(entities.size() == 2);
        QCOMPARE(entities.at(0), test.at(0));
        QCOMPARE(entities.at(1), test.at(2));
    }

    dao::_truncate<E>();
}

void UpsertTest::upsertObjectsTest() {
    if (targetDb == TestTargetDb::Target_Sqlite) {
        runUpsertObjectsTest<SqliteTest2>();
    } else if (targetDb == TestTargetDb::Target_Mysql) {
        runUpsertObjectsTest<MysqlTest2>();
    } else if (targetDb == TestTargetDb::Target_SqlServer) {
        runUpsertObjectsTest<SqlServerTest2>();
    }
}

void UpsertTest::upsertFilterTest() {
    PASSMYSQL;
    PASSSQLSERVER;

    auto ids = QList<qint64>() << 10 << 11;  //auto increment id set value will not work
    auto names = QStringList() << "name1" << "name2";
    auto numbers = QList<int>() << 4 << 50;
    auto number2s = QList<int>() << 10 << 20;

    SqliteTest2::Fields sf2;
    dao::_insert<SqliteTest2>()
        .set(sf2.id = ids, sf2.number = numbers, sf2.name = names, sf2.number2 = number2s) //set value with sequence independent
        .build().insert();

    const auto& compareResult = [] (int idSize, const QStringList& expectNames, const QList<int>& expectNumbers,
                                    const QList<int>& expectNumber2s) {
        auto query = BaseQuery::queryPrimitive(QString("select *from %1").arg(SqliteTest2::Info::getTableName()));
        QList<qint64> idsRes;
        QStringList namesRes;
        QList<int> numbersRes;
        QList<int> number2sRes;
        while (query.next()) {
            idsRes << query.value("id").toLongLong();
            namesRes << query.value("name").toString();
            numbersRes << query.value("number").toInt();
            number2sRes << query.value("number2").toInt();
        }
        QVERIFY(idsRes.size() == idSize);
        QCOMPARE(namesRes, expectNames);
        QCOMPARE(numbersRes, expectNumbers);
        QCOMPARE(number2sRes, expectNumber2s);
    };


    numbers.clear();
    number2s.clear();

    numbers << 4 << 50; //col1 conflict with update; col2 ignore by filter condition;
    number2s << 5 << 40;

    dao::UpsertExcluded<SqliteTest2>::Fields excluded;

    dao::_insertOrUpdate<SqliteTest2>()
        .set(sf2.number = numbers, sf2.name = names, sf2.number2 = number2s)
        .conflictColumns(sf2.name, sf2.number)
        .updateColumns(sf2.number2)
        .filter(excluded.number2 < sf2.number2)
        .build().insert();

    compareResult(2, QStringList() << "name1" << "name2",
                  QList<int>() << 4 << 50, QList<int>() << 5 << 20);

    dao::_truncate<SqliteTest2>();
}

void UpsertTest::cleanup() {
    clearCacheAndPrintIfTestFail();
}

void UpsertTest::cleanupTestCase() {
    ConfigManager::getConfig()->getClient()->dropDatabase();
}
