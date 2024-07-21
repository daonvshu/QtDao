#include "foreignkeytest.h"

#include "dao.h"

#include <qtest.h>
#include <qthread.h>
#include <qeventloop.h>

#include "sqlite/artist.h"
#include "sqlite/track.h"
#include "sqlite/customer.h"

#include "mysql/artist.h"
#include "mysql/track.h"
#include "mysql/customer.h"

#include "sqlserver/artist.h"
#include "sqlserver/track.h"
#include "sqlserver/customer.h"

#include "utils/testconfigloader.h"

using namespace dao;

ForeignKeyTest::ForeignKeyTest()
    : DatabaseSelector(TestConfigLoader::instance().config().getTestTargetDb())
{}

void ForeignKeyTest::initTestCase() {
    setupDatabase("foreign_key_test", 1);
    cleanup();
}

#define runTestImpl(func) \
const auto& targetDb = TestConfigLoader::instance().config().getTestTargetDb(); \
if (targetDb == TestTargetDb::Target_Sqlite) { \
    func<TsSqlite::Artist, TsSqlite::Track, TsSqlite::Customer>(); \
} else if (targetDb == TestTargetDb::Target_Mysql) { \
    func<TsMysql::Artist, TsMysql::Track, TsMysql::Customer>(); \
} else if (targetDb == TestTargetDb::Target_SqlServer) { \
    func<TsSqlServer::Artist, TsSqlServer::Track, TsSqlServer::Customer>(); \
}

template<typename Artist, typename Track, typename Customer>
void testInsertImpl() {
    QList<Artist> artistList {
        Artist(1, "The Beatles", 4),
        Artist(2, "Led Zeppelin", 4),
        Artist(3, "Pink Floyd", 5),
    };
    dao::_insert<Artist>().build().insert2(artistList);

    try {
        Track track(1, "Yesterday", 4);
        dao::_insert<Track>().build().insert(track);
        QFAIL("the track should not be inserted success here!");
    } catch (dao::DaoException&) {}

    QList<Track> trackList {
        Track(1, "Yesterday", 1),
        Track(2, "Stairway to Heaven", 2),
        Track(3, "Another Brick in the Wall", 3),
    };
    dao::_insert<Track>().build().insert2(trackList);

    QList<Customer> customerList {
        Customer(1, "Alice", 1, 4),
        Customer(2, "Bob", 2, 4),
        Customer(3, "Charlie", 3, 5),
    };
    dao::_insert<Customer>().build().insert2(customerList);

    int trackSize = dao::_count<Track>().count();
    QCOMPARE(trackSize, 3);
    int customerSize = dao::_count<Customer>().count();
    QCOMPARE(customerSize, 3);
}

void ForeignKeyTest::testInsert() {
    runTestImpl(testInsertImpl)
}

template<typename Artist, typename Track, typename Customer>
void testUpdateImpl() {
    Artist artist(3, "Pink Floyd", 5);
    dao::_insert<Artist>().build().insert(artist);

    Track track(3, "Another Brick in the Wall", 3);
    dao::_insert<Track>().build().insert(track);

    Customer customer(3, "Charlie", 3, 5);
    dao::_insert<Customer>().build().insert(customer);

    typename Artist::Fields af;
    dao::_update<Artist>().set(af.id = 5).filter(af.id == 3).build().update();

    typename Track::Fields tf;
    auto track3 = dao::_select<Track>().filter(tf.id == 3).build().unique();
    QCOMPARE(track3.pkArtist, 5);

    typename Customer::Fields cf;
    auto customer3 = dao::_select<Customer>().filter(cf.id == 3).build().unique();
    QCOMPARE(customer3.pkArtistId, 5);
}

void ForeignKeyTest::testUpdate() {
    runTestImpl(testUpdateImpl)
}

template<typename Artist, typename Track, typename Customer>
void testDeleteImpl() {
    Artist artist(3, "Pink Floyd", 5);
    dao::_insert<Artist>().build().insert(artist);

    Track track(3, "Another Brick in the Wall", 3);
    dao::_insert<Track>().build().insert(track);

    Customer customer(3, "Charlie", 3, 5);
    dao::_insert<Customer>().build().insert(customer);

    typename Artist::Fields af;
    dao::_delete<Artist>().filter(af.id == 3).build().deleteBy();

    typename Track::Fields tf;
    auto track3 = dao::_select<Track>().filter(tf.id == 3).build().unique();
    QCOMPARE(track3.id, -1);

    typename Customer::Fields cf;
    auto customer3 = dao::_select<Customer>().filter(cf.id == 3).build().unique();
    QCOMPARE(customer3.id, -1);
}

void ForeignKeyTest::testDelete() {
    runTestImpl(testDeleteImpl)
}

template<typename Artist, typename Track, typename Customer>
void testTransactionImpl() {

    Artist artist(3, "Pink Floyd", 5);
    dao::_insert<Artist>().build().insert(artist);

    int step = 0;
    try {
        dao::transaction();

        Customer customer1(3, "Alice", 3, 5);
        dao::_insert<Customer>().build().insert(customer1);
        step = 1;

        Customer customer2(4, "Bob", 4, 5);
        dao::_insert<Customer>().build().insert(customer2);
        step = 2;

        Customer customer3(5, "Charlie", 3, 5);
        dao::_insert<Customer>().build().insert(customer3);
        step = 3;

        dao::commit();
        step = 4;
    } catch (dao::DaoException&) {
        dao::rollback();
    }
    int customerSize = dao::_count<Customer>().count();
    QCOMPARE(customerSize, 0);
    QCOMPARE(step, 1);

    //deferrable test
    if (TestConfigLoader::instance().config().getTestTargetDb() == TestTargetDb::Target_Sqlite) {
        step = 0;
        try {
            dao::transaction();

            Track track1(1, "Yesterday", 3);
            dao::_insert<Track>().build().insert(track1);
            step = 1;

            Track track2(2, "Stairway to Heaven", 2);
            dao::_insert<Track>().build().insert(track2);
            step = 2;

            Track track3(3, "Another Brick in the Wall", 3);
            dao::_insert<Track>().build().insert(track3);
            step = 3;

            dao::commit();
            step = 4;
        } catch (dao::DaoException&) {
            dao::rollback();
        }
        int trackSize = dao::_count<Track>().count();
        QCOMPARE(trackSize, 0);
        QCOMPARE(step, 3);
    }
}

void ForeignKeyTest::testTransaction() {
    runTestImpl(testTransactionImpl)
}

template<typename Artist, typename Track, typename Customer>
void testVersionUpgradeImpl() {
    QList<Artist> artistList {
            Artist(1, "The Beatles", 4),
            Artist(2, "Led Zeppelin", 4),
            Artist(3, "Pink Floyd", 5),
    };
    dao::_insert<Artist>().build().insert2(artistList);

    try {
        Track track(1, "Yesterday", 4);
        dao::_insert<Track>().build().insert(track);
        QFAIL("the track should not be inserted success here!");
    } catch (dao::DaoException&) {}

    QList<Track> trackList {
            Track(1, "Yesterday", 1),
            Track(2, "Stairway to Heaven", 2),
            Track(3, "Another Brick in the Wall", 3),
    };
    dao::_insert<Track>().build().insert2(trackList);

    QList<Customer> customerList {
            Customer(1, "Alice", 1, 4),
            Customer(2, "Bob", 2, 4),
            Customer(3, "Charlie", 3, 5),
    };
    dao::_insert<Customer>().build().insert2(customerList);

    ForeignKeyTest().setupDatabase("foreign_key_test", 2);

    int trackSize = dao::_count<Track>().count();
    QCOMPARE(trackSize, 3);
    int customerSize = dao::_count<Customer>().count();
    QCOMPARE(customerSize, 3);

    //check foreign key exist
    try {
        Track track(1, "Yesterday", 4);
        dao::_insert<Track>().build().insert(track);
        QFAIL("the track should not be inserted success here!");
    } catch (dao::DaoException&) {}
}

void ForeignKeyTest::testVersionUpgrade() {
    runTestImpl(testVersionUpgradeImpl)
}

void ForeignKeyTest::testRunInThread() {
    QEventLoop loop;
    QThread::create([&] {
        testInsert();
        loop.quit();
    })->start();
    loop.exec();
}

template<typename Artist, typename Track, typename Customer>
void cleanupImpl() {
    dao::_delete<Track>().build().deleteBy();
    dao::_delete<Customer>().build().deleteBy();
    dao::_delete<Artist>().build().deleteBy();
}

void ForeignKeyTest::cleanup() {
    runTestImpl(cleanupImpl)
}

void ForeignKeyTest::cleanupTestCase() {
    dao::ConfigManager::getConfig()->getClient()->dropDatabase();
}
