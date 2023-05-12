#include "utils/dbupgrader.h"

#include "config/configbuilder.h"

QTDAO_BEGIN_NAMESPACE

DatabaseUpgrader::DatabaseUpgrader()
    : entityReader(nullptr)
    , currentDatabaseType(ConfigType::Sqlite)
{

}

void DatabaseUpgrader::setEntityReader(dao::EntityReaderInterface *reader) {
    entityReader = reader;
}

void DatabaseUpgrader::setConfigType(ConfigType type) {
    currentDatabaseType = type;
}

void DatabaseUpgrader::onUpgrade(int oldVersion, int curVersion) {
    //
    QString tmpTableName = "tmp_" + entityReader->getTableName();
    //current config client
    auto client = globalConfig->getClient();
    //drop tmp table if exist
    client->dropTable(tmpTableName);
    //clear current table index
    client->dropAllIndexOnTable(entityReader->getTableName());
    //rename current table to temporary table
    client->renameTable(entityReader->getTableName(), tmpTableName);
    //create new table
    client->createTable(entityReader);
    //move temporary table data to new table
    if (currentDatabaseType == ConfigType::SqlServer) {
        //remove timestamp fields, cannot insert directly
        client->restoreData2NewTable(entityReader->getTableName(), entityReader->getFieldsWithoutTimestamp());
    } else {
        client->restoreData2NewTable(entityReader->getTableName(), entityReader->getFields());
    }
    //drop temporary table
    client->dropTable(tmpTableName);
}

QTDAO_END_NAMESPACE