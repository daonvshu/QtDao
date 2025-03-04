#include "multidbtest.h"

#include <qdir.h>
#include <dao.h>
#include <qtest.h>
#include <qeventloop.h>
#include <qthread.h>

#define SESSION_SQLITE1     1
#define SESSION_SQLITE2     2
#define SESSION_MYSQL       3
#define SESSION_SQLSERVER   4
#define SESSION_PSQL        5

#include "sqlite/v1/artist.h"
#include "sqlite/v2/track.h"
#include "mysql/artist.h"
#include "sqlserver/artist.h"
#include "psql/artist.h"

#include "utils/testconfigloader.h"

void MultiDbTest::initTestCase() {
}

void MultiDbTest::setupTest() {
    try {
        dao::_config<dao::ConfigSqliteBuilder>()
                .version(1)
                .databaseName("multi_sqlitev1test")
                .saveDir(QDir::currentPath())
                .configAlias("sql1")
                .session(SESSION_SQLITE1)
                .initializeDatabase();

        dao::_config<dao::ConfigSqliteBuilder>()
                .version(1)
                .databaseName("multi_sqlitev2test")
                .saveDir(QDir::currentPath())
                .configAlias("sql2")
                .session(SESSION_SQLITE2)
                .initializeDatabase();

        const auto& mysqlConfigOption = TestConfigLoader::instance().config().optionMysql();
        dao::_config<dao::ConfigMysqlBuilder>()
                .version(1)
                .databaseName("multi_mysqltest")
                .host(mysqlConfigOption.host())
                .port(mysqlConfigOption.port())
                .user(mysqlConfigOption.user())
                .password(mysqlConfigOption.password())
                .configAlias("mys")
                .session(SESSION_MYSQL)
                .initializeDatabase();

        const auto& sqlServerConfigOption = TestConfigLoader::instance().config().optionSqlServer();
        dao::_config<dao::ConfigSqlServerBuilder>()
                .version(1)
                .databaseName("multi_sqlservertest")
                .host(sqlServerConfigOption.host())
                .user(sqlServerConfigOption.user())
                .password(sqlServerConfigOption.password())
                .configAlias("ms")
                .session(SESSION_SQLSERVER)
                .initializeDatabase();

        const auto& psqlConfigOption = TestConfigLoader::instance().config().optionPSql();
        dao::_config<dao::ConfigPSqlBuilder>()
                .version(1)
                .databaseName("multi_psqltest")
                .host(psqlConfigOption.host())
                .port(psqlConfigOption.port())
                .user(psqlConfigOption.user())
                .password(psqlConfigOption.password())
                .configAlias("ps")
                .session(SESSION_PSQL)
                .initializeDatabase();

    } catch (dao::DaoException& e) {
        Q_UNUSED(e)
        auto validDrivers = QSqlDatabase::drivers();
        Q_UNUSED(validDrivers)
        qFatal("setup database fail!");
    }
}

void MultiDbTest::sqliteNormalQueryTest() {
    using namespace SqlV1;
    using namespace SqlV2;
    Artist artistData1(1, "The Beatles", 4);
    Artist artistData2(2, "Led Zeppelin", 5);
    dao::_insert<Artist>(SESSION_SQLITE1).build().insert({ artistData1, artistData2 });
    Track trackData1(2, "Stairway to Heaven");
    Track trackData2(3, "Another Brick in the Wall");
    dao::_insert<Track>(SESSION_SQLITE2).build().insert({ trackData1, trackData2 });

    Artist::Fields af;
    Track::Fields tf;

    dao::_beginSession(SESSION_SQLITE1);
    auto artistQuery = dao::_select<Artist>().filter(af.id == 1).build().unique();
    QCOMPARE(artistQuery, artistData1);
    dao::_endSession();

    dao::_beginSession(SESSION_SQLITE2);
    auto trackQuery = dao::_select<Track>().filter(tf.id == 2).build().unique();
    QCOMPARE(trackQuery, trackData1);
    dao::_endSession();

    {
        DAO_SCOPED_SESSION_BEGIN(SESSION_SQLITE1);
        artistQuery = dao::_select<Artist>().filter(af.id == 2).build().unique();
        QCOMPARE(artistQuery, artistData2);
        {
            DAO_SCOPED_SESSION_BEGIN(SESSION_SQLITE2);
            trackQuery = dao::_select<Track>().filter(tf.id == 3).build().unique();
            QCOMPARE(trackQuery, trackData2);
        }
        artistQuery = dao::_select<Artist>().filter(af.id == 1).build().unique();
        QCOMPARE(artistQuery, artistData1);
    }
}

void MultiDbTest::differentDbQueryTest() {
    SqlV1::Artist artistData1(1, "The Beatles", 4);
    SqlV1::Artist artistData2(2, "Led Zeppelin", 5);
    dao::_insert<SqlV1::Artist>(SESSION_SQLITE1).build().insert({ artistData1, artistData2 });

    Mys::Artist artistData3(1, "The Beatles2", 14);
    Mys::Artist artistData4(3, "Pink Floyd2", 15);
    dao::_insert<Mys::Artist>(SESSION_MYSQL).build().insert({ artistData3, artistData4 });

    Ms::Artist artistData5(2, "Led Zeppelin3", 15);
    Ms::Artist artistData6(3, "Pink Floyd3", 15);
    dao::_insert<Ms::Artist>(SESSION_SQLSERVER).build().insert({ artistData5, artistData6 });

    Ps::Artist artistData7(2, "Led Zeppelin4", 15);
    Ps::Artist artistData8(3, "Pink Floyd4", 15);
    dao::_insert<Ps::Artist>(SESSION_PSQL).build().insert({ artistData7, artistData8 });

    SqlV1::Artist::Fields af;

    dao::_beginSession(SESSION_SQLITE1);
    auto artistQuery1 = dao::_select<SqlV1::Artist>().filter(af.id == 1).build().unique();
    QCOMPARE(artistQuery1, artistData1);
    dao::_endSession();

    dao::_beginSession(SESSION_MYSQL);
    auto artistQuery2 = dao::_select<Mys::Artist>().filter(af.id == 1).build().unique();
    QCOMPARE(artistQuery2, artistData3);
    dao::_endSession();

    dao::_beginSession(SESSION_SQLSERVER);
    auto artistQuery3 = dao::_select<Ms::Artist>().filter(af.id == 2).build().unique();
    QCOMPARE(artistQuery3, artistData5);
    dao::_endSession();

    dao::_beginSession(SESSION_PSQL);
    auto artistQuery4 = dao::_select<Ps::Artist>().filter(af.id == 2).build().unique();
    QCOMPARE(artistQuery4, artistData7);
    dao::_endSession();

    {
        DAO_SCOPED_SESSION_BEGIN(SESSION_SQLITE1);
        artistQuery1 = dao::_select<SqlV1::Artist>().filter(af.id == 2).build().unique();
        QCOMPARE(artistQuery1, artistData2);
        {
            DAO_SCOPED_SESSION_BEGIN(SESSION_MYSQL);
            {
                DAO_SCOPED_SESSION_BEGIN(SESSION_SQLSERVER);
                {
                    DAO_SCOPED_SESSION_BEGIN(SESSION_PSQL);
                    artistQuery4 = dao::_select<Ps::Artist>().filter(af.id == 3).build().unique();
                    QCOMPARE(artistQuery4, artistData8);
                }
                artistQuery3 = dao::_select<Ms::Artist>().filter(af.id == 3).build().unique();
                QCOMPARE(artistQuery3, artistData6);
            }
            artistQuery2 = dao::_select<Mys::Artist>().filter(af.id == 3).build().unique();
            QCOMPARE(artistQuery2, artistData4);
        }
        artistQuery1 = dao::_select<SqlV1::Artist>().filter(af.id == 1).build().unique();
        QCOMPARE(artistQuery1, artistData1);
    }
}

void MultiDbTest::multiThreadTest() {
    QEventLoop loop;

    SqlV1::Artist artistData1(1, "The Beatles", 4);
    SqlV1::Artist artistData2(2, "Led Zeppelin", 5);
    dao::_insert<SqlV1::Artist>(SESSION_SQLITE1).build().insert({ artistData1, artistData2 });

    Mys::Artist artistData3(1, "The Beatles2", 14);
    Mys::Artist artistData4(3, "Pink Floyd2", 15);
    dao::_insert<Mys::Artist>(SESSION_MYSQL).build().insert({ artistData3, artistData4 });

    QThread::create([&] {
        QThread::msleep(100);
        DAO_SCOPED_SESSION_BEGIN(SESSION_SQLITE1);

        SqlV1::Artist::Fields af;
        auto artistQuery1 = dao::_select<SqlV1::Artist>().filter(af.id == 2).build().unique();
        QCOMPARE(artistQuery1, artistData2);
    })->start();

    QThread::create([&] {
        QThread::msleep(100);
        DAO_SCOPED_SESSION_BEGIN(SESSION_MYSQL);

        Mys::Artist::Fields af;
        auto artistQuery2 = dao::_select<Mys::Artist>().filter(af.id == 3).build().unique();
        QCOMPARE(artistQuery2, artistData4);

        QThread::msleep(100);
        loop.quit();
    })->start();

    loop.exec();
}

void MultiDbTest::cleanup() {
    dao::_delete<SqlV1::Artist>(SESSION_SQLITE1).build().deleteBy();
    dao::_delete<SqlV2::Track>(SESSION_SQLITE2).build().deleteBy();
    dao::_delete<Mys::Artist>(SESSION_MYSQL).build().deleteBy();
    dao::_delete<Ms::Artist>(SESSION_SQLSERVER).build().deleteBy();
    dao::_delete<Ps::Artist>(SESSION_PSQL).build().deleteBy();
}

void MultiDbTest::cleanupTestCase() {
    dao::ConfigManager::getConfig(SESSION_SQLITE1)->getClient()->dropDatabase();
    dao::ConfigManager::getConfig(SESSION_SQLITE2)->getClient()->dropDatabase();
    dao::ConfigManager::getConfig(SESSION_MYSQL)->getClient()->dropDatabase();
    dao::ConfigManager::getConfig(SESSION_SQLSERVER)->getClient()->dropDatabase();
    dao::ConfigManager::getConfig(SESSION_PSQL)->getClient()->dropDatabase();
}