#include "clientsqlitetest.h"

#include "sqlite/testtb1.h"
#include "sqlite/testtb2.h"

#include <dao.h>
#include <qtest.h>

using namespace CT;

void ClientSqliteTest::initTestCase() {
    dao::_config<dao::ConfigSqliteBuilder>()
            .version(1)
            .databaseName("sqlite_client_test")
            .disableCreateDatabase()
            .disableCreateTables()
            .initializeDatabase();

    client = dao::ConfigManager::getConfig()->getClient();
}

void ClientSqliteTest::cleanup() {
    client->dropTable(TestTb1::Info::getTableName());
    QVERIFY(!client->checkTableExist(TestTb1::Info::getTableName()));

    client->dropTable(TestTb2::Info::getTableName());
    QVERIFY(!client->checkTableExist(TestTb2::Info::getTableName()));
}

void ClientSqliteTest::cleanupTestCase() {
    client->dropDatabase();
}

void ClientSqliteTest::databaseProcessTest() {
    client->dropDatabase();
    client->createDatabase();

    dao::BaseQuery::queryPrimitive("select 1");

    auto appLocal = dynamic_cast<dao::ConfigSqliteBuilder*>(dao::ConfigManager::getConfig().get())->getDbStorePath();
    QVERIFY(QFile::exists(appLocal));
}

void ClientSqliteTest::tableProcessTest() {
    auto currentTables = client->exportAllTables();
    QCOMPARE(currentTables, {});

    QVERIFY(!client->checkTableExist(TestTb1::Info::getTableName()));

    //create test
    auto testTb1Reader = new dao::SqliteEntityReaderProvider<TestTb1>;
    client->createTableIfNotExist(testTb1Reader);
    delete testTb1Reader;
    QVERIFY(client->checkTableExist(TestTb1::Info::getTableName()));

    client->createTableIfNotExist(
            "testtb2",
            { "id long", "name text default 'text'", "number real default 10.0" },
            { "id", "name" },
            {},
            "");
    QVERIFY(client->checkTableExist("testtb2"));

    currentTables = client->exportAllTables();
    std::sort(currentTables.begin(), currentTables.end());
    QStringList expect;
    expect << "testtb2" << "ts_testtb1";
    QCOMPARE(currentTables, expect);

    //rename test
    client->renameTable("testtb2", "newtesttb");
    currentTables = client->exportAllTables();
    std::sort(currentTables.begin(), currentTables.end());
    expect.clear();
    expect << "newtesttb" << "ts_testtb1";
    QCOMPARE(currentTables, expect);

    //drop test
    client->dropTable("newtesttb");
    currentTables = client->exportAllTables();
    expect.clear();
    expect << "ts_testtb1";
    QCOMPARE(currentTables, expect);

    //truncate test
    TestTb1List data;
    data << TestTb1("f1", 1, 10, "text", "f10");
    data << TestTb1("f2", 2, 11, "text", "f11");
    data << TestTb1("f3", 3, 12, "text", "f12");
    data << TestTb1("f4", 4, 13, "text", "f13");
    dao::_insert<TestTb1>().build().insert2(data);

    TestTb1::Fields tf;
    auto fv = dao::_select<TestTb1>()
            .column(tf.field1)
            .with(_orderBy(tf.field1))
            .build()
            .list(tf.field1);
    QList<qint64> expectV = { 1, 2, 3, 4 };
    QCOMPARE(fv, expectV);

    client->truncateTable(TestTb1::Info::getTableName());
    dao::_insert<TestTb1>().build().insert2(data);

    fv = dao::_select<TestTb1>()
            .column(tf.field1)
            .with(_orderBy(tf.field1))
            .build()
            .list(tf.field1);
    QCOMPARE(fv, expectV);
}

void ClientSqliteTest::indexProcessTest() {
    auto testTb1Reader = new dao::SqliteEntityReaderProvider<TestTb1>;
    client->createTableIfNotExist(testTb1Reader);
    client->createIndex(testTb1Reader);
    delete testTb1Reader;

    //get field name test
    QStringList usedField1 = { "f1", "\"after\"", "\"by\"" };
    auto indexName = client->getIndexFromFields(usedField1);
    QCOMPARE(indexName, "index_f1_after_by");

    QList<QStringList> usedField2;
    usedField2 << (QStringList() << "\"case\"" << "field3");
    usedField2 << (QStringList() << "field4" << "field10");
    auto indexNames = client->getIndexArrayFromFields(usedField2);
    QStringList expectNames = { "index_case_field3", "index_field4_field10" };
    QCOMPARE(indexNames, expectNames);

    //export test
    auto indexes = client->exportAllIndexes(TestTb1::Info::getTableName());

    auto normalIndex = indexes[dao::IndexType::INDEX_NORMAL];
    std::sort(normalIndex.begin(), normalIndex.end());
    QStringList expectIndex = { "index_field2", "index_field3_field4" };
    QCOMPARE(normalIndex, expectIndex);

    auto uniqueIndex = indexes[dao::IndexType::INDEX_UNIQUE];
    std::sort(uniqueIndex.begin(), uniqueIndex.end());
    QStringList expectUIndex = { "index_field2_field6" };
    QCOMPARE(uniqueIndex, expectUIndex);

    //drop index test
    client->dropIndex(TestTb1::Info::getTableName(), QStringList() << "field3" << "field4");
    indexes = client->exportAllIndexes(TestTb1::Info::getTableName());
    normalIndex = indexes[dao::IndexType::INDEX_NORMAL];
    QStringList expectDropIndex1 = { "index_field2" };
    QCOMPARE(normalIndex, expectDropIndex1);

    client->dropIndex(TestTb1::Info::getTableName(), "index_field2_field6");
    indexes = client->exportAllIndexes(TestTb1::Info::getTableName());
    uniqueIndex = indexes[dao::IndexType::INDEX_UNIQUE];
    QVERIFY(uniqueIndex.isEmpty());

    //create index test
    client->createIndex(TestTb1::Info::getTableName(),
                        QStringList() << "field3" << "field4",
                        dao::IndexType::INDEX_NORMAL);
    indexes = client->exportAllIndexes(TestTb1::Info::getTableName());
    normalIndex = indexes[dao::IndexType::INDEX_NORMAL];
    std::sort(normalIndex.begin(), normalIndex.end());
    QStringList expectIndex2 = { "index_field2", "index_field3_field4" };
    QCOMPARE(normalIndex, expectIndex2);

    client->createIndex(TestTb1::Info::getTableName(),
                        "index_of_f4",
                        QStringList() << "field4",
                        dao::IndexType::INDEX_NORMAL,
                        nullptr);
    indexes = client->exportAllIndexes(TestTb1::Info::getTableName());
    normalIndex = indexes[dao::IndexType::INDEX_NORMAL];
    std::sort(normalIndex.begin(), normalIndex.end());
    QStringList expectIndex3 = { "index_field2", "index_field3_field4", "index_of_f4" };
    QCOMPARE(normalIndex, expectIndex3);

    //drop all
    client->dropAllIndexOnTable(TestTb1::Info::getTableName());
    indexes = client->exportAllIndexes(TestTb1::Info::getTableName());
    normalIndex = indexes[dao::IndexType::INDEX_NORMAL];
    QVERIFY(normalIndex.isEmpty());
    uniqueIndex = indexes[dao::IndexType::INDEX_UNIQUE];
    QVERIFY(uniqueIndex.isEmpty());
}

void ClientSqliteTest::fieldProcessTest() {
    auto testTb1Reader = new dao::SqliteEntityReaderProvider<TestTb1>;
    client->createTableIfNotExist(testTb1Reader);
    client->createIndex(testTb1Reader);
    delete testTb1Reader;

    auto fields = client->exportAllFields(TestTb1::Info::getTableName());
    QList<QPair<QString, QString>> expectFields;
    expectFields << qMakePair(QLatin1String("field1"), QLatin1String("INTEGER"));
    expectFields << qMakePair(QLatin1String("field2"), QLatin1String("TEXT"));
    expectFields << qMakePair(QLatin1String("field3"), QLatin1String("INTEGER"));
    expectFields << qMakePair(QLatin1String("field4"), QLatin1String("INTEGER"));
    expectFields << qMakePair(QLatin1String("field5"), QLatin1String("BLOB"));
    expectFields << qMakePair(QLatin1String("field6"), QLatin1String("TEXT"));
    QCOMPARE(fields, expectFields);

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 4)
    //drop all user index before drop columns
    client->dropAllIndexOnTable(TestTb1::Info::getTableName());

    //fail
    //client->dropField(TestTb1::Info::getTableName(), "field1"); //primary key constraint

    client->dropField(TestTb1::Info::getTableName(), "field2"); //not null constraint

    //fail
    //client->dropField(TestTb1::Info::getTableName(), "field3"); //unique constraint

    client->dropField(TestTb1::Info::getTableName(), "field4"); //default constraint
    client->dropField(TestTb1::Info::getTableName(), "field5");
    client->dropField(TestTb1::Info::getTableName(), "field6");

    client->addField(TestTb1::Info::getTableName(), "field10 int not null default 1");

    client->renameField(TestTb1::Info::getTableName(), "field10", "field20");

    fields = client->exportAllFields(TestTb1::Info::getTableName());
    expectFields.clear();
    expectFields << qMakePair(QLatin1String("field1"), QLatin1String("INTEGER"));
    expectFields << qMakePair(QLatin1String("field3"), QLatin1String("INTEGER"));
    expectFields << qMakePair(QLatin1String("field20"), QLatin1String("INT"));
    QCOMPARE(fields, expectFields);
#endif
}

void ClientSqliteTest::dataTransTest() {
    auto testTb1Reader = new dao::SqliteEntityReaderProvider<TestTb1>;
    client->createTableIfNotExist(testTb1Reader);
    delete testTb1Reader;

    auto testTb2Reader = new dao::SqliteEntityReaderProvider<TestTb2>;
    client->createTableIfNotExist(testTb2Reader);
    client->createIndex(testTb2Reader);
    delete testTb2Reader;

    TestTb1List testData;
    testData << TestTb1("fv2_1", 1, 11, "fv5_1", "fv6_1");
    testData << TestTb1("fv2_2", 2, 22, "fv5_2", "fv6_2");
    testData << TestTb1("fv2_3", 3, 33, "fv5_3", "fv6_3");
    dao::_insert<TestTb1>().build().insert2(testData);

    client->transferData(TestTb1::Info::getTableName(), TestTb2::Info::getTableName());

    TestTb2::Fields tf2;
    auto data2 = dao::_select<TestTb2>().build().list(tf2.field3, tf2.field4, tf2.fieldEx);

    QList<std::tuple<int, int, int>> expectV2;
    expectV2 << std::make_tuple(1, 11, 20);
    expectV2 << std::make_tuple(2, 22, 20);
    expectV2 << std::make_tuple(3, 33, 20);
    QCOMPARE(data2, expectV2);
}

