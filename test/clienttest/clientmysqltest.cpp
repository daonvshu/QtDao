#include "clientmysqltest.h"

#include "mysql/testtb3.h"
#include "mysql/testtb4.h"

#include <dao.h>
#include <qtest.h>

void ClientMysqlTest::initTestCase() {
    dao::_config<dao::ConfigMysqlBuilder>()
            .version(1)
            .databaseName("mysql_client_test")
            .host("localhost")
            .port(3306)
            .user("root")
            .password("root")
            .options("MYSQL_OPT_CONNECT_TIMEOUT=3;MYSQL_OPT_READ_TIMEOUT=3;MYSQL_OPT_WRITE_TIMEOUT=3")
            .disableCreateTables()
            .disableCreateDatabase()
            .initializeDatabase();

    client = dao::globalConfig->getClient();
}

void ClientMysqlTest::cleanup() {
    client->dropTable(TestTb3::Info::getTableName());
    QVERIFY(!client->checkTableExist(TestTb3::Info::getTableName()));

    client->dropTable(TestTb4::Info::getTableName());
    QVERIFY(!client->checkTableExist(TestTb4::Info::getTableName()));
}

void ClientMysqlTest::cleanupTestCase() {
    client->dropDatabase();
}

void ClientMysqlTest::databaseProcessTest() {
    client->dropDatabase();
    client->createDatabase();

    dao::BaseQuery::queryPrimitive("select 1");
}

void ClientMysqlTest::tableProcessTest() {
    auto currentTables = client->exportAllTables();
    QCOMPARE(currentTables, {});

    QVERIFY(!client->checkTableExist(TestTb3::Info::getTableName()));

    //create test
    auto testTb3Reader = new dao::MysqlEntityReaderProvider<TestTb3>;
    client->createTableIfNotExist(testTb3Reader);
    delete testTb3Reader;
    QVERIFY(client->checkTableExist(TestTb3::Info::getTableName()));

    client->createTableIfNotExist(
            "TestTb4",
            { "id bigint", "name varchar(100)", "number real default 10.0" },
            { "id", "name" },
            {},
            "");
    QVERIFY(client->checkTableExist("TestTb4"));

    currentTables = client->exportAllTables();
    std::sort(currentTables.begin(), currentTables.end());
    QStringList expect;
    expect << "testtb4" << "ts_testtb3";
    QCOMPARE(currentTables, expect);

    //rename test
    client->renameTable("TestTb4", "newtesttb");
    currentTables = client->exportAllTables();
    std::sort(currentTables.begin(), currentTables.end());
    expect.clear();
    expect << "newtesttb" << "ts_testtb3";
    QCOMPARE(currentTables, expect);

    //drop test
    client->dropTable("newtesttb");
    currentTables = client->exportAllTables();
    expect.clear();
    expect << "ts_testtb3";
    QCOMPARE(currentTables, expect);

    //truncate test
    TestTb3List data;
    data << TestTb3("f1", 1, 10, "f10");
    data << TestTb3("f2", 2, 11, "f11");
    data << TestTb3("f3", 3, 12, "f12");
    data << TestTb3("f4", 4, 13, "f13");
    dao::_insert<TestTb3>().build().insert2(data);

    TestTb3::Fields tf;
    auto fv = dao::_select<TestTb3>()
            .column(tf.field1)
            .with(_orderBy(tf.field1))
            .build()
            .list(tf.field1);
    QList<qint64> expectV = { 1, 2, 3, 4 };
    QCOMPARE(fv, expectV);

    client->truncateTable(TestTb3::Info::getTableName());
    dao::_insert<TestTb3>().build().insert2(data);

    fv = dao::_select<TestTb3>()
            .column(tf.field1)
            .with(_orderBy(tf.field1))
            .build()
            .list(tf.field1);
    QCOMPARE(fv, expectV);
}

void ClientMysqlTest::indexProcessTest() {
    auto testTb3Reader = new dao::MysqlEntityReaderProvider<TestTb3>;
    client->createTableIfNotExist(testTb3Reader);
    client->createIndex(testTb3Reader);
    delete testTb3Reader;

    //get field name test
    QStringList usedField1 = { "f1", "`after`", "`by`" };
    auto indexName = client->getIndexFromFields(usedField1);
    QCOMPARE(indexName, "index_f1_after_by");

    QList<QStringList> usedField2;
    usedField2 << (QStringList() << "`case`" << "field3");
    usedField2 << (QStringList() << "field4" << "field10");
    auto indexNames = client->getIndexArrayFromFields(usedField2);
    QStringList expectNames = { "index_case_field3", "index_field4_field10" };
    QCOMPARE(indexNames, expectNames);

    //export test
    auto indexes = client->exportAllIndexes(TestTb3::Info::getTableName());

    auto normalIndex = indexes[dao::IndexType::INDEX_NORMAL];
    std::sort(normalIndex.begin(), normalIndex.end());
    QStringList expectIndex = { "index_field2", "index_field3_field4" };
    QCOMPARE(normalIndex, expectIndex);

    auto uniqueIndex = indexes[dao::IndexType::INDEX_UNIQUE];
    std::sort(uniqueIndex.begin(), uniqueIndex.end());
    QStringList expectUIndex = { "index_field2_field4" };
    QCOMPARE(uniqueIndex, expectUIndex);

    //drop index test
    client->dropIndex(TestTb3::Info::getTableName(), QStringList() << "field3" << "field4");
    indexes = client->exportAllIndexes(TestTb3::Info::getTableName());
    normalIndex = indexes[dao::IndexType::INDEX_NORMAL];
    QStringList expectDropIndex1 = { "index_field2" };
    QCOMPARE(normalIndex, expectDropIndex1);

    client->dropIndex(TestTb3::Info::getTableName(), "index_field2_field4");
    indexes = client->exportAllIndexes(TestTb3::Info::getTableName());
    uniqueIndex = indexes[dao::IndexType::INDEX_UNIQUE];
    QVERIFY(uniqueIndex.isEmpty());

    //create index test
    client->createIndex(TestTb3::Info::getTableName(),
                        QStringList() << "field3" << "field4",
                        dao::IndexType::INDEX_NORMAL);
    indexes = client->exportAllIndexes(TestTb3::Info::getTableName());
    normalIndex = indexes[dao::IndexType::INDEX_NORMAL];
    std::sort(normalIndex.begin(), normalIndex.end());
    QStringList expectIndex2 = { "index_field2", "index_field3_field4" };
    QCOMPARE(normalIndex, expectIndex2);

    client->createIndex(TestTb3::Info::getTableName(),
                        "index_of_f4",
                        QStringList() << "field4",
                        dao::IndexType::INDEX_NORMAL,
                        nullptr);
    indexes = client->exportAllIndexes(TestTb3::Info::getTableName());
    normalIndex = indexes[dao::IndexType::INDEX_NORMAL];
    std::sort(normalIndex.begin(), normalIndex.end());
    QStringList expectIndex3 = { "index_field2", "index_field3_field4", "index_of_f4" };
    QCOMPARE(normalIndex, expectIndex3);

    //drop all
    client->dropAllIndexOnTable(TestTb3::Info::getTableName());
    indexes = client->exportAllIndexes(TestTb3::Info::getTableName());
    normalIndex = indexes[dao::IndexType::INDEX_NORMAL];
    QVERIFY(normalIndex.isEmpty());
    uniqueIndex = indexes[dao::IndexType::INDEX_UNIQUE];
    QVERIFY(uniqueIndex.isEmpty());
}

void ClientMysqlTest::fieldProcessTest() {
    auto testTb3Reader = new dao::MysqlEntityReaderProvider<TestTb3>;
    client->createTableIfNotExist(testTb3Reader);
    client->createIndex(testTb3Reader);
    delete testTb3Reader;

    auto fields = client->exportAllFields(TestTb3::Info::getTableName());
    QList<QPair<QString, QString>> expectFields;
    expectFields << qMakePair(QLatin1String("field1"), QLatin1String("BIGINT"));
    expectFields << qMakePair(QLatin1String("field2"), QLatin1String("VARCHAR"));
    expectFields << qMakePair(QLatin1String("field3"), QLatin1String("INT"));
    expectFields << qMakePair(QLatin1String("field4"), QLatin1String("INT"));
    expectFields << qMakePair(QLatin1String("field5"), QLatin1String("TEXT"));
    QCOMPARE(fields, expectFields);

    //drop all user index before drop columns
    client->dropAllIndexOnTable(TestTb3::Info::getTableName());

    client->dropField(TestTb3::Info::getTableName(), "field1"); //primary key constraint

    client->dropField(TestTb3::Info::getTableName(), "field2"); //not null constraint

    client->dropField(TestTb3::Info::getTableName(), "field3"); //unique constraint

    client->dropField(TestTb3::Info::getTableName(), "field4"); //default constraint

    client->addField(TestTb3::Info::getTableName(), "field10 int not null default 1");

    client->renameField(TestTb3::Info::getTableName(), "field10", "field20");

    fields = client->exportAllFields(TestTb3::Info::getTableName());
    expectFields.clear();
    expectFields << qMakePair(QLatin1String("field20"), QLatin1String("INT"));
    expectFields << qMakePair(QLatin1String("field5"), QLatin1String("TEXT"));
    QCOMPARE(fields, expectFields);
}

void ClientMysqlTest::dataTransTest() {
    auto testTb3Reader = new dao::MysqlEntityReaderProvider<TestTb3>;
    client->createTableIfNotExist(testTb3Reader);
    delete testTb3Reader;

    auto testTb4Reader = new dao::MysqlEntityReaderProvider<TestTb4>;
    client->createTableIfNotExist(testTb4Reader);
    client->createIndex(testTb4Reader);
    delete testTb4Reader;

    TestTb3List testData;
    testData << TestTb3("fv2_1", 1, 11, "fv6_1");
    testData << TestTb3("fv2_2", 2, 22, "fv6_2");
    testData << TestTb3("fv2_3", 3, 33, "fv6_3");
    dao::_insert<TestTb3>().build().insert2(testData);

    client->transferData(TestTb3::Info::getTableName(), TestTb4::Info::getTableName());

    TestTb4::Fields tf2;
    auto data2 = dao::_select<TestTb4>().build().list(tf2.field3, tf2.field4, tf2.fieldEx);

    QList<std::tuple<int, int, int>> expectV2;
    expectV2 << std::make_tuple(1, 11, 20);
    expectV2 << std::make_tuple(2, 22, 20);
    expectV2 << std::make_tuple(3, 33, 20);
    QCOMPARE(data2, expectV2);
}