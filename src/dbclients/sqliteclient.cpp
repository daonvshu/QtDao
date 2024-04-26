#include "dbclients/sqliteclient.h"

#include "dbexception.h"
#include "connectionpool.h"
#include "query/basequery.h"
#include "config/configsqlite.h"

#include <qstandardpaths.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qstringbuilder.h>
#include <qregularexpression.h>

QTDAO_BEGIN_NAMESPACE

#define SQLITE_KEYWORDS_ESCAPES {"\""}

#define RL_TB(tb) checkAndRemoveKeywordEscapes(tb, SQLITE_KEYWORDS_ESCAPES)

void SqliteClient::testConnect() {
    auto appLocal = dynamic_cast<ConfigSqliteBuilder*>(config)->getDbStoreDirectory();
    QDir dir;
    if (!dir.exists(appLocal)) {
        if (!dir.mkpath(appLocal)) {
            throw DaoException("cannot create sqlite store path! applocal = " + appLocal);
        }
    }
}

void SqliteClient::createDatabase() {
    //auto create when connect database
    enableForeignKey(QString(), true); //enable foreign key
}

void SqliteClient::dropDatabase() {
    ConnectionPool::closeAllConnection();

    QFile file(dynamic_cast<ConfigSqliteBuilder*>(config)->getDbStorePath());
    if (file.exists()) {
        if (!file.remove()) {
            throw DaoException("unable remove database file:" + QFileInfo(file).absoluteFilePath());
        }
    }
}

QStringList SqliteClient::exportAllTables() {
    auto query = BaseQuery::queryPrimitive("select name from sqlite_master where type = 'table' and name not like 'sqlite_%'", {}, currentSessionId());
    QStringList tableNames;
    while (query.next()) {
        tableNames << query.value(0).toString();
    }
    return tableNames;
}

bool SqliteClient::checkTableExist(const QString& tbName) {
    auto query = BaseQuery::queryPrimitive("select *from sqlite_master where type='table' and name = '" % RL_TB(tbName) % "'", {}, currentSessionId());
    return query.next();
}

void SqliteClient::createTableIfNotExist(const QString &tbName,
                                         const QStringList &fieldsType,
                                         const QStringList &primaryKeys,
                                         const QList<ForeignKey>& foreignKeys,
                                         const QString&)
{
    QString str = "create table if not exists " % tbName % "(" % fieldsType.join(",");
    for (const auto& key : foreignKeys) {
        str = str % ", " % translateForeignKeyStatement(key);
    }
    if (primaryKeys.size() > 1) {
        str = str % ", primary key(" % primaryKeys.join(",") % ")";
    }
    str.append(")");

    BaseQuery::queryPrimitive(str, {}, currentSessionId());
}

void SqliteClient::renameTable(const QString& oldName, const QString& newName) {
    BaseQuery::queryPrimitive("alter table " % oldName % " rename to " % newName, {}, currentSessionId());
}

void SqliteClient::dropTable(const QString& tbName) {
    BaseQuery::queryPrimitive("drop table if exists " % tbName, {}, currentSessionId());
}

void SqliteClient::truncateTable(const QString& tbName) {
    BaseQuery::queryPrimitive("delete from " % tbName, {}, currentSessionId());

    if (checkTableExist("sqlite_sequence")) {
        BaseQuery::queryPrimitive("delete from sqlite_sequence where name = '" % RL_TB(tbName) % "'", {}, currentSessionId());
    }
}

void SqliteClient::enableForeignKey(const QString &tbName, bool enabled) {
    if (!tbName.isEmpty()) {
        return;
    }
    BaseQuery::queryPrimitive(QLatin1String("PRAGMA foreign_keys = ") % (enabled ? "ON" : "OFF"), {}, currentSessionId());
}

void SqliteClient::dropReferencedForeignKey(const QString &) {
    //not implement
}

QList<QPair<QString, QString>> SqliteClient::exportAllFields(const QString& tbName) {
    QList<QPair<QString, QString>> fields;

    auto query = BaseQuery::queryPrimitive("select name, type from pragma_table_info('" % RL_TB(tbName) % "')", {}, currentSessionId());

    while (query.next()) {
        fields << qMakePair(
                createEscapeCharsForName(query.value(0).toString()),
                query.value(1).toString().toUpper()
                );
    }
    return fields;
}

void SqliteClient::addField(const QString &tbName, const QString &field) {
    BaseQuery::queryPrimitive("alter table " % tbName % " add column " % field, {}, currentSessionId());
}

void SqliteClient::dropField(const QString &tbName, const QString &fieldName) {
    Q_ASSERT(dropColumnSupported());
    BaseQuery::queryPrimitive("alter table " % tbName % " drop column " % fieldName, {}, currentSessionId());
}

bool SqliteClient::dropColumnSupported() {
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 4)
    return true;
#else
    return false;
#endif
    /*auto query = BaseQuery::queryPrimitive("select sqlite_version()", {}, currentSessionId());
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
    BaseQuery::queryPrimitive("alter table " % tbName % " rename column " % oldFieldName % " to " % newFieldName, {}, currentSessionId());
}

bool SqliteClient::renameColumnSupported() {
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 1)
    return true;
#else
    return false;
#endif
    /*auto query = BaseQuery::queryPrimitive("select sqlite_version()", {}, currentSessionId());
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

    auto query = BaseQuery::queryPrimitive("select name, `unique` from pragma_index_list('" % RL_TB(tbName) % "') where origin = 'c'", {}, currentSessionId());
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
    BaseQuery::queryPrimitive(str, {}, currentSessionId());
}

void SqliteClient::dropIndex(const QString& tbName, const QString& indexName) {
    BaseQuery::queryPrimitive("drop index " % indexName, {}, currentSessionId());
}

QString SqliteClient::getIndexFromFields(const QStringList &fields) {
    return AbstractClient::getIndexFromFields(fields, SQLITE_KEYWORDS_ESCAPES);
}

QString SqliteClient::createEscapeCharsForName(const QString &sourceName) const {
    return '"' % sourceName % '"';
}

QString SqliteClient::removeEscapeCharsForName(const QString &sourceName) const {
    QString newName = sourceName;
    newName.remove(QRegularExpression("['\"\\[\\]`]"));
    return newName;
}

QTDAO_END_NAMESPACE