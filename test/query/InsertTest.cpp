#include "..\..\test\query\InsertTest.h"

#include <qtest.h>

#include "../sqliteentity/SqliteTest1.h"
#include "../sqliteentity/SqliteTest2.h"

#include "../../src/dao.h"

void InsertTest::initTestCase() {
    DbLoader::init(":/QtDao/test/sqliteentity/sqlite_cfg.xml");
    DbLoader::getClient().testConnect();
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
            QFAIL(("set insert fail! " + err).toUtf8());
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
            QFAIL(("set insert batch fail! " + err).toUtf8());
        });
    } else {
        QFAIL("set insert batch fail!");
    }
}

void InsertTest::cleanup() {

}

void InsertTest::cleanupTestCase() {
    ConnectionPool::release();
    DbLoader::getClient().dropDatabase();
}
