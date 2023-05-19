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

#define RL_TB(tb) checkAndRemoveKeywordEscapes(tb, SQLITE_KEYWORDS_ESCAPES)

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
}

QStringList SqliteClient::exportAllTables() {
    auto query = BaseQuery::queryPrimitive("select name from sqlite_master where type = 'table' and name not like 'sqlite_%'");
    QStringList tableNames;
    while (query.next()) {
        tableNames << query.value(0).toString();
    }
    return tableNames;
}

bool SqliteClient::checkTableExist(const QString& tbName) {
    auto query = BaseQuery::queryPrimitive("select *from sqlite_master where type='table' and name = '" % RL_TB(tbName) % "'");
    return query.next();
}

void SqliteClient::createTableIfNotExist(const QString &tbName,
                                         const QStringList &fieldsType,
                                         const QStringList &primaryKeys,
                                         const QString&)
{
    QString str = "create table if not exists " % tbName % "(" % fieldsType.join(",");
    if (primaryKeys.size() > 1) {
        str = str % ", primary key(" % primaryKeys.join(",") % ")";
    }
    str.append(")");

    BaseQuery::queryPrimitive(str);
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
        BaseQuery::queryPrimitive("delete from sqlite_sequence where name = '" % RL_TB(tbName) % "'");
    }
}

QList<QPair<QString, QString>> SqliteClient::exportAllFields(const QString& tbName) {
    QList<QPair<QString, QString>> fields;

    auto query = BaseQuery::queryPrimitive("select name, type from pragma_table_info('" % RL_TB(tbName) % "')");

    while (query.next()) {
        fields << qMakePair(
                query.value(0).toString(),
                query.value(1).toString().toUpper()
                );
    }
    return fields;
}

void SqliteClient::addField(const QString &tbName, const QString &field) {
    BaseQuery::queryPrimitive("alter table " % tbName % " add column " % field);
}

void SqliteClient::dropField(const QString &tbName, const QString &fieldName) {
    Q_ASSERT(dropColumnSupported());
    BaseQuery::queryPrimitive("alter table " % tbName % " drop column " % fieldName);
}

bool SqliteClient::dropColumnSupported() {
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 4)
    return true;
#else
    return false;
#endif
    /*auto query = BaseQuery::queryPrimitive("select sqlite_version()");
    if (query.next()) {
        auto versionStr = query.value(0).toString();
        auto versions = versionStr.split(".");
        if (versions[0].toInt() >= 3 && versions[1].toInt() >= 35) {
            return true;
        }
    }
    return false;*/
}

void SqliteClient::renameField(const QString &tbName, const QString &oldFieldName, const QString &newFieldName) {
    Q_ASSERT(renameColumnSupported());
    BaseQuery::queryPrimitive("alter table " % tbName % " rename column " % oldFieldName % " to " % newFieldName);
}

bool SqliteClient::renameColumnSupported() {
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 1)
    return true;
#else
    return false;
#endif
    /*auto query = BaseQuery::queryPrimitive("select sqlite_version()");
    if (query.next()) {
        auto versionStr = query.value(0).toString();
        auto versions = versionStr.split(".");
        if (versions[0].toInt() >= 3 && versions[1].toInt() >= 25) {
            return true;
        }
    }
    return false;*/
}

QHash<IndexType, QStringList> SqliteClient::exportAllIndexes(const QString &tbName) {
    QHash<IndexType, QStringList> indexes;

    auto query = BaseQuery::queryPrimitive("select name, `unique` from pragma_index_list('" % RL_TB(tbName) % "') where origin = 'c'");
    while (query.next()) {
        indexes[IndexType(query.value(1).toInt())] << query.value(0).toString();
    }

    return indexes;
}

void SqliteClient::createIndex(const QString &tbName,
                               const QString &indexName,
                               const QStringList& fields,
                               IndexType type,
                               const QString&)
{
    QString typeStr = "";
    if (type == IndexType::INDEX_UNIQUE) {
        typeStr = "unique ";
    }

    QString str = "create " % typeStr % "index " % indexName % " on " % tbName % " (" % fields.join(",") % ")";
    BaseQuery::queryPrimitive(str);
}

void SqliteClient::dropIndex(const QString& tbName, const QString& indexName) {
    BaseQuery::queryPrimitive("drop index " % indexName);
}

QString SqliteClient::getIndexFromFields(const QStringList &fields) {
    return AbstractClient::getIndexFromFields(fields, SQLITE_KEYWORDS_ESCAPES);
}

QTDAO_END_NAMESPACE