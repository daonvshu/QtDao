#include "SqlServerClient.h"

void SqlServerClient::testConnect() {
}

void SqlServerClient::createDatabase() {
}

void SqlServerClient::dropDatabase() {
}

bool SqlServerClient::checkTableExist(const QString& tbName) {
    Q_UNUSED(tbName);
    return false;
}

void SqlServerClient::createTableIfNotExist(const QString& tbName, QStringList fieldsType, QStringList primaryKeys) {
    Q_UNUSED(tbName);
    Q_UNUSED(fieldsType);
    Q_UNUSED(primaryKeys);
}

void SqlServerClient::createIndex(const QString& tbName, QStringList fields, IndexType type) {
    Q_UNUSED(tbName);
    Q_UNUSED(fields);
    Q_UNUSED(type);
}

void SqlServerClient::renameTable(const QString& oldName, const QString& newName) {
    Q_UNUSED(oldName);
    Q_UNUSED(newName);
}

void SqlServerClient::dropTable(const QString& tbName) {
    Q_UNUSED(tbName);
}

void SqlServerClient::truncateTable(const QString& tbName) {
    Q_UNUSED(tbName);
}

QStringList SqlServerClient::getTagTableFields(const QString& tbName) {
    Q_UNUSED(tbName);
    return QStringList();
}

void SqlServerClient::dropAllIndexOnTable(const QString& tbName) {
    Q_UNUSED(tbName);
}

