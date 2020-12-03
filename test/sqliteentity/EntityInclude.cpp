#include "EntityInclude.h"
#include "SqliteTest1.h"
#include "SqliteTest2.h"
#include "SqliteTest3.h"

#include "../../src/DbLoader.h"
namespace DaoSqlite {
    void EntityDelegate::createEntityTables() {
        DbLoader::getClient().createTables<SqliteTest1,SqliteTest2,SqliteTest3>();
    }

    void EntityDelegate::entityTablesUpgrade() {
        DbLoader::getClient().tablesUpgrade<SqliteTest1,SqliteTest2,SqliteTest3>();
    }

    const int entityDelegateId = qRegisterMetaType<EntityDelegate*>();
}

