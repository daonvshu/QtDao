#include "JoinTest.h"

#include <qtest.h>

#include "../sqliteentity/SqliteTest1.h"
#include "../sqliteentity/SqliteTest2.h"

#include "../../src/dao.h"

void JoinTest::initTestCase() {
    /*dao::_join()
        .from<SqliteTest1>().filter()
        .crossJoin<SqliteTest2>().filter()
        .build().list();*/
}

void JoinTest::cleanup() {
}

void JoinTest::cleanupTestCase() {
}
