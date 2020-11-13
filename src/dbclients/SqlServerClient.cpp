#include "SqlServerClient.h"

void SqlServerClient::testConnect() {
}

void SqlServerClient::createDatabase() {
}

void SqlServerClient::dropDatabase() {
}

bool SqlServerClient::checkTableExist(const QString& tbName) {
    return false;
}

void SqlServerClient::createTableIfNotExist(const QString& tbName, QStringList fieldsType, QStringList primaryKeys) {
}

void SqlServerClient::createIndex(const QString& tbName, QStringList fields, IndexType type) {
}

void SqlServerClient::renameTable(const QString& oldName, const QString& newName) {
}

void SqlServerClient::dropTable(const QString& tbName) {
}

QStringList SqlServerClient::getTagTableFields(const QString& tbName) {
    return QStringList();
}

void SqlServerClient::dropAllIndexOnTable(const QString& tbName) {
}
