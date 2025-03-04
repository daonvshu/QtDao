#include "clientsqlservertest.h"

#include "sqlserver/testtb5.h"
#include "sqlserver/testtb6.h"
#include "utils/testconfigloader.h"

#include <dao.h>
#include <qtest.h>

void ClientSqlServerTest::initTestCase() {
    const auto& configOption = TestConfigLoader::instance().config().optionSqlServer();
    dao::_config<dao::ConfigSqlServerBuilder>()
            .version(1)
            .databaseName("sqlservertest")
            .host(configOption.host())
            .user(configOption.user())
            .password(configOption.password())
            .options(configOption.options())
            .disableCreateTables()
            .disableCreateDatabase()
            .initializeDatabase();

    client = dao::ConfigManager::getConfig()->getClient();
}

void ClientSqlServerTest::cleanup() {
    client->dropTable(TestTb5::Info::getTableName());
    QVERIFY(!client->checkTableExist(TestTb5::Info::getTableName()));

    client->dropTable(TestTb6::Info::getTableName());
    QVERIFY(!client->checkTableExist(TestTb6::Info::getTableName()));
}

void ClientSqlServerTest::cleanupTestCase() {
    client->dropDatabase();
}

void ClientSqlServerTest::databaseProcessTest() {
    client->dropDatabase();
    client->createDatabase();

    dao::BaseQuery::queryPrimitive("select 1");
}

void ClientSqlServerTest::tableProcessTest() {
    auto currentTables = client->exportAllTables();
    QCOMPARE(currentTables, {});

    QVERIFY(!client->checkTableExist(TestTb5::Info::getTableName()));

    //create test
    auto testTb3Reader = new dao::SqlServerEntityReaderProvider<TestTb5>;
    client->createTableIfNotExist(testTb3Reader);
    delete testTb3Reader;
    QVERIFY(client->checkTableExist(TestTb5::Info::getTableName()));

    client->createTableIfNotExist(
            "TestTb6",
            { "id bigint", "name varchar(100)", "number real default 10.0" },
            { "id", "name" },
            {},
            "");
    QVERIFY(client->checkTableExist("TestTb6"));

    currentTables = client->exportAllTables();
    std::sort(currentTables.begin(), currentTables.end());
    QStringList expect;
    expect << "TestTb6" << "ts_testtb5";
    QCOMPARE(currentTables, expect);

    //rename test
    client->renameTable("TestTb6", "newtesttb");
    currentTables = client->exportAllTables();
    std::sort(currentTables.begin(), currentTables.end());
    expect.clear();
    expect << "newtesttb" << "ts_testtb5";
    QCOMPARE(currentTables, expect);

    //drop test
    client->dropTable("newtesttb");
    currentTables = client->exportAllTables();
    expect.clear();
    expect << "ts_testtb5";
    QCOMPARE(currentTables, expect);

    //truncate test
    TestTb5List data;
    data << TestTb5("f1", 1, 10, "f10");
    data << TestTb5("f2", 2, 11, "f11");
    data << TestTb5("f3", 3, 12, "f12");
    data << TestTb5("f4", 4, 13, "f13");
    dao::_insert<TestTb5>().build().insert2(data);

    TestTb5::Fields tf;
    auto fv = dao::_select<TestTb5>()
            .column(tf.field1)
            .with(_orderBy(tf.field1))
            .build()
            .list(tf.field1);
    QList<qint64> expectV = { 1, 2, 3, 4 };
    QCOMPARE(fv, expectV);

    client->truncateTable(TestTb5::Info::getTableName());
    dao::_insert<TestTb5>().build().insert2(data);

    fv = dao::_select<TestTb5>()
            .column(tf.field1)
            .with(_orderBy(tf.field1))
            .build()
            .list(tf.field1);
    QCOMPARE(fv, expectV);
}

void ClientSqlServerTest::indexProcessTest() {
    auto testTb3Reader = new dao::SqlServerEntityReaderProvider<TestTb5>;
    client->createTableIfNotExist(testTb3Reader);
    client->createIndex(testTb3Reader);
    delete testTb3Reader;

    //get field name test
    QStringList usedField1 = { "f1", "[after]", "[by]" };
    auto indexName = client->getIndexFromFields("", usedField1);
    QCOMPARE(indexName, "index_f1_after_by");

    QList<QStringList> usedField2;
    usedField2 << (QStringList() << "[case]" << "field3");
    usedField2 << (QStringList() << "field4" << "field10");
    auto indexNames = client->getIndexArrayFromFields("", usedField2);
    QStringList expectNames = { "index_case_field3", "index_field4_field10" };
    QCOMPARE(indexNames, expectNames);

    //export test
    auto indexes = client->exportAllIndexes(TestTb5::Info::getTableName());

    auto normalIndex = indexes[dao::IndexType::INDEX_NONCLUSTERED];
    std::sort(normalIndex.begin(), normalIndex.end());
    QStringList expectIndex = { "index_field2", "index_field3_field4" };
    QCOMPARE(normalIndex, expectIndex);

    auto uniqueIndex = indexes[dao::IndexType::INDEX_UNIQUE_NONCLUSTERED];
    std::sort(uniqueIndex.begin(), uniqueIndex.end());
    QStringList expectUIndex = { "index_field2_field4" };
    QCOMPARE(uniqueIndex, expectUIndex);

    //drop index test
    client->dropIndex(TestTb5::Info::getTableName(), QStringList() << "field3" << "field4");
    indexes = client->exportAllIndexes(TestTb5::Info::getTableName());
    normalIndex = indexes[dao::IndexType::INDEX_NONCLUSTERED];
    QStringList expectDropIndex1 = { "index_field2" };
    QCOMPARE(normalIndex, expectDropIndex1);

    client->dropIndex(TestTb5::Info::getTableName(), "index_field2_field4");
    indexes = client->exportAllIndexes(TestTb5::Info::getTableName());
    uniqueIndex = indexes[dao::IndexType::INDEX_UNIQUE_NONCLUSTERED];
    QVERIFY(uniqueIndex.isEmpty());

    //create index test
    client->createIndex(TestTb5::Info::getTableName(),
                        QStringList() << "field3" << "field4",
                        dao::IndexType::INDEX_NONCLUSTERED);
    indexes = client->exportAllIndexes(TestTb5::Info::getTableName());
    normalIndex = indexes[dao::IndexType::INDEX_NONCLUSTERED];
    std::sort(normalIndex.begin(), normalIndex.end());
    QStringList expectIndex2 = { "index_field2", "index_field3_field4" };
    QCOMPARE(normalIndex, expectIndex2);

    client->createIndex(TestTb5::Info::getTableName(),
                        "index_of_f4",
                        QStringList() << "field4",
                        dao::IndexType::INDEX_NONCLUSTERED,
                        nullptr);
    indexes = client->exportAllIndexes(TestTb5::Info::getTableName());
    normalIndex = indexes[dao::IndexType::INDEX_NONCLUSTERED];
    std::sort(normalIndex.begin(), normalIndex.end());
    QStringList expectIndex3 = { "index_field2", "index_field3_field4", "index_of_f4" };
    QCOMPARE(normalIndex, expectIndex3);

    //drop all
    client->dropAllIndexOnTable(TestTb5::Info::getTableName());
    indexes = client->exportAllIndexes(TestTb5::Info::getTableName());
    normalIndex = indexes[dao::IndexType::INDEX_NONCLUSTERED];
    QVERIFY(normalIndex.isEmpty());
    uniqueIndex = indexes[dao::IndexType::INDEX_UNIQUE_NONCLUSTERED];
    QVERIFY(uniqueIndex.isEmpty());
}

void ClientSqlServerTest::fieldProcessTest() {
    auto testTb3Reader = new dao::SqlServerEntityReaderProvider<TestTb5>;
    client->createTableIfNotExist(testTb3Reader);
    client->createIndex(testTb3Reader);
    delete testTb3Reader;

    auto fields = client->exportAllFields(TestTb5::Info::getTableName());
    QList<QPair<QString, QString>> expectFields;
    expectFields << qMakePair(QLatin1String("[field1]"), QLatin1String("BIGINT"));
    expectFields << qMakePair(QLatin1String("[field2]"), QLatin1String("VARCHAR"));
    expectFields << qMakePair(QLatin1String("[field3]"), QLatin1String("INT"));
    expectFields << qMakePair(QLatin1String("[field4]"), QLatin1String("INT"));
    expectFields << qMakePair(QLatin1String("[field5]"), QLatin1String("TEXT"));
    QCOMPARE(fields, expectFields);

    //drop all user index before drop columns
    client->dropAllIndexOnTable(TestTb5::Info::getTableName());

    client->dropField(TestTb5::Info::getTableName(), "field1"); //primary key constraint

    client->dropField(TestTb5::Info::getTableName(), "field2"); //not null constraint

    client->dropField(TestTb5::Info::getTableName(), "field3"); //unique constraint

    client->dropField(TestTb5::Info::getTableName(), "field4"); //default constraint

    client->addField(TestTb5::Info::getTableName(), "field10 int not null default 1");

    client->renameField(TestTb5::Info::getTableName(), "field10", "field20");

    fields = client->exportAllFields(TestTb5::Info::getTableName());
    std::sort(fields.begin(), fields.end(), [](const QPair<QString, QString>& a, const QPair<QString, QString>& b) {
        return a.first < b.first;
    });
    expectFields.clear();
    expectFields << qMakePair(QLatin1String("[field20]"), QLatin1String("INT"));
    expectFields << qMakePair(QLatin1String("[field5]"), QLatin1String("TEXT"));
    QCOMPARE(fields, expectFields);
}

void ClientSqlServerTest::dataTransTest() {
    auto testTb3Reader = new dao::SqlServerEntityReaderProvider<TestTb5>;
    client->createTableIfNotExist(testTb3Reader);
    delete testTb3Reader;

    auto testTb4Reader = new dao::SqlServerEntityReaderProvider<TestTb6>;
    client->createTableIfNotExist(testTb4Reader);
    client->createIndex(testTb4Reader);
    delete testTb4Reader;

    TestTb5List testData;
    testData << TestTb5("fv2_1", 1, 11, "fv6_1");
    testData << TestTb5("fv2_2", 2, 22, "fv6_2");
    testData << TestTb5("fv2_3", 3, 33, "fv6_3");
    dao::_insert<TestTb5>().build().insert2(testData);

    client->transferData(TestTb5::Info::getTableName(), TestTb6::Info::getTableName());

    TestTb6::Fields tf2;
    auto data2 = dao::_select<TestTb6>().build().list(tf2.field3, tf2.field4, tf2.fieldEx);

    QList<std::tuple<int, int, int>> expectV2;
    expectV2 << std::make_tuple(1, 11, 20);
    expectV2 << std::make_tuple(2, 22, 20);
    expectV2 << std::make_tuple(3, 33, 20);
    QCOMPARE(data2, expectV2);
}