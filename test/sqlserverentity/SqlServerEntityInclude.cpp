#include "SqlServerEntityInclude.h"
#include "SqlServerTest1.h"
#include "SqlServerTest2.h"
#include "SqlServerTest3.h"
#include "SqlServerDefaultTestInt.h"
#include "SqlServerDefaultTestTime.h"
#include "SqlServerDefaultTestStr.h"
#include "SqlServerDefaultTestBytes.h"

#include "DbLoader.h"
namespace DaoSqlServer {
    void SqlServerEntityDelegate::createEntityTables() {
        DbLoader::getClient().createTables<ClientSqlServer, SqlServerTest1, SqlServerTest2, SqlServerTest3, SqlServerDefaultTestInt, SqlServerDefaultTestTime, SqlServerDefaultTestStr, SqlServerDefaultTestBytes>();
    }

    void SqlServerEntityDelegate::entityTablesUpgrade() {
        DbLoader::getClient().tablesUpgrade<ClientSqlServer, SqlServerTest1, SqlServerTest2, SqlServerTest3, SqlServerDefaultTestInt, SqlServerDefaultTestTime, SqlServerDefaultTestStr, SqlServerDefaultTestBytes>();
    }

    const int entitySqlServerDelegateId = qRegisterMetaType<SqlServerEntityDelegate*>();
}

