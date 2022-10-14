#include "sqlserverentityinclude.h"
#include "sqlservertest1.h"
#include "sqlservertest2.h"
#include "sqlservertest3.h"
#include "sqlserverdefaulttestint.h"
#include "sqlserverdefaulttesttime.h"
#include "sqlserverdefaultteststr.h"
#include "sqlserverdefaulttestbytes.h"

#include "dao.h"

QTDAO_USING_NAMESPACE

namespace DaoSqlServer {
    void SqlServerEntityDelegate::createEntityTables() {
        globalConfig->getClient()->createTables<ClientSqlServer, SqlServerTest1, SqlServerTest2, SqlServerTest3, SqlServerDefaultTestInt, SqlServerDefaultTestTime, SqlServerDefaultTestStr, SqlServerDefaultTestBytes>();
    }

    void SqlServerEntityDelegate::entityTablesUpgrade() {
        globalConfig->getClient()->tablesUpgrade<ClientSqlServer, SqlServerTest1, SqlServerTest2, SqlServerTest3, SqlServerDefaultTestInt, SqlServerDefaultTestTime, SqlServerDefaultTestStr, SqlServerDefaultTestBytes>();
    }

    const int entitySqlServerDelegateId = qRegisterMetaType<SqlServerEntityDelegate*>();
}

