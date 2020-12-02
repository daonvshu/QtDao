#include "JoinTest.h"

#include <qtest.h>

#include "../sqliteentity/SqliteTest1.h"
#include "../sqliteentity/SqliteTest2.h"

#include "../../src/dao.h"

void JoinTest::initTestCase() {
    SqliteTest1::Fields sf1;
    SqliteTest2::Fields sf2;

    /*dao::_join<SqliteTest1, SqliteTest2>()
        .from<SqliteTest1>().filter(sf1.name == "a")
        .crossJoin<SqliteTest2>().on(sf2.id == sf1.id)
        .build().list();*/
}

void JoinTest::cleanup() {
}

void JoinTest::cleanupTestCase() {
}