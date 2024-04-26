#include "dbclients/mysqlclient.h"

#include "connectionpool.h"
#include "dbexception.h"
#include "query/basequery.h"
#include "config/configbuilder.h"

#include <qstringbuilder.h>
#include <qregularexpression.h>

QTDAO_BEGIN_NAMESPACE

#define MYSQL_KEYWORDS_ESCAPES {"`"}

#define RL_TB(tb) checkAndRemoveKeywordEscapes(tb, MYSQL_KEYWORDS_ESCAPES)

void MysqlClient::testConnect() {
    BaseQuery::executePrimitiveQuery("select 1", currentSessionId(), "mysql");
}

void MysqlClient::createDatabase() {
    BaseQuery::executePrimitiveQuery("create database if not exists " % currentDatabaseName() %
        " default character set utf8mb4 COLLATE utf8mb4_general_ci", currentSessionId(), "mysql");
}

void MysqlClient::dropDatabase() {
    ConnectionPool::closeAllConnection();
    BaseQuery::executePrimitiveQuery("drop database if exists " % currentDatabaseName(), currentSessionId(), "mysql");
}

QStringList MysqlClient::exportAllTables() {
    auto query = BaseQuery::queryPrimitive("select table_name from information_schema.tables where table_schema = '"
                                           % currentDatabaseName() % "'", {}, currentSessionId());
    QStringList tableNames;
    while (query.next()) {
        tableNames << query.value(0).toString();
    }
    return tableNames;
}

bool MysqlClient::checkTableExist(const QString& tbName) {
    auto query = BaseQuery::queryPrimitive("select table_name from information_schema.TABLES where table_name ='"
                                           % RL_TB(tbName) % "' and table_schema = '" % currentDatabaseName() % "'", {}, currentSessionId());
    return query.next();
}

void MysqlClient::createTableIfNotExist(const QString &tbName,
                                        const QStringList &fieldsType,
                                        const QStringList &primaryKeys,
                                        const QList<ForeignKey>& foreignKeys,
                                        const QString &engine)
{
    QString str = "create table if not exists " % tbName % "(" % fieldsType.join(",");
    for (const auto& key : foreignKeys) {
        str = str % ", " % translateForeignKeyStatement(key);
    }
    if (primaryKeys.size() > 1) {
        str = str % ", primary key(" % primaryKeys.join(",") % ")";
    }
    str.append(")");

    if (!engine.isEmpty()) {
        str = str % " engine=" % engine;
    }
    str.append(" default charset = utf8mb4");

    BaseQuery::queryPrimitive(str, {}, currentSessionId());
}

void MysqlClient::renameTable(const QString& oldName, const QString& newName) {
    BaseQuery::queryPrimitive("rename table " % oldName % " to " % newName, {}, currentSessionId());
}

void MysqlClient::dropTable(const QString& tbName) {
    BaseQuery::queryPrimitive("drop table if exists " % tbName, {}, currentSessionId());
}

void MysqlClient::truncateTable(const QString& tbName) {
    BaseQuery::queryPrimitive("truncate table " % tbName, {}, currentSessionId());
}

void MysqlClient::enableForeignKey(const QString &tbName, bool enabled) {
    if (!tbName.isEmpty()) {
        return;
    }
    BaseQuery::queryPrimitive(QLatin1String("SET FOREIGN_KEY_CHECKS=") % (enabled ? "1" : "0"), {}, currentSessionId());
}

void MysqlClient::dropReferencedForeignKey(const QString &tbName) {
    if (tbName.isEmpty()) {
        return;
    }
    QStringList referenceChildTbName, foreignKeyName;
    {
        auto query = BaseQuery::queryPrimitive(
                QStringLiteral("select table_name, constraint_name from information_schema.KEY_COLUMN_USAGE "
                              "where referenced_table_name='%1' group by table_name, constraint_name").arg(tbName), {}, currentSessionId());
        while (query.next()) {
            referenceChildTbName.append(query.value(0).toString());
            foreignKeyName.append(query.value(1).toString());
        }
    }

    for (int i = 0; i < referenceChildTbName.size(); i++) {
        BaseQuery::queryPrimitive(QStringLiteral("ALTER TABLE %1 DROP FOREIGN KEY %2")
                                          .arg(referenceChildTbName[i], foreignKeyName[i]), {}, currentSessionId());
    }
}

QList<QPair<QString, QString>> MysqlClient::exportAllFields(const QString& tbName) {
    QList<QPair<QString, QString>> fields;

    auto query = BaseQuery::queryPrimitive("select COLUMN_NAME, DATA_TYPE from information_schema.COLUMNS where "
                                           "table_name = '" % RL_TB(tbName) % "' and table_schema = '" % currentDatabaseName() % "'", {}, currentSessionId());
    while (query.next()) {
        fields << qMakePair(
                createEscapeCharsForName(query.value(0).toString()),
                query.value(1).toString().toUpper()
                );
    }
    return fields;
}

void MysqlClient::addField(const QString &tbName, const QString &field) {
    BaseQuery::queryPrimitive("alter table " % tbName % " add " % field, {}, currentSessionId());
}

void MysqlClient::dropField(const QString &tbName, const QString &fieldName) {
    BaseQuery::queryPrimitive("alter table " % tbName % " drop " % fieldName, {}, currentSessionId());
}

void MysqlClient::renameField(const QString &tbName, const QString &oldFieldName, const QString &newFieldName) {
    BaseQuery::queryPrimitive("alter table " % tbName % " rename column " % oldFieldName % " to " % newFieldName, {}, currentSessionId());
}

QHash<IndexType, QStringList> MysqlClient::exportAllIndexes(const QString &tbName) {
    QHash<IndexType, QStringList> indexes;

    auto query = BaseQuery::queryPrimitive("SELECT INDEX_NAME, NON_UNIQUE FROM INFORMATION_SCHEMA.STATISTICS "
                                           "WHERE TABLE_NAME = '" % RL_TB(tbName) % "' AND TABLE_SCHEMA = '"
                                            % currentDatabaseName() % "' GROUP BY INDEX_NAME, NON_UNIQUE", {}, currentSessionId());
    while (query.next()) {
        auto indexName = query.value(0).toString();
        if (indexName.startsWith("index_")) {
            indexes[IndexType(1 - query.value(1).toInt())] << indexName;
        }
    }

    return indexes;
}

void MysqlClient::createIndex(const QString &tbName,
                              const QString &indexName,
                              const QStringList& fields,
                              dao::IndexType type,
                              const QString&) {
    QString typeStr = "";
    if (type == IndexType::INDEX_UNIQUE) {
        typeStr = "unique ";
    }

    QString str = "create " % typeStr % "index " % indexName % " on " % tbName % " (" % fields.join(",") % ")";
    BaseQuery::queryPrimitive(str, {}, currentSessionId());
}

void MysqlClient::dropIndex(const QString& tbName, const QString& indexName) {
    BaseQuery::queryPrimitive("drop index " % indexName % " on " % tbName, {}, currentSessionId());
}

QString MysqlClient::getIndexFromFields(const QStringList &fields) {
    return AbstractClient::getIndexFromFields(fields, MYSQL_KEYWORDS_ESCAPES);
}

QString MysqlClient::createEscapeCharsForName(const QString &sourceName) const {
    return "`" % sourceName % "`";
}

QString MysqlClient::removeEscapeCharsForName(const QString &sourceName) const {
    QString newName = sourceName;
    newName.remove(QRegularExpression("[`]"));
    return newName;
}

QTDAO_END_NAMESPACE