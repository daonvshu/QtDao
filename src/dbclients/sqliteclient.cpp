#include "dbclients/sqliteclient.h"

#include "dbexception.h"
#include "connectionpool.h"
#include "query/basequery.h"
#include "config/configsqlite.h"

#include <qstandardpaths.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qstringbuilder.h>

QTDAO_BEGIN_NAMESPACE

#define SQLITE_KEYWORDS_ESCAPES {"\""}

void SqliteClient::testConnect() {
    auto appLocal = dynamic_cast<ConfigSqliteBuilder*>(globalConfig.get())->getDbStoreDirectory();
    QDir dir;
    if (!dir.exists(appLocal)) {
        if (!dir.mkpath(appLocal)) {
            throw DaoException("cannot create sqlite store path! applocal = " + appLocal);
        }
    }
}

void SqliteClient::createDatabase() {
    //auto create when connect database
}

void SqliteClient::dropDatabase() {
    ConnectionPool::closeAllConnection();

    auto appLocal = dynamic_cast<ConfigSqliteBuilder*>(globalConfig.get())->getDbStoreDirectory();
    QFile file(dynamic_cast<ConfigSqliteBuilder*>(globalConfig.get())->getDbStorePath());
    if (file.exists()) {
        if (!file.remove()) {
            throw DaoException("unable remove database file!");
        }
    }
    QDir dir(appLocal);
    if (dir.exists()) {
        dir.rmdir(appLocal);
    }
}

bool SqliteClient::checkTableExist(const QString& tbName) {
    auto str = QString("select *from sqlite_master where type='table' and name = '%1'").arg(
        checkAndRemoveKeywordEscapes(tbName, SQLITE_KEYWORDS_ESCAPES));

    auto query = BaseQuery::queryPrimitive(str);
    return query.next();
}

void SqliteClient::renameTable(const QString& oldName, const QString& newName) {
    BaseQuery::queryPrimitive("alter table " % oldName % " rename to " % newName);
}

void SqliteClient::dropTable(const QString& tbName) {
    BaseQuery::queryPrimitive("drop table if exists " % tbName);
}

void SqliteClient::truncateTable(const QString& tbName) {
    BaseQuery::queryPrimitive("delete from " % tbName);

    if (checkTableExist("sqlite_sequence")) {
        auto str = QString("delete from sqlite_sequence where name = '%1'").arg(checkAndRemoveKeywordEscapes(tbName, SQLITE_KEYWORDS_ESCAPES));
        BaseQuery::queryPrimitive(str);
    }
}

QStringList SqliteClient::getTagTableFields(const QString& tbName) {
    QStringList fields;

    auto query = BaseQuery::queryPrimitive(QString("pragma table_info('%1')")
        .arg(checkAndRemoveKeywordEscapes(tbName, SQLITE_KEYWORDS_ESCAPES)));
    while (query.next()) {
        fields << query.value(1).toString();
    }
    return fields;
}

void SqliteClient::dropAllIndexOnTable(const QString& tbName) {
    auto query = BaseQuery::queryPrimitive(
        QString("select *from sqlite_master where type='index' and tbl_name = '%1'")
            .arg(checkAndRemoveKeywordEscapes(tbName, SQLITE_KEYWORDS_ESCAPES))
    );
    QStringList indexNames;
    while (query.next()) {
        QString indexName = query.value(1).toString();
        if (!indexName.startsWith("index_"))
            continue;
        indexNames << indexName;
    }

    for (const auto& name : indexNames) {
        BaseQuery::queryPrimitive("drop index " % name);
    }
}

void SqliteClient::createTable(dao::EntityReaderInterface *reader) {
    createTableIfNotExist(reader->getTableName(), reader->getFieldsType(), reader->getPrimaryKeys());

    auto indexes = reader->getIndexFields();
    for (const auto& i : indexes) {
        createIndex(reader->getTableName(), i, INDEX_NORMAL);
    }

    indexes = reader->getUniqueIndexFields();
    for (const auto& i : indexes) {
        createIndex(reader->getTableName(), i, INDEX_UNIQUE);
    }
}

void SqliteClient::createTableIfNotExist(const QString& tbName, const QStringList& fieldsType, const QStringList& primaryKeys) {
    QString str = "create table if not exists " % tbName % "(" % fieldsType.join(",");
    if (primaryKeys.size() > 1) {
        str = str % ", primary key(" % primaryKeys.join(",") % ")";
    }
    str.append(")");

    BaseQuery::queryPrimitive(str);
}

void SqliteClient::createIndex(const QString& tbName, const QStringList& fields, IndexType type) {
    QString indexName = "index";
    for (const auto& field : fields) {
        indexName.append("_").append(checkAndRemoveKeywordEscapes(field.split(" ").at(0), SQLITE_KEYWORDS_ESCAPES));
    }
    QString typeStr = "";
    if (type == AbstractClient::INDEX_UNIQUE) {
        typeStr = "unique ";
    }

    QString str = "create " % typeStr % "index " % indexName % " on " % tbName % " (" % fields.join(",") % ")";
    BaseQuery::queryPrimitive(str);
}

QTDAO_END_NAMESPACE