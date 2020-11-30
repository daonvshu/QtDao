#include "..\..\test\query\InsertTest.h"

#include <qtest.h>

#include "../sqliteentity/SqliteTest1.h"
#include "../sqliteentity/SqliteTest2.h"

#include "../../src/dao.h"

void InsertTest::initTestCase() {
    DbLoader::init(":/QtDao/test/sqliteentity/sqlite_cfg.xml");
}

void InsertTest::setInsertTest() {
    SqliteTest1::Fields sf1;
    bool success = dao::_insert<SqliteTest1>()
        .set(sf1.name = "test", sf1.number = 1)
        .build().insert();

    if (success) {
        BaseQuery::queryPrimitive(QString("select *from %1").arg(SqliteTest1::Info::getTableName())
            , [&](QSqlQuery& query) {
            QVariantList names, numbers;
            while (query.next()) {
                names << query.value("name");
                numbers << query.value("number");
            } 
            QCOMPARE(names, QVariantList() << "test");
            QCOMPARE(numbers, QVariantList() << 1);
        }
            , [&](QString err) {
            QFAIL(("test set insert fail! " + err).toUtf8());
        });
    } else {
        QFAIL("set insert fail!");
    }
}

void InsertTest::setInsertBatchTest() {
    SqliteTest2::Fields sf2;
    auto ids = QList<qint64>() << 10 << 11;  //auto increment id set value will not work 
    auto names = QStringList() << "name1" << "name2";
    auto numbers = QList<int>() << 4 << 50;
    bool success = dao::_insert<SqliteTest2>()
        .set(sf2.id = ids, sf2.number = numbers, sf2.name = names) //set value with sequence independent
        .build().insert();

    if (success) {
        BaseQuery::queryPrimitive(QString("select *from %1").arg(SqliteTest2::Info::getTableName())
            , [&](QSqlQuery& query) {
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
            , [&](QString err) {
            QFAIL(("test set insert batch fail! " + err).toUtf8());
        });
    } else {
        QFAIL("set insert batch fail!");
    }
}

void InsertTest::insertObjectTest() {
    SqliteTest1 test1(2, "test", 40, "object");
    bool success = dao::_insert<SqliteTest1>().build().insert(test1);
    QVERIFY(success);
    QCOMPARE(test1.getId(), 2);

    if (success) {
        BaseQuery::queryPrimitive(QString("select *from %1 where id = 2").arg(SqliteTest1::Info::getTableName())
            , [&](QSqlQuery& query) {
            QVariantList data;
            if (query.next()) {
                for (const auto& f : SqliteTest1::Info::getFieldsWithoutAutoIncrement()) {
                    data << query.value(f);
                }
            }
            QCOMPARE(data, SqliteTest1::Tool::getValueWithoutAutoIncrement(test1));
        }
            , [&](QString err) {
            QFAIL(("test insert object fail! " + err).toUtf8());
        });
    }

    //test multi primary key insert
    SqliteTest1 test2(2, "test", 20, "abc");
    try {
        dao::_insert<SqliteTest1>().throwable().build().insert(test2);
        QFAIL("insert should be fail!");
    }
    catch (DaoException&) {
    }

    SqliteTest2 test3(10, "testname", 23, 233, QDate(2020, 10, 20));
    success = dao::_insert<SqliteTest2>().build().insert(test3);
    QVERIFY(success);
    QCOMPARE(test3.getId(), 3);
    if (success) {
        BaseQuery::queryPrimitive(QString("select *from %1 where id = 3").arg(SqliteTest2::Info::getTableName())
            , [&](QSqlQuery& query) {
            QVariantList data;
            if (query.next()) {
                for (const auto& f : SqliteTest2::Info::getFieldsWithoutAutoIncrement()) {
                    data << query.value(f);
                }
            }
            QCOMPARE(data, SqliteTest2::Tool::getValueWithoutAutoIncrement(test3));
        }
            , [&](QString err) {
            QFAIL(("test insert object fail! " + err).toUtf8());
        });
    }
}

void InsertTest::insertObjectsTest() {
    SqliteTest2List test;
    test << SqliteTest2(0, "name1", 1, 11, 111);
    test << SqliteTest2(0, "name2", 2, 22, 22.2);
    test << SqliteTest2(0, "name3", 3, 33, "333");
    bool success = dao::_insert<SqliteTest2>().build().insert(test);
    QVERIFY(success);
    if (success) {
        BaseQuery::queryPrimitive(QString("select *from %1 where id in (4, 5, 6)").arg(SqliteTest2::Info::getTableName())
            , [&](QSqlQuery& query) {
            QList<QVariantList> data;
            while (query.next()) {
                QVariantList d;
                for (const auto& f : SqliteTest2::Info::getFieldsWithoutAutoIncrement()) {
                    d << query.value(f);
                }
                data << d;
            }
            QList<QVariantList> excepted;
            for (const auto& t : test) {
                excepted << SqliteTest2::Tool::getValueWithoutAutoIncrement(t);
            }
            QCOMPARE(data, excepted);
        }
            , [&](QString err) {
            QFAIL(("test insert objects fail! " + err).toUtf8());
        });
    }
}

void InsertTest::insertObjects2Test() {
    SqliteTest2List test;
    test << SqliteTest2(0, "name4", 4, 44, 444);
    test << SqliteTest2(0, "name5", 5, 55, 55.2);
    test << SqliteTest2(0, "name6", 6, 66, "666");
    bool success = dao::_insert<SqliteTest2>().build().insert2(test);
    QVERIFY(success);
    if (success) {
        BaseQuery::queryPrimitive(QString("select *from %1 where id in (7, 8, 9)").arg(SqliteTest2::Info::getTableName())
            , [&](QSqlQuery& query) {
            QList<QVariantList> data;
            while (query.next()) {
                QVariantList d;
                for (const auto& f : SqliteTest2::Info::getFieldsWithoutAutoIncrement()) {
                    d << query.value(f);
                }
                data << d;
            }
            QList<QVariantList> excepted;
            for (const auto& t : test) {
                excepted << SqliteTest2::Tool::getValueWithoutAutoIncrement(t);
            }
            QCOMPARE(data, excepted);
        }
            , [&](QString err) {
            QFAIL(("test insert objects 2 fail! " + err).toUtf8());
        });
    }
}

void InsertTest::cleanup() {

}

void InsertTest::cleanupTestCase() {
    ConnectionPool::release();
    DbLoader::getClient().dropDatabase();
}
