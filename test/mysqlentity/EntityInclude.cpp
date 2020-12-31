#include "EntityInclude.h"
#include "MysqlTest1.h"
#include "MysqlTest2.h"
#include "MysqlTest3.h"

#include "../../src/DbLoader.h"
namespace DaoMysql {
    void EntityDelegate::createEntityTables() {
        DbLoader::getClient().createTables<MysqlTest1,MysqlTest2,MysqlTest3>();
    }

    void EntityDelegate::entityTablesUpgrade() {
        DbLoader::getClient().tablesUpgrade<MysqlTest1,MysqlTest2,MysqlTest3>();
    }

    const int entityDelegateId = qRegisterMetaType<EntityDelegate*>();
}

