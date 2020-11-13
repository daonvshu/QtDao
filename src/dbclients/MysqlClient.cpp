#include "MysqlClient.h"

void MysqlClient::testConnect() {
}

void MysqlClient::createDatabase() {
}

void MysqlClient::dropDatabase() {
}

bool MysqlClient::checkTableExist(const QString& tbName) {
    return false;
}

void MysqlClient::createTableIfNotExist(const QString& tbName, QStringList fieldsType, QStringList primaryKeys) {
}

void MysqlClient::createIndex(const QString& tbName, QStringList fields, IndexType type) {
}

void MysqlClient::renameTable(const QString& oldName, const QString& newName) {
}

void MysqlClient::dropTable(const QString& tbName) {
}

QStringList MysqlClient::getTagTableFields(const QString& tbName) {
    return QStringList();
}

void MysqlClient::dropAllIndexOnTable(const QString& tbName) {
}
