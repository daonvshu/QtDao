#include "MysqlEntityInclude.h"
#include "MysqlTest1.h"
#include "MysqlTest2.h"
#include "MysqlTest3.h"

#include "DbLoader.h"
namespace DaoMysql {
    void MysqlEntityDelegate::createEntityTables() {
        DbLoader::getClient().createTables<ClientMysql, MysqlTest1, MysqlTest2, MysqlTest3>();
    }

    void MysqlEntityDelegate::entityTablesUpgrade() {
        DbLoader::getClient().tablesUpgrade<ClientMysql, MysqlTest1, MysqlTest2, MysqlTest3>();
    }

    const int entityMysqlDelegateId = qRegisterMetaType<MysqlEntityDelegate*>();
}

