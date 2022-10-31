#include <qcoreapplication.h>

#include "dao.h"

#ifdef TEST_SQLITE
#include "sqlite/database.h"
#elif defined TEST_MYSQL
#include "mysql/database.h"
#elif defined TEST_SQLSERVER
#include "sqlserver/database.h"
#endif

int main(int argc, char** argv) {
    QCoreApplication a(argc, argv);

#ifdef TEST_SQLITE
    try {
        dao::_config<dao::ConfigSqliteBuilder>()
                .version(1)
                .databaseName("sqlite_keywords_test")
                .initializeDatabase();

    } catch (dao::DaoException& e) {
        Q_UNUSED(e)
        auto validDrivers = QSqlDatabase::drivers();
        Q_UNUSED(validDrivers)
        qFatal("setup database fail!");
    }
#elif defined TEST_MYSQL
    dao::_config<dao::ConfigMysqlBuilder>()
                .version(1)
                .databaseName("mysql_keywords_test")
                .host("localhost")
                .port(3306)
                .user("root")
                .password("root")
                .initializeDatabase();
#elif defined TEST_SQLSERVER
    dao::_config<dao::ConfigSqlServerBuilder>()
                .version(1)
                .databaseName("sqlserver_keywords_test")
                .host("192.168.1.159")
                .user("sa")
                .password("xykj20160315")
                .initializeDatabase();
#endif

    //insert
    {
        DatabaseList databaseList {
          Database(1, "group_num1"),
          Database(2, "group_ku2"),
        };
        dao::_insert<Database>().build().insert(databaseList);
    }

    //test keywords in column
    Database::Fields df;
    dao::_select<Database>()
            .column(df.group, df.column)
            .filter(df.group.like("group_%"))
            .with(_orderBy(df.group))
            .build().list();

    //test update set
    dao::_update<Database>()
            .set(df.column = 10, df.group = "groups")
            .filter(df.column == 1)
            .build().update();

    //test join
    class DatabaseTmp : public dao::self<Database> {};
    DatabaseTmp::Fields dtf;

    dao::_join<Database, DatabaseTmp>()
            .from<Database>()
            .innerJoin<DatabaseTmp>().on(dtf.column == df.column)
            .filter(df.column > 0)
            .build().list();

    dao::globalConfig->getClient()->dropDatabase();

    return 0;
}