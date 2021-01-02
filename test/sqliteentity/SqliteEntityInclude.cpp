#include "SqliteEntityInclude.h"
#include "SqliteTest1.h"
#include "SqliteTest2.h"
#include "SqliteTest3.h"

#include "../../src/DbLoader.h"
namespace DaoSqlite {
    void SqliteEntityDelegate::createEntityTables() {
        DbLoader::getClient().createTables<ClientSqlite, SqliteTest1, SqliteTest2, SqliteTest3>();
    }

    void SqliteEntityDelegate::entityTablesUpgrade() {
        DbLoader::getClient().tablesUpgrade<ClientSqlite, SqliteTest1, SqliteTest2, SqliteTest3>();
    }

    const int entitySqliteDelegateId = qRegisterMetaType<SqliteEntityDelegate*>();
}

