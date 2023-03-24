#include "customtypetest.h"

#include <dao.h>
#include <qtest.h>

#include "entity/customtypes.h"

void CustomTypeTest::initTestCase() {
    try {
        dao::_config<dao::ConfigSqliteBuilder>()
                .version(1)
                .databaseName("sqlite_customtype_test")
                .initializeDatabase();

    } catch (dao::DaoException& e) {
        Q_UNUSED(e)
        auto validDrivers = QSqlDatabase::drivers();
        Q_UNUSED(validDrivers)
        qFatal("setup database fail!");
    }
}

void CustomTypeTest::testInsert() {

    CustomTypes typeInstance("test1", 0x8000, QDate::fromString("2022-02-22", "yyyy-MM-dd"),
                             {QRect(1, 1, 10, 10), QRect(10, 10, 100, 100)},
                             {
                                     {"key1", {MyStruct{"name1", 10, QDateTime::fromString("2022-02-23 12:22:23", "yyyy-MM-dd HH:mm:dd")}}},
                                     {"key2", {MyStruct{"name2", 20, QDateTime::fromString("2022-02-24 12:22:24", "yyyy-MM-dd HH:mm:dd")}}},
                             },
                             MyStruct{"name3", 30, QDateTime::fromString("2022-02-25 12:22:25", "yyyy-MM-dd HH:mm:dd")}
                             );

    dao::_insert<CustomTypes>().build().insert(typeInstance);

    CustomTypes::Fields cf;
    auto instance = dao::_select<CustomTypes>().filter(cf.testName == "test1").build().unique();
    QCOMPARE(instance, typeInstance);
}

void CustomTypeTest::testSelect() {
    auto customData = MyStruct{"name3", 30, QDateTime::fromString("2022-02-25 12:22:25", "yyyy-MM-dd HH:mm:dd")};
    CustomTypes::Fields cf;
    auto data = dao::_select<CustomTypes>().filter(cf.myStruct == customData).build().one();
    QCOMPARE(data.getTestName(), "test1");
}

void CustomTypeTest::cleanup() {

}

void CustomTypeTest::cleanupTestCase() {
    ConnectionPool::release();
    dao::globalConfig->getClient()->dropDatabase();
}