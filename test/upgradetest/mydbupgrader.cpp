#include "mydbupgrader.h"

#ifndef TEST_DB_CLEAR
#include "table1.h"

void MyDbUpgrader::onUpgrade(int oldVersion, int curVersion) {
    if (entityReader->isTable<Table1>()) {
        return;
    }
    if (oldVersion == 1 && curVersion == 2) {
        upgrade1To2();
    } else if (oldVersion == 2 && curVersion == 3) {
        upgrade2To3();
    } else {
        upgradeWithDataRecovery();
    }
}

void MyDbUpgrader::upgrade1To2() {
    if (config->isSqlite()) {
        upgradeWithDataRecovery();
        return;
    }
    dao::transaction();
    try {
        //drop index
        client->dropIndex(entityReader->getTableName(), "index_number2");
        client->dropIndex(entityReader->getTableName(), "index_number_number2");
        //remove unique index
        client->dropIndex(entityReader->getTableName(), "index_name_number");
        if (TEST_DB == QLatin1String("sqlserver")) {
            client->createIndex(entityReader->getTableName(), entityReader->getNonClusteredIndexFields()[0], dao::IndexType::INDEX_NORMAL);
        } else {
            client->createIndex(entityReader->getTableName(), entityReader->getIndexFields()[0], dao::IndexType::INDEX_NORMAL);
        }
        //drop column
        client->dropField(entityReader->getTableName(), "number2");
        //add new column 'age'
        client->addField(entityReader->getTableName(), entityReader->getFieldsType()[4]);
        dao::commit();
    } catch (dao::DaoException& e) {
        Q_UNUSED(e)
        dao::rollback();
        qFatal("database upgrade fail!");
    }
}

void MyDbUpgrader::upgrade2To3() {
    if (config->isSqlite()) {
        upgradeWithDataRecovery();
        return;
    }
    dao::transaction();
    try {
        //drop index
        client->dropIndex(entityReader->getTableName(), "index_name_number");
        //drop column
        client->dropField(entityReader->getTableName(), "name");
        //add new column 'name'
        client->addField(entityReader->getTableName(), entityReader->getFieldsType()[2]);
        //rename 'number' to 'score'
        client->renameField(entityReader->getTableName(), "number", "score");
        dao::commit();
    } catch (dao::DaoException& e) {
        Q_UNUSED(e)
        dao::rollback();
        qFatal("database upgrade fail!");
    }
}
#else
void MyDbUpgrader::onUpgrade(int oldVersion, int curVersion) {
}

void MyDbUpgrader::upgrade1To2() {
}

void MyDbUpgrader::upgrade2To3() {
}
#endif