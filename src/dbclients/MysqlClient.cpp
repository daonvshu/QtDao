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
