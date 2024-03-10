#include "foreignkeytest.h"

#include "dao.h"

#include <qtest.h>

#include "artist.h"
#include "track.h"
#include "customer.h"

using namespace dao;

void ForeignKeyTest::initTestCase() {
    try {
        if (TEST_DB == QLatin1String("sqlite")) {
            dao::_config<dao::ConfigSqliteBuilder>()
                    .version(1)
                    .databaseName("sqlite_foreign_key_test")
                    .initializeDatabase();
        } else if (TEST_DB == QLatin1String("mysql")) {
            dao::_config<dao::ConfigMysqlBuilder>()
                    .version(1)
                    .databaseName("mysql_foreign_key_test")
                    .host("localhost")
                    .port(3306)
                    .user("root")
                    .password("root")
                    .initializeDatabase();
        } else if (TEST_DB == QLatin1String("sqlserver")) {
            dao::_config<dao::ConfigSqlServerBuilder>()
                    .version(1)
                    .databaseName("sqlserver_foreign_key_test")
                    .host("localhost")
                    .user("sa")
                    .password("root")
                    .initializeDatabase();
        }
    } catch (dao::DaoException& e) {
        Q_UNUSED(e)
        auto validDrivers = QSqlDatabase::drivers();
        Q_UNUSED(validDrivers)
        qFatal("setup database fail!");
    }

    cleanup();
}

void ForeignKeyTest::testInsert() {
    ArtistList artistList {
        Artist(1, "The Beatles", 4),
        Artist(2, "Led Zeppelin", 4),
        Artist(3, "Pink Floyd", 5),
    };
    dao::_insert<Artist>().build().insert2(artistList);

    try {
        Track track(1, "Yesterday", 4);
        dao::_insert<Track>().disableFatalMsg().build().insert(track);
        QFAIL("the track should not be inserted success here!");
    } catch (dao::DaoException&) {}

    TrackList trackList {
        Track(1, "Yesterday", 1),
        Track(2, "Stairway to Heaven", 2),
        Track(3, "Another Brick in the Wall", 3),
    };
    dao::_insert<Track>().build().insert2(trackList);

    CustomerList customerList {
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

void ForeignKeyTest::testUpdate() {
    Artist artist(3, "Pink Floyd", 5);
    dao::_insert<Artist>().build().insert(artist);

    Track track(3, "Another Brick in the Wall", 3);
    dao::_insert<Track>().build().insert(track);

    Customer customer(3, "Charlie", 3, 5);
    dao::_insert<Customer>().build().insert(customer);

    Artist::Fields af;
    dao::_update<Artist>().set(af.id = 5).filter(af.id == 3).build().update();

    Track::Fields tf;
    auto track3 = dao::_select<Track>().filter(tf.id == 3).build().unique();
    QCOMPARE(track3.pkArtist, 5);

    Customer::Fields cf;
    auto customer3 = dao::_select<Customer>().filter(cf.id == 3).build().unique();
    QCOMPARE(customer3.pkArtistId, 5);
}

void ForeignKeyTest::testDelete() {
    Artist artist(3, "Pink Floyd", 5);
    dao::_insert<Artist>().build().insert(artist);

    Track track(3, "Another Brick in the Wall", 3);
    dao::_insert<Track>().build().insert(track);

    Customer customer(3, "Charlie", 3, 5);
    dao::_insert<Customer>().build().insert(customer);

    Artist::Fields af;
    dao::_delete<Artist>().filter(af.id == 3).build().deleteBy();

    Track::Fields tf;
    auto track3 = dao::_select<Track>().filter(tf.id == 3).build().unique();
    QCOMPARE(track3.id, -1);

    Customer::Fields cf;
    auto customer3 = dao::_select<Customer>().filter(cf.id == 3).build().unique();
    QCOMPARE(customer3.id, -1);
}

void ForeignKeyTest::testTransaction() {

    Artist artist(3, "Pink Floyd", 5);
    dao::_insert<Artist>().build().insert(artist);

    int step = 0;
    try {
        dao::transaction();

        Customer customer1(3, "Alice", 3, 5);
        dao::_insert<Customer>().build().insert(customer1);
        step = 1;

        Customer customer2(4, "Bob", 4, 5);
        dao::_insert<Customer>().disableFatalMsg().build().insert(customer2);
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
    if (TEST_DB == QLatin1String("sqlite")) {
        step = 0;
        try {
            dao::transaction();

            Track track1(1, "Yesterday", 3);
            dao::_insert<Track>().build().insert(track1);
            step = 1;

            Track track2(2, "Stairway to Heaven", 2);
            dao::_insert<Track>().disableFatalMsg().build().insert(track2);
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

void ForeignKeyTest::testVersionUpgrade() {
    ArtistList artistList {
            Artist(1, "The Beatles", 4),
            Artist(2, "Led Zeppelin", 4),
            Artist(3, "Pink Floyd", 5),
    };
    dao::_insert<Artist>().build().insert2(artistList);

    try {
        Track track(1, "Yesterday", 4);
        dao::_insert<Track>().disableFatalMsg().build().insert(track);
        QFAIL("the track should not be inserted success here!");
    } catch (dao::DaoException&) {}

    TrackList trackList {
            Track(1, "Yesterday", 1),
            Track(2, "Stairway to Heaven", 2),
            Track(3, "Another Brick in the Wall", 3),
    };
    dao::_insert<Track>().build().insert2(trackList);

    CustomerList customerList {
            Customer(1, "Alice", 1, 4),
            Customer(2, "Bob", 2, 4),
            Customer(3, "Charlie", 3, 5),
    };
    dao::_insert<Customer>().build().insert2(customerList);

    try {
        if (TEST_DB == QLatin1String("sqlite")) {
            dao::_config<dao::ConfigSqliteBuilder>()
                    .version(2)
                    .databaseName("sqlite_foreign_key_test")
                    .initializeDatabase();
        } else if (TEST_DB == QLatin1String("mysql")) {
            dao::_config<dao::ConfigMysqlBuilder>()
                    .version(2)
                    .databaseName("mysql_foreign_key_test")
                    .host("localhost")
                    .port(3306)
                    .user("root")
                    .password("root")
                    .initializeDatabase();
        } else if (TEST_DB == QLatin1String("sqlserver")) {
            dao::_config<dao::ConfigSqlServerBuilder>()
                    .version(2)
                    .databaseName("sqlserver_foreign_key_test")
                    .host("localhost")
                    .user("sa")
                    .password("root")
                    .initializeDatabase();
        }
    } catch (dao::DaoException& e) {
        Q_UNUSED(e);
        QFAIL("upgrade with foreign key test fail!");
    }

    int trackSize = dao::_count<Track>().count();
    QCOMPARE(trackSize, 3);
    int customerSize = dao::_count<Customer>().count();
    QCOMPARE(customerSize, 3);

    //check foreign key exist
    try {
        Track track(1, "Yesterday", 4);
        dao::_insert<Track>().disableFatalMsg().build().insert(track);
        QFAIL("the track should not be inserted success here!");
    } catch (dao::DaoException&) {}
}

void ForeignKeyTest::cleanup() {
    dao::_delete<Track>().build().deleteBy();
    dao::_delete<Customer>().build().deleteBy();
    dao::_delete<Artist>().build().deleteBy();
}

void ForeignKeyTest::cleanupTestCase() {
    dao::globalConfig->getClient()->dropDatabase();
}
