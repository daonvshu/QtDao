#include "dbclients/MysqlClient.h"

#include "DbLoader.h"

#include "ConnectionPool.h"

#include "query/BaseQuery.h"

#include <QSqlQuery>
#include <QSqlError>

void MysqlClient::testConnect() {
    QString lastErrStr;
    [&] {
        auto db = ConnectionPool::prepareConnect("testconnection", "mysql");
        if (!db.open()) {
            lastErrStr = "open master table 'mysql' fail! " + db.lastError().text();
            return;
        }
        QSqlQuery query("select 1", db);
        if (query.lastError().type() != QSqlError::NoError) {
            lastErrStr = "master table 'mysql' cannot execute query!";
            db.close();
            return;
        }
        db.close();
    } ();
    QSqlDatabase::removeDatabase("testconnection");
    if (!lastErrStr.isEmpty()) {
        throw DaoException(DbErrCode::MYSQL_CONNECT_FAIL, lastErrStr);
    }
}

void MysqlClient::createDatabase() {
    QString lastErrStr;
    [&] {
        auto db = ConnectionPool::prepareConnect("createconnection", "mysql");
        if (!db.open()) {
            lastErrStr = "create database open fail! " + db.lastError().text();
            return;
        }
        QString sql = "create database if not exists %1 default character set utf8 COLLATE utf8_general_ci";
        QSqlQuery query(db);
        if (!query.exec(sql.arg(DbLoader::getConfig().dbName))) {
            lastErrStr = "create database fail! err = " + db.lastError().text();
            db.close();
            return;
        }
        db.close();
    } ();
    QSqlDatabase::removeDatabase("createconnection");
    if (!lastErrStr.isEmpty()) {
        throw DaoException(DbErrCode::MYSQL_CREATE_DATABASE_FAIL, lastErrStr);
    }
}

void MysqlClient::dropDatabase() {
    QString lastErrStr;
    [&] {
        auto db = ConnectionPool::prepareConnect("dropconnection", "mysql");
        if (!db.open()) {
            lastErrStr = "drop database open fail! " + db.lastError().text();
            return;
        }
        QString sql = "drop database if exists %1";
        QSqlQuery query(db);
        if (!query.exec(sql.arg(DbLoader::getConfig().dbName))) {
            lastErrStr = "drop database fail! err = " + db.lastError().text();
            db.close();
            return;
        }
        db.close();
    } ();
    QSqlDatabase::removeDatabase("dropconnection");
    QSqlDatabase::removeDatabase("createconnection");
    if (!lastErrStr.isEmpty()) {
        throw DaoException(DbErrCode::SQL_EXEC_FAIL, lastErrStr);
    }
}

bool MysqlClient::checkTableExist(const QString& tbName) {
    auto str = QString("select table_name from information_schema.TABLES where table_name ='%1' and table_schema = '%2'")
        .arg(tbName, DbLoader::getConfig().dbName);

    bool exist = false;
    BaseQuery::queryPrimitive(str, [&](QSqlQuery& query) {
        if (query.next()) {
            exist = true;
        }
    });
    return exist;
}

void MysqlClient::createTableIfNotExist(const QString& tbName, const QString& engine, QStringList fieldsType, QStringList primaryKeys) {
    QString str = "create table if not exists %1(";
    str = str.arg(tbName);
    for (const auto& ft : fieldsType) {
        str.append(ft).append(",");
    }
    if (primaryKeys.size() <= 1) {
        str.chop(1);
    } else {
        str.append("primary key(");
        for (const auto& key : primaryKeys) {
            str.append(key).append(",");
        }
        str.chop(1);
        str.append(")");
    }
    str.append(")");
    if (!engine.isEmpty()) {
        str.append(" engine=").append(engine);
    }
    str.append(" default charset = utf8");

    BaseQuery::setErrIfQueryFail(DbErrCode::MYSQL_CREATE_TABLE_FAIL);
    BaseQuery::queryPrimitiveThrowable(str);
}

void MysqlClient::createIndex(const QString& tbName, QStringList fields, IndexType type) {
    QString str = "create %1 index %2 on %3 (";
    QString indexName = "index";
    for (const auto& field : fields) {
        indexName.append("_").append(field.split(" ").at(0));
        str.append(field).append(",");
    }
    QString typeStr = "";
    switch (type) {
    case AbstractClient::INDEX_UNIQUE:
        typeStr = "unique";
        break;
    default:
        break;
    }
    str = str.chopped(1).arg(typeStr).arg(indexName).arg(tbName);
    str.append(")");

    BaseQuery::setErrIfQueryFail(DbErrCode::MYSQL_CREATE_INDEX_FAIL);
    BaseQuery::queryPrimitiveThrowable(str);
}

void MysqlClient::renameTable(const QString& oldName, const QString& newName) {
    auto str = QString("rename table %1 to %2").arg(oldName, newName);

    BaseQuery::setErrIfQueryFail(DbErrCode::MYSQL_CREATE_TMP_TABLE_FAIL);
    BaseQuery::queryPrimitiveThrowable(str);
}

void MysqlClient::dropTable(const QString& tbName) {
    auto str = QString("drop table if exists %1").arg(tbName);
    BaseQuery::queryPrimitive(str);
}

void MysqlClient::truncateTable(const QString& tbName) {
    auto str = QString("truncate table %1").arg(tbName);
    BaseQuery::queryPrimitive(str);
}

QStringList MysqlClient::getTagTableFields(const QString& tbName) {
    auto str = 
        QString("select COLUMN_NAME from information_schema.COLUMNS where table_name = '%1' and table_schema = '%2';")
    .arg(tbName, DbLoader::getConfig().dbName);

    QStringList fields;

    BaseQuery::setErrIfQueryFail(DbErrCode::MYSQL_DUMP_FIELD_FAIL);
    auto query = BaseQuery::queryPrimitiveThrowable(str);
    while (query.next()) {
        fields << query.value(0).toString();
    }
    return fields;
}

void MysqlClient::dropAllIndexOnTable(const QString& tbName) {
    BaseQuery::setErrIfQueryFail(DbErrCode::MYSQL_DROP_OLD_INDEX_FAIL);
    auto query = BaseQuery::queryPrimitiveThrowable(
        QString("SELECT index_name FROM information_schema.statistics where TABLE_SCHEMA = '%1' and TABLE_NAME = '%2' GROUP BY index_name")
        .arg(DbLoader::getConfig().dbName, tbName)
    );
    QStringList indexNames;
    while (query.next()) {
        QString indexName = query.value(0).toString();
        if (!indexName.startsWith("index_"))
            continue;
        indexNames << indexName;
    }
    for (const auto& name : indexNames) {
        BaseQuery::queryPrimitiveThrowable(
            QString("drop index %1 on %2").arg(name, tbName)
        );
    }
}
