#include "dbclients/sqlserverclient.h"

#include "connectionpool.h"
#include "dbexception.h"
#include "query/basequery.h"
#include "config/configbuilder.h"

#include <qcoreapplication.h>
#include <qstandardpaths.h>
#include <qstringbuilder.h>

QTDAO_BEGIN_NAMESPACE

#define SQLSERVER_KEYWORDS_ESCAPES {"[", "]"}

#define RL_TB(tb) checkAndRemoveKeywordEscapes(tb, SQLSERVER_KEYWORDS_ESCAPES)

void SqlServerClient::testConnect() {
    BaseQuery::executePrimitiveQuery("select 1", "master");
}

void SqlServerClient::createDatabase() {

    QString storePath;
    BaseQuery::executePrimitiveQuery("SELECT physical_name FROM sys.master_files where name = 'master'", "master", "",
                                     [&] (QSqlQuery& query) {
        if (query.next()) {
            storePath = query.value("physical_name").toString();
            if (!storePath.isEmpty()) {
                storePath = storePath.remove("master.mdf");
            }
        }
    });

    if (storePath.isEmpty()) {
        throw DaoException("cannot create sqlserver store path! path = " % storePath %
                           " , cannot read master table physical path!");
    }

    QString sql = ""
                "IF DB_ID(N'%1') IS NULL\n"
                "   CREATE DATABASE [%1]\n"
                "   ON\n"
                "   PRIMARY\n"
                "   (\n"
                "      NAME = N'%1',\n"
                "      FILENAME = N'%2%1.ndf',\n"
                "      SIZE = 8MB,\n"
                "      MAXSIZE = UNLIMITED,\n"
                "      FILEGROWTH = 10%\n"
                "   )\n"
                "   LOG ON\n"
                "   (\n"
                "      NAME = N'%1_log',\n"
                "      FILENAME = N'%2%1_log.ldf',\n"
                "      SIZE = 8MB,\n"
                "      MAXSIZE = UNLIMITED,\n"
                "      FILEGROWTH = 10%\n"
                "   )"
                ;
    sql = sql.arg(currentDatabaseName(), storePath);
    BaseQuery::executePrimitiveQuery(sql, "master");
}

void SqlServerClient::dropDatabase() {
    ConnectionPool::closeAllConnection();
    BaseQuery::executePrimitiveQuery("if exists (select* from sysdatabases where name ='" % currentDatabaseName() %
                                     "') drop database " % currentDatabaseName(), "master");
}

QStringList SqlServerClient::exportAllTables() {
    auto query = BaseQuery::queryPrimitive("select table_name from information_schema.tables where table_catalog = '"
                                           % currentDatabaseName() % "' and table_type = 'BASE TABLE'");
    QStringList tableNames;
    while (query.next()) {
        tableNames << query.value(0).toString();
    }
    return tableNames;
}

bool SqlServerClient::checkTableExist(const QString& tbName) {
    auto query = BaseQuery::queryPrimitive("select * from sys.tables where name = '" % RL_TB(tbName) % "' and type = 'U'");
    return query.next();
}

void SqlServerClient::createTableIfNotExist(const QString &tbName,
                                            const QStringList &fieldsType,
                                            const QStringList &primaryKeys,
                                            const QList<ForeignKey>& foreignKeys,
                                            const QString &) {
    QString str = "if not exists (select * from sys.tables where name = '" % RL_TB(tbName) % "' and type = 'U') "
                  "create table " % tbName % "(" % fieldsType.join(",");
    for (const auto& key : foreignKeys) {
        str = str % ", " % translateForeignKeyStatement(key);
    }
    if (primaryKeys.size() > 1) {
        str = str % ", primary key(" % primaryKeys.join(",") % ")";
    }
    str.append(")");

    BaseQuery::queryPrimitive(str);
}

void SqlServerClient::renameTable(const QString& oldName, const QString& newName) {
    BaseQuery::queryPrimitive("exec sp_rename '" % RL_TB(oldName) % "','" % RL_TB(newName) % "'");
}

void SqlServerClient::dropTable(const QString& tbName) {
    BaseQuery::queryPrimitive("drop table if exists " % tbName);
}

void SqlServerClient::truncateTable(const QString& tbName) {
    BaseQuery::queryPrimitive("truncate table " % tbName);
}

void SqlServerClient::enableForeignKey(const QString &tbName, bool enabled) {
    if (tbName.isEmpty()) {
        return;
    }
    BaseQuery::queryPrimitive(QLatin1String("ALTER TABLE %1 %2 CONSTRAINT ALL").arg(tbName, enabled ? "CHECK" : "NOCHECK"));
}

QList<QPair<QString, QString>> SqlServerClient::exportAllFields(const QString& tbName) {
    QList<QPair<QString, QString>> fields;

    auto query = BaseQuery::queryPrimitive("select COLUMN_NAME, DATA_TYPE from information_schema.COLUMNS "
                                           "where table_name = '" % RL_TB(tbName) % "'");
    while (query.next()) {
        fields << qMakePair(
                query.value(0).toString(),
                query.value(1).toString().toUpper()
                );
    }
    return fields;
}

void SqlServerClient::addField(const QString &tbName, const QString &field) {
    BaseQuery::queryPrimitive("alter table " % tbName % " add " % field);
}

void SqlServerClient::dropField(const QString &tbName, const QString &fieldName) {
    QStringList constraint;
    //search primary key / unique index constraint
    {
        QString sql = "select\n"
                      "    k.name\n"
                      "from\n"
                      "    sys.columns AS c\n"
                      "    inner join sys.tables as t on c.object_id = t.object_id\n"
                      "    inner join sys.index_columns as i on i.column_id = c.column_id and i.object_id = c.object_id\n"
                      "    inner join sys.key_constraints as k on k.unique_index_id = i.index_id and k.parent_object_id = c.object_id\n"
                      "where\n"
                      "    t.name = '" % RL_TB(tbName) % "' and c.name = '" % RL_TB(fieldName) % "' and k.type in ('PK', 'UQ')";
        auto query = BaseQuery::queryPrimitive(sql);
        while (query.next()) {
            constraint << query.value(0).toString();
        }
    }

    //search default constraint
    {
        QString sql = "select\n"
                      "    d.name\n"
                      "from\n"
                      "    sys.columns as c\n"
                      "    inner join sys.tables as t on c.object_id = t.object_id\n"
                      "    inner join sys.default_constraints as d on d.parent_column_id = c.column_id and d.parent_object_id = c.object_id\n"
                      "where\n"
                      "    t.name = '" % RL_TB(tbName) % "' and c.name = '" % RL_TB(fieldName) % "'";
        auto query = BaseQuery::queryPrimitive(sql);
        while (query.next()) {
            constraint << query.value(0).toString();
        }
    }

    for (const auto& c : constraint) {
        dropConstraint(tbName, c);
    }

    BaseQuery::queryPrimitive("alter table " % tbName % " drop column " % fieldName);
}

void SqlServerClient::dropConstraint(const QString &tbName, const QString &constraintName) {
    BaseQuery::queryPrimitive("alter table " % tbName % " drop constraint " % constraintName);
}

void SqlServerClient::renameField(const QString &tbName, const QString &oldFieldName, const QString &newFieldName) {
    BaseQuery::queryPrimitive("exec sp_rename '" % RL_TB(tbName) % "." % RL_TB(oldFieldName) % "', '"
                              % RL_TB(newFieldName) % "', 'COLUMN'");
}

QHash<IndexType, QStringList> SqlServerClient::exportAllIndexes(const QString &tbName) {
    QHash<IndexType, QStringList> indexes;
    auto query = BaseQuery::queryPrimitive("select name, type_desc, is_unique from sys.indexes where object_id = "
                                           "OBJECT_ID('" % RL_TB(tbName) % "') and is_primary_key = 0 and is_unique_constraint = 0 "
                                           "and name like 'index_%'");
    while (query.next()) {
        auto indexName = query.value(0).toString();
        if (indexName.startsWith("index_")) {
            auto desc = query.value(1).toString();
            auto unique = query.value(2).toInt();
            IndexType indexType;
            if (desc == "CLUSTERED") {
                indexType = unique == 0 ? IndexType::INDEX_CLUSTERED : IndexType::INDEX_UNIQUE_CLUSTERED;
            } else if (desc == "NONCLUSTERED") {
                indexType = unique == 0 ? IndexType::INDEX_NONCLUSTERED : IndexType::INDEX_UNIQUE_NONCLUSTERED;
            }
            indexes[indexType] << indexName;
        }
    }

    return indexes;
}

void SqlServerClient::transferDataBefore(const QString& tbName) {
    QSqlQuery query = BaseQuery::queryPrimitive("select objectproperty(object_id('" % RL_TB(tbName) %  "'),'TableHasIdentity')");
    if (query.next()) {
        if (query.value(0).toInt() == 1) {
            BaseQuery::queryPrimitive("set identity_insert " % tbName % " on");
        }
    }
}

void SqlServerClient::transferDataAfter(const QString& tbName) {
    QSqlQuery query = BaseQuery::queryPrimitive("select objectproperty(object_id('" % RL_TB(tbName) %  "'),'TableHasIdentity')");
    if (query.next()) {
        if (query.value(0).toInt() == 1) {
            BaseQuery::queryPrimitive("set identity_insert " % tbName % " off");
        }
    }
}

void SqlServerClient::createIndex(const QString &tbName,
                                  const QString &indexName,
                                  const QStringList& fields,
                                  dao::IndexType type,
                                  const QString &indexOption) {
    QString typeStr = "nonclustered";
    switch (type) {
        case IndexType::INDEX_CLUSTERED:
            typeStr = "clustered";
            break;
        case IndexType::INDEX_UNIQUE_CLUSTERED:
            typeStr = "unique clustered";
            break;
        case IndexType::INDEX_NONCLUSTERED:
            typeStr = "nonclustered";
            break;
        case IndexType::INDEX_UNIQUE_NONCLUSTERED:
            typeStr = "unique nonclustered";
            break;
        default:
            break;
    }

    QString str = "create " % typeStr % " index " % indexName % " on " % tbName % " (" % fields.join(",") % ")";
    if (!indexOption.isEmpty()) {
        str.append(" with (" % indexOption % ")");
    }

    BaseQuery::queryPrimitive(str);
}

void SqlServerClient::dropIndex(const QString &tbName, const QString& indexName) {
    BaseQuery::queryPrimitive("drop index " % indexName % " on " % tbName);
}

QString SqlServerClient::getIndexFromFields(const QStringList &fields) {
    return AbstractClient::getIndexFromFields(fields, SQLSERVER_KEYWORDS_ESCAPES);
}

void SqlServerClient::transferData(const QString &fromTb, const QString &toTb) {
    try {
        transferDataBefore(toTb);
        AbstractClient::transferData(fromTb, toTb);
        transferDataAfter(toTb);
    } catch (DaoException& e) {
        transferDataAfter(toTb);
        throw e;
    }
}

QTDAO_END_NAMESPACE