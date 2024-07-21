#include "upgradetest.h"

#include "dao.h"

#ifndef TEST_DB_CLEAR
#include "table1.h"
#include "table2.h"
#endif

#include "mydbupgrader.h"
#include "utils/testconfigloader.h"

#include <qtest.h>

void UpgradeTest::initTestCase() {

}

void UpgradeTest::cleanup() {

}

void UpgradeTest::cleanupTestCase() {

}

void UpgradeTest::upgradeTest() {
#ifdef TEST_DB_CLEAR
    if (TEST_DB == QLatin1String("sqlite")) {
        dao::_config<dao::ConfigSqliteBuilder>()
                .databaseName("sqlite_upgrade_test")
                .disableCreateTables()
                .initializeDatabase();
    } else if (TEST_DB == QLatin1String("mysql")) {
        const auto& configOption = TestConfigLoader::instance().config().optionMysql();
        dao::_config<dao::ConfigMysqlBuilder>()
                .databaseName("mysql_upgrade_test")
                .host(configOption.host())
                .port(configOption.port())
                .user(configOption.user())
                .password(configOption.password())
                .disableCreateTables()
                .initializeDatabase();
    } else if (TEST_DB == QLatin1String("sqlserver")) {
        const auto& configOption = TestConfigLoader::instance().config().optionSqlServer();
        dao::_config<dao::ConfigSqlServerBuilder>()
                .databaseName("sqlserver_upgrade_test")
                .host(configOption.host())
                .user(configOption.user())
                .password(configOption.password())
                .disableCreateTables()
                .initializeDatabase();
    }

    dao::ConfigManager::getConfig()->getClient()->dropDatabase();
#else
    if (TEST_DB == QLatin1String("sqlite")) {
        dao::_config<dao::ConfigSqliteBuilder>()
                .version(TARGET_VER)
                .databaseName("sqlite_upgrade_test")
                .setDatabaseUpgrader(new MyDbUpgrader)
                .initializeDatabase();
    } else if (TEST_DB == QLatin1String("mysql")) {
        const auto& configOption = TestConfigLoader::instance().config().optionMysql();
        dao::_config<dao::ConfigMysqlBuilder>()
                .version(TARGET_VER)
                .databaseName("mysql_upgrade_test")
                .host(configOption.host())
                .port(configOption.port())
                .user(configOption.user())
                .password(configOption.password())
                .setDatabaseUpgrader(new MyDbUpgrader)
                .initializeDatabase();
    } else if (TEST_DB == QLatin1String("sqlserver")) {
        const auto& configOption = TestConfigLoader::instance().config().optionSqlServer();
        dao::_config<dao::ConfigSqlServerBuilder>()
                .version(TARGET_VER)
                .databaseName("sqlserver_upgrade_test")
                .host(configOption.host())
                .user(configOption.user())
                .password(configOption.password())
                .setDatabaseUpgrader(new MyDbUpgrader)
                .initializeDatabase();
    }
#endif
}

void UpgradeTest::fieldTest() {
#ifndef TEST_DB_CLEAR
    auto expected = Table1::Info::getFields();
    std::sort(expected.begin(), expected.end());
    auto fields = dao::ConfigManager::getConfig()->getClient()->exportAllFields(Table1::Info::getTableName());
    std::sort(fields.begin(), fields.end());
    QStringList actualFields;
    for (const auto& f : fields) {
        actualFields << f.first;
    }
    QCOMPARE(actualFields, expected);

    expected = Table2::Info::getFields();
    std::sort(expected.begin(), expected.end());
    fields = dao::ConfigManager::getConfig()->getClient()->exportAllFields(Table2::Info::getTableName());
    std::sort(fields.begin(), fields.end());
    actualFields.clear();
    for (const auto& f : fields) {
        actualFields << f.first;
    }
    QCOMPARE(actualFields, expected);
#endif
}

template<int V>
void databaseDataTest();

#ifndef TEST_DB_CLEAR
template<>
void databaseDataTest<1>() {
    int dataSize = dao::_count<Table1>().count();
    if (dataSize == 0) {
        Table1List table1List;
        table1List << Table1(1, "Alice", 75);
        table1List << Table1(2, "Bob", 76);
        table1List << Table1(3, "Charlie", 77);
        table1List << Table1(4, "David", 78);
        table1List << Table1(5, "Emily", 79);
        dao::_insert<Table1>().build().insert2(table1List);
    }

    dataSize = dao::_count<Table2>().count();
    if (dataSize == 0) {
        Table2List table2List;
        table2List << Table2("001", "Alice", 75, 80);
        table2List << Table2("002", "Bob", 76, 81);
        table2List << Table2("003", "Charlie", 75, 80);
        table2List << Table2("004", "David", 76, 82);
        table2List << Table2("005", "Emily", 79, 85);
        dao::_insert<Table2>().build().insert2(table2List);
    }
}

template<>
void databaseDataTest<2>() {
    Table1List table1List;
    table1List << Table1(1, "Alice", 75);
    table1List << Table1(2, "Bob", 76);
    table1List << Table1(3, "Charlie", 77);
    table1List << Table1(4, "David", 78);
    table1List << Table1(5, "Emily", 79);

    auto tb1Data = dao::_selectAll<Table1>();
    QCOMPARE(tb1Data, table1List);

    Table2List table2List;
    if (TEST_DB == QLatin1String("sqlserver")) { //sqlserver adding column dose not set default value
        table2List << Table2("001", "Alice", 75, 0);
        table2List << Table2("002", "Bob", 76, 0);
        table2List << Table2("003", "Charlie", 75, 0);
        table2List << Table2("004", "David", 76, 0);
        table2List << Table2("005", "Emily", 79, 0);
    } else {
        table2List << Table2("001", "Alice", 75, 18);
        table2List << Table2("002", "Bob", 76, 18);
        table2List << Table2("003", "Charlie", 75, 18);
        table2List << Table2("004", "David", 76, 18);
        table2List << Table2("005", "Emily", 79, 18);
    }

    auto tb2Data = dao::_selectAll<Table2>();
    QCOMPARE(tb2Data, table2List);
}

template<>
void databaseDataTest<3>() {
    Table1List table1List;
    table1List << Table1(1, "Alice", 75);
    table1List << Table1(2, "Bob", 76);
    table1List << Table1(3, "Charlie", 77);
    table1List << Table1(4, "David", 78);
    table1List << Table1(5, "Emily", 79);

    auto tb1Data = dao::_selectAll<Table1>();
    QCOMPARE(tb1Data, table1List);

    Table2List table2List;
    if (TEST_DB == QLatin1String("sqlite")) { //default upgrade strategy will lost data column 'number'
        table2List << Table2("001", "Alice", 0, 18);
        table2List << Table2("002", "Bob", 0, 18);
        table2List << Table2("003", "Charlie", 0, 18);
        table2List << Table2("004", "David", 0, 18);
        table2List << Table2("005", "Emily", 0, 18);
    } else { //change column 'name' data type will lost data
        if (TEST_DB == QLatin1String("sqlserver")) { //sqlserver adding column dose not set default value
            table2List << Table2("001", QString(), 75, 0);
            table2List << Table2("002", QString(), 76, 0);
            table2List << Table2("003", QString(), 75, 0);
            table2List << Table2("004", QString(), 76, 0);
            table2List << Table2("005", QString(), 79, 0);
        } else {
            table2List << Table2("001", QString(), 75, 18);
            table2List << Table2("002", QString(), 76, 18);
            table2List << Table2("003", QString(), 75, 18);
            table2List << Table2("004", QString(), 76, 18);
            table2List << Table2("005", QString(), 79, 18);
        }
    }

    auto tb2Data = dao::_selectAll<Table2>();
    QCOMPARE(tb2Data, table2List);
}
#endif

void UpgradeTest::dataTest() {
#ifndef TEST_DB_CLEAR
    databaseDataTest<TARGET_VER>();
#endif
}
