#include "MysqlClient.h"

void MysqlClient::testConnect() {
}

void MysqlClient::createDatabase() {
}

void MysqlClient::dropDatabase() {
}

bool MysqlClient::checkTableExist(const QString& tbName) {
    Q_UNUSED(tbName);
    return false;
}

void MysqlClient::createTableIfNotExist(const QString& tbName, QStringList fieldsType, QStringList primaryKeys) {
    Q_UNUSED(tbName);
    Q_UNUSED(fieldsType);
    Q_UNUSED(primaryKeys);
}

void MysqlClient::createIndex(const QString& tbName, QStringList fields, IndexType type) {
    Q_UNUSED(tbName);
    Q_UNUSED(fields);
    Q_UNUSED(type);
}

void MysqlClient::renameTable(const QString& oldName, const QString& newName) {
    Q_UNUSED(oldName);
    Q_UNUSED(newName);
}

void MysqlClient::dropTable(const QString& tbName) {
    Q_UNUSED(tbName);
}

void MysqlClient::truncateTable(const QString& tbName) {
    Q_UNUSED(tbName);
}

QStringList MysqlClient::getTagTableFields(const QString& tbName) {
    Q_UNUSED(tbName);
    return QStringList();
}

void MysqlClient::dropAllIndexOnTable(const QString& tbName) {
    Q_UNUSED(tbName);
}
