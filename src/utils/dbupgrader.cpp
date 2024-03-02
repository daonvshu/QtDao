#include "utils/dbupgrader.h"

#include "dao.h"

#include "config/configbuilder.h"

QTDAO_BEGIN_NAMESPACE

DatabaseUpgrader::DatabaseUpgrader()
    : entityReader(nullptr)
{

}

void DatabaseUpgrader::setEntityReader(EntityReaderInterface *reader) {
    entityReader = reader;
}

void DatabaseUpgrader::setCurClient(dao::AbstractClient *curClient) {
    client = curClient;
}

void DatabaseUpgrader::setCurConfig(dao::ConfigBuilder *curConfig) {
    config = curConfig;
}

void DatabaseUpgrader::onUpgrade(int oldVersion, int curVersion) {
    //default upgrade strategy
    client->enableForeignKey(QString(), false);
    upgradeWithDataRecovery();
    client->enableForeignKey(QString(), true);
}

void DatabaseUpgrader::upgradeWithDataRecovery() {
    QString tmpTableName = "tmp_" + entityReader->getTableName();

    client->enableForeignKey(entityReader->getTableName(), false);
    dao::transcation();
    try {
        //drop tmp table if exist
        client->dropTable(tmpTableName);
        //create a tmp table
        client->createTableIfNotExist(tmpTableName,
                                      entityReader->getFieldsType(),
                                      entityReader->getPrimaryKeys(),
                                      entityReader->getForeignKeys(),
                                      entityReader->getTableEngine());
        //copy data
        client->transferData(entityReader->getTableName(), tmpTableName);
        //remove old table
        client->dropTable(entityReader->getTableName());
        //rename tmp table into target table
        client->renameTable(tmpTableName, entityReader->getTableName());
        //create index for new table
        client->createIndex(entityReader);

        dao::commit();
    } catch (DaoException& e) {
        dao::rollback();
        client->enableForeignKey(entityReader->getTableName(), true);
        throw e;
    }
    client->enableForeignKey(entityReader->getTableName(), true);
}

QTDAO_END_NAMESPACE