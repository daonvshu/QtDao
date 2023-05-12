#include "dbclients/mysqlclient.h"

#include "connectionpool.h"
#include "dbexception.h"
#include "query/basequery.h"
#include "config/configbuilder.h"

#include <qstringbuilder.h>

QTDAO_BEGIN_NAMESPACE

#define MYSQL_KEYWORDS_ESCAPES {"`"}

void MysqlClient::testConnect() {
    BaseQuery::executePrimitiveQuery("select 1", "mysql");
}

void MysqlClient::createDatabase() {
    BaseQuery::executePrimitiveQuery("create database if not exists " % currentDatabaseName() %
        " default character set utf8mb4 COLLATE utf8mb4_general_ci", "mysql");
}

void MysqlClient::dropDatabase() {
    ConnectionPool::closeAllConnection();
    BaseQuery::executePrimitiveQuery("drop database if exists " % currentDatabaseName(), "mysql");
}

bool MysqlClient::checkTableExist(const QString& tbName) {
    auto str = QString("select table_name from information_schema.TABLES where table_name ='%1' and table_schema = '%2'")
        .arg(checkAndRemoveKeywordEscapes(tbName, MYSQL_KEYWORDS_ESCAPES), currentDatabaseName());

    auto query = BaseQuery::queryPrimitive(str);
    return query.next();
}

void MysqlClient::renameTable(const QString& oldName, const QString& newName) {
    BaseQuery::queryPrimitive("rename table " % oldName % " to " % newName);
}

void MysqlClient::dropTable(const QString& tbName) {
    BaseQuery::queryPrimitive("drop table if exists " % tbName);
}

void MysqlClient::truncateTable(const QString& tbName) {
    BaseQuery::queryPrimitive("truncate table " % tbName);
}

QStringList MysqlClient::getTagTableFields(const QString& tbName) {
    auto str = 
        QString("select COLUMN_NAME from information_schema.COLUMNS where table_name = '%1' and table_schema = '%2';")
    .arg(checkAndRemoveKeywordEscapes(tbName, MYSQL_KEYWORDS_ESCAPES), currentDatabaseName());

    QStringList fields;

    auto query = BaseQuery::queryPrimitive(str);
    while (query.next()) {
        fields << query.value(0).toString();
    }
    return fields;
}

void MysqlClient::dropAllIndexOnTable(const QString& tbName) {
    auto query = BaseQuery::queryPrimitive(
        QString("SELECT index_name FROM information_schema.statistics where TABLE_SCHEMA = '%1' and TABLE_NAME = '%2' GROUP BY index_name")
        .arg(currentDatabaseName(), checkAndRemoveKeywordEscapes(tbName, MYSQL_KEYWORDS_ESCAPES))
    );
    QStringList indexNames;
    while (query.next()) {
        QString indexName = query.value(0).toString();
        if (!indexName.startsWith("index_"))
            continue;
        indexNames << indexName;
    }
    for (const auto& name : indexNames) {
        BaseQuery::queryPrimitive("drop index " % name % " on " % tbName);
    }
}

void MysqlClient::createTable(dao::EntityReaderInterface *reader) {
    createTableIfNotExist(reader->getTableName(), reader->getTableEngine(), reader->getFieldsType(), reader->getPrimaryKeys());

    auto indexes = reader->getIndexFields();
    for (const auto& i : indexes) {
        createIndex(reader->getTableName(), i, INDEX_NORMAL);
    }

    indexes = reader->getUniqueIndexFields();
    for (const auto& i : indexes) {
        createIndex(reader->getTableName(), i, INDEX_UNIQUE);
    }
}

void MysqlClient::createTableIfNotExist(const QString& tbName, const QString& engine, const QStringList& fieldsType, const QStringList& primaryKeys) {
    QString str = "create table if not exists " % tbName % "(" % fieldsType.join(",");
    if (primaryKeys.size() > 1) {
        str = str % ", primary key(" % primaryKeys.join(",") % ")";
    }
    str.append(")");

    if (!engine.isEmpty()) {
        str = str % " engine=" % engine;
    }
    str.append(" default charset = utf8mb4");

    BaseQuery::queryPrimitive(str);
}

void MysqlClient::createIndex(const QString& tbName, const QStringList& fields, IndexType type) {
    QString indexName = "index";
    for (const auto& field : fields) {
        indexName.append("_").append(checkAndRemoveKeywordEscapes(field.split(" ").at(0), MYSQL_KEYWORDS_ESCAPES));
    }
    QString typeStr = "";
    if (type == AbstractClient::INDEX_UNIQUE) {
        typeStr = "unique ";
    }

    QString str = "create " % typeStr % "index " % indexName % " on " % tbName % " (" % fields.join(",") % ")";
    BaseQuery::queryPrimitive(str);
}

QTDAO_END_NAMESPACE