#include "mysqlentityinclude.h"
#include "mysqltest1.h"
#include "mysqltest2.h"
#include "mysqltest3.h"
#include "mysqldefaulttestint.h"
#include "mysqldefaulttesttime.h"
#include "mysqldefaultteststr.h"

#include "dbloader.h"

QTDAO_USING_NAMESPACE

namespace DaoMysql {
    void MysqlEntityDelegate::createEntityTables() {
        DbLoader::getClient().createTables<ClientMysql, MysqlTest1, MysqlTest2, MysqlTest3, MysqlDefaultTestInt, MysqlDefaultTestTime, MysqlDefaultTestStr>();
    }

    void MysqlEntityDelegate::entityTablesUpgrade() {
        DbLoader::getClient().tablesUpgrade<ClientMysql, MysqlTest1, MysqlTest2, MysqlTest3, MysqlDefaultTestInt, MysqlDefaultTestTime, MysqlDefaultTestStr>();
    }

    const int entityMysqlDelegateId = qRegisterMetaType<MysqlEntityDelegate*>();
}

