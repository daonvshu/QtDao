#include "SqliteEntityInclude.h"
#include "SqliteTest1.h"
#include "SqliteTest2.h"
#include "SqliteTest3.h"
#include "SqliteDefaultTestInt.h"
#include "SqliteDefaultTestStr.h"

#include "DbLoader.h"
namespace DaoSqlite {
    void SqliteEntityDelegate::createEntityTables() {
        DbLoader::getClient().createTables<ClientSqlite, SqliteTest1, SqliteTest2, SqliteTest3, SqliteDefaultTestInt, SqliteDefaultTestStr>();
    }

    void SqliteEntityDelegate::entityTablesUpgrade() {
        DbLoader::getClient().tablesUpgrade<ClientSqlite, SqliteTest1, SqliteTest2, SqliteTest3, SqliteDefaultTestInt, SqliteDefaultTestStr>();
    }

    const int entitySqliteDelegateId = qRegisterMetaType<SqliteEntityDelegate*>();
}

