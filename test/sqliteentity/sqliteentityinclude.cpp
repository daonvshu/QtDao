#include "sqliteentityinclude.h"
#include "sqlitetest1.h"
#include "sqlitetest2.h"
#include "sqlitetest3.h"
#include "sqlitedefaulttestint.h"
#include "sqlitedefaultteststr.h"

#include "dao.h"

QTDAO_USING_NAMESPACE

namespace DaoSqlite {
    void SqliteEntityDelegate::createEntityTables() {
        globalConfig->getClient()->createTables<ClientSqlite, SqliteTest1, SqliteTest2, SqliteTest3, SqliteDefaultTestInt, SqliteDefaultTestStr>();
    }

    void SqliteEntityDelegate::entityTablesUpgrade() {
        globalConfig->getClient()->tablesUpgrade<ClientSqlite, SqliteTest1, SqliteTest2, SqliteTest3, SqliteDefaultTestInt, SqliteDefaultTestStr>();
    }

    const int entitySqliteDelegateId = qRegisterMetaType<SqliteEntityDelegate*>();
}

