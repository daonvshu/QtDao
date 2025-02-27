#include "clientpsqltest.h"

#include "psql/testtb7.h"
#include "psql/testtb8.h"
#include "utils/testconfigloader.h"

#include <dao.h>
#include <qtest.h>

void ClientPSqlTest::initTestCase() {
    const auto &configOption = TestConfigLoader::instance().config().optionPSql();
    dao::_config<dao::ConfigPSqlBuilder>()
        .version(1)
        .databaseName("psql_client_test")
        .host(configOption.host())
        .port(configOption.port())
        .user(configOption.user())
        .password(configOption.password())
        .options(configOption.options())
        .disableCreateTables()
        .disableCreateDatabase()
        .initializeDatabase();

    client = dao::ConfigManager::getConfig()->getClient();
}

void ClientPSqlTest::cleanup() {
    client->dropTable(TestTb7::Info::getTableName());
    QVERIFY(!client->checkTableExist(TestTb7::Info::getTableName()));

    client->dropTable(TestTb8::Info::getTableName());
    QVERIFY(!client->checkTableExist(TestTb8::Info::getTableName()));
}

void ClientPSqlTest::cleanupTestCase() {
    client->dropDatabase();
}

void ClientPSqlTest::databaseProcessTest() {
    client->dropDatabase();
    client->createDatabase();

    dao::BaseQuery::queryPrimitive("select 1");
}

void ClientPSqlTest::tableProcessTest() {
    auto currentTables = client->exportAllTables();
    QCOMPARE(currentTables, {});

    QVERIFY(!client->checkTableExist(TestTb7::Info::getTableName()));

    //create test
    auto testTb7Reader = new dao::PSqlEntityReaderProvider<TestTb7>;
    client->createTableIfNotExist(testTb7Reader);
    delete testTb7Reader;
    QVERIFY(client->checkTableExist(TestTb7::Info::getTableName()));

    client->createTableIfNotExist(
        "TestTb8",
        {"id bigint", "name varchar(100)", "number real default 10.0"},
        {"id", "name"},
        {},
        "");
    QVERIFY(client->checkTableExist("TestTb8"));

    currentTables = client->exportAllTables();
    std::sort(currentTables.begin(), currentTables.end());
    QStringList expect;
    expect << "TestTb8" << "ts_testtb7";
    QCOMPARE(currentTables, expect);

    //rename test
    client->renameTable("TestTb8", "newtesttb");
    currentTables = client->exportAllTables();
    std::sort(currentTables.begin(), currentTables.end());
    expect.clear();
    expect << "newtesttb" << "ts_testtb7";
    QCOMPARE(currentTables, expect);

    //drop test
    client->dropTable("newtesttb");
    currentTables = client->exportAllTables();
    expect.clear();
    expect << "ts_testtb7";
    QCOMPARE(currentTables, expect);

    //truncate test
    TestTb7List data;
    data << TestTb7("f1", 1, 10, "f10");
    data << TestTb7("f2", 2, 11, "f11");
    data << TestTb7("f3", 3, 12, "f12");
    data << TestTb7("f4", 4, 13, "f13");
    dao::_insert<TestTb7>().build().insert2(data);

    TestTb7::Fields tf;
    auto fv = dao::_select<TestTb7>()
              .column(tf.field1)
              .with(_orderBy(tf.field1))
              .build()
              .list(tf.field1);
    QList<qint64> expectV = {1, 2, 3, 4};
    QCOMPARE(fv, expectV);

    client->truncateTable(TestTb7::Info::getTableName());
    dao::_insert<TestTb7>().build().insert2(data);

    fv = dao::_select<TestTb7>()
         .column(tf.field1)
         .with(_orderBy(tf.field1))
         .build()
         .list(tf.field1);
    QCOMPARE(fv, expectV);
}

void ClientPSqlTest::indexProcessTest() {
    auto testTb7Reader = new dao::PSqlEntityReaderProvider<TestTb7>;
    client->createTableIfNotExist(testTb7Reader);
    client->createIndex(testTb7Reader);
    delete testTb7Reader;

    //get field name test
    QStringList usedField1 = {"f1", "\"after\"", "\"by\""};
    auto indexName = client->getIndexFromFields(TestTb7::Info::getTableName(), usedField1);
    QCOMPARE(indexName, "ts_testtb7_index_f1_after_by");

    QList<QStringList> usedField2;
    usedField2 << (QStringList() << "\"case\"" << "field3");
    usedField2 << (QStringList() << "field4" << "field10");
    auto indexNames = client->getIndexArrayFromFields(TestTb7::Info::getTableName(), usedField2);
    QStringList expectNames = {"ts_testtb7_index_case_field3", "ts_testtb7_index_field4_field10"};
    QCOMPARE(indexNames, expectNames);

    //export test
    auto indexes = client->exportAllIndexes(TestTb7::Info::getTableName());

    auto normalIndex = indexes[dao::IndexType::INDEX_NORMAL];
    std::sort(normalIndex.begin(), normalIndex.end());
    QStringList expectIndex = {"ts_testtb7_index_field2", "ts_testtb7_index_field3_field4"};
    QCOMPARE(normalIndex, expectIndex);

    auto uniqueIndex = indexes[dao::IndexType::INDEX_UNIQUE];
    std::sort(uniqueIndex.begin(), uniqueIndex.end());
    QStringList expectUIndex = {"ts_testtb7_index_field2_field4"};
    QCOMPARE(uniqueIndex, expectUIndex);

    //drop index test
    client->dropIndex(TestTb7::Info::getTableName(), QStringList() << "field3" << "field4");
    indexes = client->exportAllIndexes(TestTb7::Info::getTableName());
    normalIndex = indexes[dao::IndexType::INDEX_NORMAL];
    QStringList expectDropIndex1 = {"ts_testtb7_index_field2"};
    QCOMPARE(normalIndex, expectDropIndex1);

    client->dropIndex(TestTb7::Info::getTableName(), "ts_testtb7_index_field2_field4");
    indexes = client->exportAllIndexes(TestTb7::Info::getTableName());
    uniqueIndex = indexes[dao::IndexType::INDEX_UNIQUE];
    QVERIFY(uniqueIndex.isEmpty());

    //create index test
    client->createIndex(TestTb7::Info::getTableName(),
                        QStringList() << "field3" << "field4",
                        dao::IndexType::INDEX_NORMAL);
    indexes = client->exportAllIndexes(TestTb7::Info::getTableName());
    normalIndex = indexes[dao::IndexType::INDEX_NORMAL];
    std::sort(normalIndex.begin(), normalIndex.end());
    QStringList expectIndex2 = {"ts_testtb7_index_field2", "ts_testtb7_index_field3_field4"};
    QCOMPARE(normalIndex, expectIndex2);

    client->createIndex(TestTb7::Info::getTableName(),
                        "ts_testtb7_index_of_f4",
                        QStringList() << "field4",
                        dao::IndexType::INDEX_NORMAL,
                        nullptr);
    indexes = client->exportAllIndexes(TestTb7::Info::getTableName());
    normalIndex = indexes[dao::IndexType::INDEX_NORMAL];
    std::sort(normalIndex.begin(), normalIndex.end());
    QStringList expectIndex3 = {"ts_testtb7_index_field2", "ts_testtb7_index_field3_field4", "ts_testtb7_index_of_f4"};
    QCOMPARE(normalIndex, expectIndex3);

    //drop all
    client->dropAllIndexOnTable(TestTb7::Info::getTableName());
    indexes = client->exportAllIndexes(TestTb7::Info::getTableName());
    normalIndex = indexes[dao::IndexType::INDEX_NORMAL];
    QVERIFY(normalIndex.isEmpty());
    uniqueIndex = indexes[dao::IndexType::INDEX_UNIQUE];
    QVERIFY(uniqueIndex.isEmpty());
}

void ClientPSqlTest::fieldProcessTest() {
    auto testTb7Reader = new dao::PSqlEntityReaderProvider<TestTb7>;
    client->createTableIfNotExist(testTb7Reader);
    client->createIndex(testTb7Reader);
    delete testTb7Reader;

    auto fields = client->exportAllFields(TestTb7::Info::getTableName());
    QList<QPair<QString, QString>> expectFields;
    expectFields << qMakePair(QLatin1String("\"field1\""), QLatin1String("BIGINT"));
    expectFields << qMakePair(QLatin1String("\"field2\""), QLatin1String("CHARACTER VARYING"));
    expectFields << qMakePair(QLatin1String("\"field3\""), QLatin1String("INTEGER"));
    expectFields << qMakePair(QLatin1String("\"field4\""), QLatin1String("INTEGER"));
    expectFields << qMakePair(QLatin1String("\"field5\""), QLatin1String("TEXT"));
    QCOMPARE(fields, expectFields);

    //drop all user index before drop columns
    client->dropAllIndexOnTable(TestTb7::Info::getTableName());

    client->dropField(TestTb7::Info::getTableName(), "field1");//primary key constraint

    client->dropField(TestTb7::Info::getTableName(), "field2");//not null constraint

    client->dropField(TestTb7::Info::getTableName(), "field3");//unique constraint

    client->dropField(TestTb7::Info::getTableName(), "field4");//default constraint

    client->addField(TestTb7::Info::getTableName(), "field10 int not null default 1");

    client->renameField(TestTb7::Info::getTableName(), "field10", "field20");

    fields = client->exportAllFields(TestTb7::Info::getTableName());
    std::sort(fields.begin(), fields.end(), [](const QPair<QString, QString>& a, const QPair<QString, QString>& b) {
        return a.first < b.first;
    });
    expectFields.clear();
    expectFields << qMakePair(QLatin1String("\"field20\""), QLatin1String("INTEGER"));
    expectFields << qMakePair(QLatin1String("\"field5\""), QLatin1String("TEXT"));
    QCOMPARE(fields, expectFields);
}

void ClientPSqlTest::dataTransTest() {
    auto testTb7Reader = new dao::PSqlEntityReaderProvider<TestTb7>;
    client->createTableIfNotExist(testTb7Reader);
    delete testTb7Reader;

    auto testTb4Reader = new dao::PSqlEntityReaderProvider<TestTb8>;
    client->createTableIfNotExist(testTb4Reader);
    client->createIndex(testTb4Reader);
    delete testTb4Reader;

    TestTb7List testData;
    testData << TestTb7("fv2_1", 1, 11, "fv6_1");
    testData << TestTb7("fv2_2", 2, 22, "fv6_2");
    testData << TestTb7("fv2_3", 3, 33, "fv6_3");
    dao::_insert<TestTb7>().build().insert2(testData);

    client->transferData(TestTb7::Info::getTableName(), TestTb8::Info::getTableName());

    TestTb8::Fields tf2;
    auto data2 = dao::_select<TestTb8>().build().list(tf2.field3, tf2.field4, tf2.fieldEx);

    QList<std::tuple<int, int, int>> expectV2;
    expectV2 << std::make_tuple(1, 11, 20);
    expectV2 << std::make_tuple(2, 22, 20);
    expectV2 << std::make_tuple(3, 33, 20);
    QCOMPARE(data2, expectV2);
}