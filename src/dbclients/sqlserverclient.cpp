#include "dbclients/sqlserverclient.h"

#include "connectionpool.h"
#include "dbexception.h"
#include "query/basequery.h"
#include "config/configbuilder.h"

#include <qcoreapplication.h>
#include <qdir.h>
#include <qstandardpaths.h>
#include <qstringbuilder.h>

QTDAO_BEGIN_NAMESPACE

#define SQLSERVER_KEYWORDS_ESCAPES {"[", "]"}

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
        throw DaoException("cannot create sqlserver store path! path = " % storePath % " , cannot read master table physical path!");
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

bool SqlServerClient::checkTableExist(const QString& tbName) {
    auto str = QString("select * from sys.tables where name = '%1' and type = 'U'")
        .arg(checkAndRemoveKeywordEscapes(tbName, SQLSERVER_KEYWORDS_ESCAPES));

    auto query = BaseQuery::queryPrimitive(str);
    return query.next();
}

void SqlServerClient::renameTable(const QString& oldName, const QString& newName) {
    BaseQuery::queryPrimitive("exec sp_rename '" % oldName % "','" % newName % "'");
}

void SqlServerClient::dropTable(const QString& tbName) {
    BaseQuery::queryPrimitive("drop table if exists " % tbName);
}

void SqlServerClient::truncateTable(const QString& tbName) {
    BaseQuery::queryPrimitive("truncate table " % tbName);
}

QStringList SqlServerClient::getTagTableFields(const QString& tbName) {
    auto str = QString("select COLUMN_NAME from information_schema.COLUMNS where table_name = '%1'")
        .arg(checkAndRemoveKeywordEscapes(tbName, SQLSERVER_KEYWORDS_ESCAPES));

    QStringList fields;

    auto query = BaseQuery::queryPrimitive(str);
    while (query.next()) {
        fields << query.value(0).toString();
    }
    return fields;
}

void SqlServerClient::restoreDataBefore(const QString& tbName) {
    QSqlQuery query = BaseQuery::queryPrimitive(QString("select objectproperty(object_id('%1'),'TableHasIdentity')")
        .arg(checkAndRemoveKeywordEscapes(tbName, SQLSERVER_KEYWORDS_ESCAPES)));
    if (query.next()) {
        if (query.value(0).toInt() == 1) {
            BaseQuery::queryPrimitive("set identity_insert " % tbName % " on");
        }
    }
}

void SqlServerClient::restoreDataAfter(const QString& tbName) {
    QSqlQuery query = BaseQuery::queryPrimitive(QString("select objectproperty(object_id('%1'),'TableHasIdentity')")
        .arg(checkAndRemoveKeywordEscapes(tbName, SQLSERVER_KEYWORDS_ESCAPES)));
    if (query.next()) {
        if (query.value(0).toInt() == 1) {
            BaseQuery::queryPrimitive("set identity_insert " % tbName % " off");
        }
    }
}

void SqlServerClient::dropAllIndexOnTable(const QString& tbName) {
    auto query = BaseQuery::queryPrimitive(
        QString("select a.name from sys.indexes a join sys.tables c ON (a.object_id = c.object_id) where c.name='%1' and a.name like 'index_%' group by a.name")
            .arg(checkAndRemoveKeywordEscapes(tbName, SQLSERVER_KEYWORDS_ESCAPES))
    );
    QStringList indexNames;
    while (query.next()) {
        indexNames << query.value(0).toString();
    }
    for (const auto& name : indexNames) {
        BaseQuery::queryPrimitive("drop index " % name % " on " % tbName);
    }
}

void SqlServerClient::createTable(dao::EntityReaderInterface *reader) {
    createTableIfNotExist(reader->getTableName(), reader->getFieldsType(), reader->getPrimaryKeys());

    auto optionGetter = [=](const QString& indexName) {
        return reader->getIndexOption(indexName);
    };
    //create clustered index
    auto clusteredIndexFields = reader->getClusteredIndexFields();
    for (const auto& i : clusteredIndexFields) {
        createIndex(reader->getTableName(), i, INDEX_CLUSTERED, optionGetter);
    }
    //create unique clustered index
    auto uniqueClusteredIndexFields = reader->getUniqueClusteredIndexFields();
    for (const auto& i : uniqueClusteredIndexFields) {
        createIndex(reader->getTableName(), i, INDEX_UNIQUE_CLUSTERED, optionGetter);
    }
    //create non-clustered index
    auto nonclusteredIndexFields = reader->getNonClusteredIndexFields();
    for (const auto& i : nonclusteredIndexFields) {
        createIndex(reader->getTableName(), i, INDEX_NONCLUSTERED, optionGetter);
    }
    //create unique non-clustered index
    auto uniqueNonclusteredIndexFields = reader->getUniqueNonClusteredIndexFields();
    for (const auto& i : uniqueNonclusteredIndexFields) {
        createIndex(reader->getTableName(), i, INDEX_UNIQUE_NONCLUSTERED, optionGetter);
    }
}

void SqlServerClient::createTableIfNotExist(const QString& tbName, const QStringList& fieldsType, const QStringList& primaryKeys) {
    QString str = "if not exists (select * from sys.tables where name = '%1' and type = 'U') create table %2(";
    str = str.arg(checkAndRemoveKeywordEscapes(tbName, SQLSERVER_KEYWORDS_ESCAPES), tbName);
    str.append(fieldsType.join(","));
    if (primaryKeys.size() > 1) {
        str = str % ", primary key(" % primaryKeys.join(",") % ")";
    }
    str.append(")");

    BaseQuery::queryPrimitive(str);
}

void SqlServerClient::createIndex(const QString& tbName, const QStringList& fields, IndexType type, const std::function<QString(const QString&)>& optionGet) {
    QString indexName = "index";
    for (const auto& field : fields) {
        indexName.append("_").append(checkAndRemoveKeywordEscapes(field.split(" ").at(0), SQLSERVER_KEYWORDS_ESCAPES));
    }
    QString typeStr = "nonclustered";
    switch (type) {
        case AbstractClient::INDEX_CLUSTERED:
            typeStr = "clustered";
            break;
        case AbstractClient::INDEX_UNIQUE_CLUSTERED:
            typeStr = "unique clustered";
            break;
        case AbstractClient::INDEX_NONCLUSTERED:
            typeStr = "nonclustered";
            break;
        case AbstractClient::INDEX_UNIQUE_NONCLUSTERED:
            typeStr = "unique nonclustered";
            break;
        default:
            break;
    }

    QString str = "create " % typeStr % " index " % indexName % " on " % tbName % " (" % fields.join(",") % ")";
    auto option = optionGet(indexName);
    if (!option.isEmpty()) {
        str.append(" with (" % option % ")");
    }

    BaseQuery::queryPrimitive(str);
}

QTDAO_END_NAMESPACE