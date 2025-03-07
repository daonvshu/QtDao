#include "dbclients/psqlclient.h"

#include "connectionpool.h"
#include "dbexception.h"
#include "query/basequery.h"
#include "config/configbuilder.h"
#include "config/configpsql.h"

#include <qstringbuilder.h>
#include <qregularexpression.h>

QTDAO_BEGIN_NAMESPACE

#define PSQL_KEYWORDS_ESCAPES {"\""}

#define RL_TB(tb) checkAndRemoveKeywordEscapes(tb, PSQL_KEYWORDS_ESCAPES)

void PSqlClient::testConnect() {
    BaseQuery::executePrimitiveQuery("select 1", currentSessionId(), "postgres");
}

void PSqlClient::createDatabase() {
    //check database exist
    {
        bool dbExist = false;
        BaseQuery::executePrimitiveQuery("select datname from pg_database where datname = '" % currentDatabaseName() % "'", currentSessionId(), "postgres", {}, [&] (QSqlQuery& query) {
            dbExist = query.next();
        });
        if (dbExist) {
            return;
        }
    }

    auto psqlConfig = privateConfig<ConfigPSqlBuilder>();
    QString statement = "CREATE DATABASE " % currentDatabaseName() % " WITH";
    if (!psqlConfig->mOwner.isEmpty()) {
        statement = statement % " OWNER=" % psqlConfig->mOwner;
    }
    if (!psqlConfig->mEncoding.isEmpty()) {
        statement = statement % " ENCODING=" % psqlConfig->mEncoding;
    }
    if (!psqlConfig->mCollate.isEmpty()) {
        statement = statement % " LC_COLLATE=" % psqlConfig->mCollate;
    }
    if (!psqlConfig->mCType.isEmpty()) {
        statement = statement % " LC_CTYPE=" % psqlConfig->mCType;
    }
    if (!psqlConfig->mTableSpace.isEmpty()) {
        statement = statement % " TABLESPACE=" % psqlConfig->mTableSpace;
    }
    BaseQuery::executePrimitiveQuery(statement, currentSessionId(), "postgres");
}

void PSqlClient::dropDatabase() {
    ConnectionPool::closeAllConnection();
    BaseQuery::executePrimitiveQuery("drop database if exists " % currentDatabaseName(), currentSessionId(), "postgres");
}

QStringList PSqlClient::exportAllTables() {
    auto query = BaseQuery::queryPrimitive("select tablename from pg_catalog.pg_tables where schemaname = 'public'", {}, currentSessionId());
    QStringList tableNames;
    while (query.next()) {
        tableNames << query.value(0).toString();
    }
    return tableNames;
}

bool PSqlClient::checkTableExist(const QString &tbName) {
    auto query = BaseQuery::queryPrimitive("select table_name from information_schema.TABLES where table_name ='"
                                           % RL_TB(tbName) % "' and table_schema = 'public'", {}, currentSessionId());
    return query.next();
}

void PSqlClient::createTableIfNotExist(const QString &tbName, const QStringList &fieldsType, const QStringList &primaryKeys, const QList<ForeignKey> &foreignKeys, const QString &engine) {
    QString str = "create table if not exists \"" % tbName % "\"(" % fieldsType.join(",");
    for (const auto& key : foreignKeys) {
        str = str % ", " % translateForeignKeyStatement(key);
    }
    if (primaryKeys.size() > 1) {
        str = str % ", primary key(" % primaryKeys.join(",") % ")";
    }
    str.append(")");

    BaseQuery::queryPrimitive(str, {}, currentSessionId());
}

void PSqlClient::renameTable(const QString &oldName, const QString &newName) {
    BaseQuery::queryPrimitive("alter table \"" % oldName % "\" rename to " % newName, {}, currentSessionId());
}

void PSqlClient::dropTable(const QString &tbName) {
    BaseQuery::queryPrimitive("drop table if exists \"" % tbName % "\"", {}, currentSessionId());
}

void PSqlClient::truncateTable(const QString &tbName) {
    BaseQuery::queryPrimitive("truncate table \"" % tbName % "\" restart identity", {}, currentSessionId());
}

void PSqlClient::enableForeignKey(const QString &tbName, bool enabled) {
    if (tbName.isEmpty()) {
        return;
    }
    BaseQuery::queryPrimitive("alter table \"" % tbName % "\"" % (enabled ? "enable" : "disable") % " trigger all", {}, currentSessionId());
}

void PSqlClient::dropReferencedForeignKey(const QString &tbName) {
    if (tbName.isEmpty()) {
        return;
    }
    QStringList referenceChildTbName, foreignKeyName;
    {
        auto query = BaseQuery::queryPrimitive(
        QStringLiteral("select refcls.relname, con.conname from pg_constraint as con "
                "join pg_class cls on con.confrelid = cls.oid "
                "left join pg_class refcls on con.conrelid = refcls.oid "
                "where cls.relname = '%1' and contype = 'f'").arg(tbName), {}, currentSessionId());
        while (query.next()) {
            referenceChildTbName.append(query.value(0).toString());
            foreignKeyName.append(query.value(1).toString());
        }
    }
    for (int i = 0; i < referenceChildTbName.size(); i++) {
        BaseQuery::queryPrimitive(QStringLiteral("ALTER TABLE %1 DROP CONSTRAINT %2")
                                  .arg(referenceChildTbName[i], foreignKeyName[i]), {}, currentSessionId());
    }
}

QList<QPair<QString, QString>> PSqlClient::exportAllFields(const QString &tbName) {
    QList<QPair<QString, QString>> fields;

    auto query = BaseQuery::queryPrimitive("select \"column_name\", data_type from information_schema.\"columns\" where "
                                           "table_name = '" % RL_TB(tbName) % "' and table_schema = 'public'", {}, currentSessionId());
    while (query.next()) {
        fields << qMakePair(
            createEscapeCharsForName(query.value(0).toString()),
            query.value(1).toString().toUpper()
            );
    }
    return fields;
}

void PSqlClient::addField(const QString &tbName, const QString &field) {
    BaseQuery::queryPrimitive("alter table \"" % tbName % "\" add column " % field, {}, currentSessionId());
}

void PSqlClient::dropField(const QString &tbName, const QString &fieldName) {
    BaseQuery::queryPrimitive("alter table \"" % tbName % "\" drop column " % fieldName, {}, currentSessionId());
}

void PSqlClient::renameField(const QString &tbName, const QString &oldFieldName, const QString &newFieldName) {
    BaseQuery::queryPrimitive("alter table \"" % tbName % "\" rename column " % oldFieldName % " to " % newFieldName, {}, currentSessionId());
}

QHash<IndexType, QStringList> PSqlClient::exportAllIndexes(const QString &tbName) {
    QHash<IndexType, QStringList> indexes;

    auto query = BaseQuery::queryPrimitive(QStringLiteral("select idxcls.relname, idx.indisunique from pg_index idx"
            " join pg_class cls on idx.indrelid = cls.oid"
            " join pg_class idxcls on idx.indexrelid = idxcls.oid"
            " left join pg_am am on idxcls.relam = am.oid"
            " where cls.relname = '%1' and starts_with(idxcls.relname, '%1_index_')").arg(tbName));
    while (query.next()) {
        auto indexName = query.value(0).toString();
        auto isUnique = query.value(1).toBool();
        indexes[IndexType(isUnique)] << indexName;
    }

    return indexes;
}

void PSqlClient::createIndex(const QString &tbName, const QString &indexName, const QStringList &fields, IndexType type, const QString &indexOption) {
    QString typeStr = "";
    if (type == IndexType::INDEX_UNIQUE) {
        typeStr = "unique ";
    }

    QString str = "create " % typeStr % "index " % indexName % " on \"" % tbName % "\" (" % fields.join(",") % ")";
    BaseQuery::queryPrimitive(str, {}, currentSessionId());
}

void PSqlClient::dropIndex(const QString &tbName, const QString &indexName) {
    BaseQuery::queryPrimitive("drop index if exists " % indexName, {}, currentSessionId());
}

QString PSqlClient::getIndexFromFields(const QString &tbName, const QStringList &fields) {
    return tbName.toLower() % "_" % AbstractClient::getIndexFromFields(tbName, fields, PSQL_KEYWORDS_ESCAPES);
}

QString PSqlClient::createEscapeCharsForName(const QString &sourceName) const {
    return "\"" % sourceName % "\"";
}

QString PSqlClient::removeEscapeCharsForName(const QString &sourceName) const {
    QString newName = sourceName;
    static QRegularExpression regex(R"(['\"\\])");
    newName.remove(regex);
    return newName;
}

QTDAO_END_NAMESPACE