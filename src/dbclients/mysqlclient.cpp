#include "dbclients/mysqlclient.h"

#include "connectionpool.h"
#include "dbexception.h"
#include "query/basequery.h"
#include "config/configbuilder.h"

#include <QSqlError>
#include <QSqlQuery>

QTDAO_BEGIN_NAMESPACE

void MysqlClient::testConnect() {
    QSqlError lastErr;
    [&] {
        auto db = ConnectionPool::prepareConnect("testconnection", "mysql");
        if (!db.open()) {
            lastErr = db.lastError();
            return;
        }
        QSqlQuery query("select 1", db);
        if (query.lastError().type() != QSqlError::NoError) {
            lastErr = query.lastError();
            db.close();
            return;
        }
        db.close();
    } ();
    QSqlDatabase::removeDatabase("testconnection");
    if (lastErr.type() != QSqlError::NoError) {
        throw DaoException(lastErr);
    }
}

void MysqlClient::createDatabase() {
    QSqlError lastErr;
    [&] {
        auto db = ConnectionPool::prepareConnect("createconnection", "mysql");
        if (!db.open()) {
            lastErr = db.lastError();
            return;
        }
        QString sql = "create database if not exists %1 default character set utf8mb4 COLLATE utf8mb4_general_ci";
        QSqlQuery query(db);
        if (!query.exec(sql.arg(globalConfig->mDatabaseName))) {
            lastErr = db.lastError();
            db.close();
            return;
        }
        db.close();
    } ();
    QSqlDatabase::removeDatabase("createconnection");
    if (lastErr.type() != QSqlError::NoError) {
        throw DaoException(lastErr);
    }
}

void MysqlClient::dropDatabase() {
    QSqlError lastErr;
    [&] {
        auto db = ConnectionPool::prepareConnect("dropconnection", "mysql");
        if (!db.open()) {
            lastErr = db.lastError();
            return;
        }
        QString sql = "drop database if exists %1";
        sql = sql.arg(globalConfig->mDatabaseName);
        QSqlQuery query(db);
        if (!query.exec(sql)) {
            lastErr = db.lastError();
            db.close();
            return;
        }
        db.close();
    } ();
    QSqlDatabase::removeDatabase("dropconnection");
    if (lastErr.type() != QSqlError::NoError) {
        throw DaoException(lastErr);
    }
}

bool MysqlClient::checkTableExist(const QString& tbName) {
    auto str = QString("select table_name from information_schema.TABLES where table_name ='%1' and table_schema = '%2'")
        .arg(tbName, globalConfig->mDatabaseName);

    auto query = BaseQuery::queryPrimitive(str);
    return query.next();
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
    str.append(" default charset = utf8mb4");

    BaseQuery::queryPrimitive(str);
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
    str = str.chopped(1).arg(typeStr, indexName, tbName);
    str.append(")");

    BaseQuery::queryPrimitive(str);
}

void MysqlClient::renameTable(const QString& oldName, const QString& newName) {
    auto str = QString("rename table %1 to %2").arg(oldName, newName);

    BaseQuery::queryPrimitive(str);
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
    .arg(tbName, globalConfig->mDatabaseName);

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
        .arg(globalConfig->mDatabaseName, tbName)
    );
    QStringList indexNames;
    while (query.next()) {
        QString indexName = query.value(0).toString();
        if (!indexName.startsWith("index_"))
            continue;
        indexNames << indexName;
    }
    for (const auto& name : indexNames) {
        BaseQuery::queryPrimitive(
            QString("drop index %1 on %2").arg(name, tbName)
        );
    }
}

QTDAO_END_NAMESPACE