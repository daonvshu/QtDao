#include "LoggingTest.h"

#include "entity/sqliteentity/sqlitetest1.h"
#include "entity/sqliteentity/sqlitetest2.h"

void LoggingTest::initTestCase() {
    configDb();

    PASSMYSQL;
    PASSSQLSERVER;
    PASSPSQL;

    SqliteTest1List sqliteData1;

    sqliteData1 << SqliteTest1(1, "abc", 10, "");
    sqliteData1 << SqliteTest1(2, "alice", 11, "alice1");
    sqliteData1 << SqliteTest1(3, "bob", 12, "bob boom");
    sqliteData1 << SqliteTest1(4, "client1", 14, "1");
    sqliteData1 << SqliteTest1(5, "client2", 12, "xxx");
    sqliteData1 << SqliteTest1(6, "client3", 24, "12");
    sqliteData1 << SqliteTest1(7, "client4", 8, "ax");
    dao::_insert<SqliteTest1>().build().insert2(sqliteData1);
}

Q_LOGGING_CATEGORY(statementTest, "query.statement.test")
void LoggingTest::singleStatementTest() {
    PASSMYSQL;
    PASSSQLSERVER;
    PASSPSQL;

    SqliteTest1::Fields sf;
    dao::_select<SqliteTest1>()
            .with(_orderBy(sf.name))
            .logging(statementTest)
            .build().list();
}

Q_LOGGING_CATEGORY(valueTest, "query.value.test")
void LoggingTest::statementValueTest() {
    PASSMYSQL;
    PASSSQLSERVER;
    PASSPSQL;

    SqliteTest1::Fields sf;
    dao::_select<SqliteTest1>()
            .filter(sf.number > 12, sf.name.notNull())
            .with(_limit(3))
            .logging(valueTest)
            .build().list();

    auto names = QStringList() << "abc" << "alice" << "bob" << "client1" << "client2"
                               << "client3" << "client4" << "client5" << "client6";

    dao::_select<SqliteTest1>()
            .filter(sf.name.in(names))
            .logging(valueTest)
            .build().list();
}

Q_LOGGING_CATEGORY(batchTest, "query.batch.test")
void LoggingTest::batchQueryTest() {
    PASSMYSQL;
    PASSSQLSERVER;
    PASSPSQL;

    SqliteTest2List sqliteData2;
    sqliteData2 << SqliteTest2("joker", 9999, -1, 30);
    sqliteData2 << SqliteTest2("bob", 12, 0, "abc");
    sqliteData2 << SqliteTest2("func", 10, -2, 50);
    sqliteData2 << SqliteTest2("func", 50, 0, 50);
    sqliteData2 << SqliteTest2("gyc", 11, -2, 8);
    sqliteData2 << SqliteTest2("er", 12, 0, "50");
    sqliteData2 << SqliteTest2("vcc", 13, -20, 81);
    sqliteData2 << SqliteTest2("ycc", 14, 10, 9);
    dao::_insert<SqliteTest2>().logging(batchTest).build().insert(sqliteData2);

    dao::_truncate<SqliteTest2>();
}

void LoggingTest::rulerTest() {
    PASSMYSQL;
    PASSSQLSERVER;
    PASSPSQL;

    QLoggingCategory::setFilterRules("*.debug=true\nquery.value.test.debug=false");

    singleStatementTest();
    statementValueTest();
    batchQueryTest();
}

void LoggingTest::defaultLoggingTest() {
    PASSMYSQL;
    PASSSQLSERVER;
    PASSPSQL;

    QLoggingCategory::setFilterRules("*.debug=true");
    dao::loggingUseDefault();

    SqliteTest1::Fields sf;
    dao::_select<SqliteTest1>()
            .filter(sf.number > 12, sf.name.notNull())
            .with(_limit(3))
            .build().list();

    auto names = QStringList() << "abc" << "alice" << "bob" << "client1" << "client2"
                               << "client3" << "client4" << "client5" << "client6";

    dao::_select<SqliteTest1>()
            .filter(sf.name.in(names))
            .build().list();

    dao::loggingUseDefault(false);
}

void LoggingTest::cleanup() {
    clearCacheAndPrintIfTestFail();
}

void LoggingTest::cleanupTestCase() {
    ConfigManager::getConfig()->getClient()->dropDatabase();
}

