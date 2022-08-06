#include "dbclients/sqlserverclient.h"

#include "connectionpool.h"
#include "dbloader.h"
#include "query/basequery.h"

#include <qdir.h>
#include <QCoreApplication>
#include <qstandardpaths.h>
#include <QSqlQuery>
#include <QSqlError>

QTDAO_BEGIN_NAMESPACE

void SqlServerClient::testConnect() {
    QString lastErrStr;
    [&] {
        auto db = ConnectionPool::prepareConnect("testconnection", "master");
        if (!db.open()) {
            lastErrStr = "open master table 'master' fail! " + db.lastError().text();
            return;
        }
        QSqlQuery query("select 1", db);
        if (query.lastError().type() != QSqlError::NoError) {
            lastErrStr = "master table 'master' cannot execute query!";
            db.close();
            return;
        }
        db.close();
    } ();
    QSqlDatabase::removeDatabase("testconnection");
    if (!lastErrStr.isEmpty()) {
        throw DaoException(DbErrCode::SQLSERVER_CONNECT_FAIL, lastErrStr);
    }
}

void SqlServerClient::createDatabase() {
    QString lastErrStr;
    DbErrCode::Code code = DbErrCode::ERR_NOT_SET;
    [&] {
        auto db = ConnectionPool::prepareConnect("createconnection", "master");
        if (!db.open()) {
            lastErrStr = "create database open fail! " + db.lastError().text();
            return;
        }
        auto appName = QCoreApplication::applicationName();
        QString storePath = "C:\\ProgramData\\" + appName;
        QSqlQuery query(db);
        if (!query.exec("xp_create_subdir N'" + storePath + "'")) {
            code = DbErrCode::SQLSERVER_CREATE_DB_PATH_FAIL;
            lastErrStr = "cannot create sqlserver store path! path = " + storePath + " err = " + query.lastError().text();
            db.close();
            return;
        }

        QString sql = QByteArray::fromBase64("SUYgREJfSUQoTiclMScpIElTIE5VTEwKCUNSRUFURSBEQVRBQkFTRSBbJTFdCglPTgoJUFJJTUFSWQoJKAoJCU5BTU\
UgPSBOJyUxJywKCQlGSUxFTkFNRSA9IE4nJTJcJTEubmRmJywKCQlTSVpFID0gOE1CLAoJCU1BWFNJWkUgPSBVTkxJTUlURUQsCgkJRklM\
RUdST1dUSCA9IDY0TUIKCSkKCUxPRyBPTgoJKAoJCU5BTUUgPSBOJyUxX2xvZycsCgkJRklMRU5BTUUgPSBOJyUyXCUxX2xvZy5sZGYnLA\
oJCVNJWkUgPSA4TUIsCgkJTUFYU0laRSA9IFVOTElNSVRFRCwKCQlGSUxFR1JPV1RIID0gNjRNQgoJKQ==");
        sql = sql.arg(DbLoader::getConfig().dbName, storePath);
        if (!query.exec(sql)) {
            code = DbErrCode::SQLSERVER_CREATE_DATABASE_FAIL;
            lastErrStr = "create database fail! err = " + query.lastError().text();
            db.close();
            return;
        }
        db.close();
    } ();
    QSqlDatabase::removeDatabase("createconnection");
    if (!lastErrStr.isEmpty()) {
        throw DaoException(code, lastErrStr);
    }
}

void SqlServerClient::dropDatabase() {
    QString lastErrStr;
    [&] {
        auto db = ConnectionPool::prepareConnect("dropconnection", "master");
        if (!db.open()) {
            lastErrStr = "drop database open fail! " + db.lastError().text();
            return;
        }
        QString sql = "if exists (select* from sysdatabases where name ='%1') drop database %1";
        sql = sql.arg(DbLoader::getConfig().dbName);
        QSqlQuery query(db);
        if (!query.exec(sql)) {
            lastErrStr = "drop database fail! err = " + query.lastError().text();
            db.close();
            return;
        }
        db.close();
    } ();
    QSqlDatabase::removeDatabase("dropconnection");
    if (!lastErrStr.isEmpty()) {
        throw DaoException(DbErrCode::SQL_EXEC_FAIL, lastErrStr);
    }
}

bool SqlServerClient::checkTableExist(const QString& tbName) {
    auto str = QString("select * from sys.tables where name = '%1' and type = 'U'")
        .arg(tbName);

    bool exist = false;
    BaseQuery::queryPrimitive(str, [&](QSqlQuery& query) {
        if (query.next()) {
            exist = true;
        }
    });
    return exist;
}

void SqlServerClient::createTableIfNotExist(const QString& tbName, QStringList fieldsType, QStringList primaryKeys) {
    QString str = "if not exists (select * from sys.tables where name = '%1' and type = 'U') create table %1(";
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

    BaseQuery::setErrIfQueryFail(DbErrCode::SQLSERVER_CREATE_TABLE_FAIL);
    BaseQuery::queryPrimitiveThrowable(str);
}

void SqlServerClient::createIndex(const QString& tbName, QStringList fields, IndexType type, const std::function<QString(const QString&)>& optionGet) {
    QString str = "create %1 index %2 on %3 (";
    QString indexName = "index";
    for (const auto& field : fields) {
        indexName.append("_").append(field.split(" ").at(0));
        str.append(field).append(",");
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
    str = str.chopped(1).arg(typeStr).arg(indexName).arg(tbName);
    str.append(")");
    auto option = optionGet(indexName);
    if (!option.isEmpty()) {
        str.append(" with (").append(option).append(")");
    }

    BaseQuery::setErrIfQueryFail(DbErrCode::SQLSERVER_CREATE_INDEX_FAIL);
    BaseQuery::queryPrimitiveThrowable(str);
}

void SqlServerClient::renameTable(const QString& oldName, const QString& newName) {
    auto str = QString("exec sp_rename '%1','%2'").arg(oldName, newName);

    BaseQuery::setErrIfQueryFail(DbErrCode::SQLSERVER_CREATE_TMP_TABLE_FAIL);
    BaseQuery::queryPrimitiveThrowable(str);
}

void SqlServerClient::dropTable(const QString& tbName) {
    auto str = QString("drop table if exists %1").arg(tbName);
    BaseQuery::queryPrimitive(str);
}

void SqlServerClient::truncateTable(const QString& tbName) {
    auto str = QString("truncate table %1").arg(tbName);
    BaseQuery::queryPrimitive(str);
}

QStringList SqlServerClient::getTagTableFields(const QString& tbName) {
    auto str = QString("select COLUMN_NAME from information_schema.COLUMNS where table_name = '%1'")
        .arg(tbName);

    QStringList fields;

    BaseQuery::setErrIfQueryFail(DbErrCode::SQLSERVER_DUMP_FIELD_FAIL);
    auto query = BaseQuery::queryPrimitiveThrowable(str);
    while (query.next()) {
        fields << query.value(0).toString();
    }
    return fields;
}

void SqlServerClient::restoreDataBefore(const QString& tbName) {
    QSqlQuery query = BaseQuery::queryPrimitiveThrowable(QString("select objectproperty(object_id('%1'),'TableHasIdentity')").arg(tbName));
    if (query.next()) {
        if (query.value(0).toInt() == 1) {
            BaseQuery::queryPrimitiveThrowable(QString("set identity_insert %1 on").arg(tbName));
        }
    }
}

void SqlServerClient::restoreDataAfter(const QString& tbName) {
    QSqlQuery query = BaseQuery::queryPrimitiveThrowable(QString("select objectproperty(object_id('%1'),'TableHasIdentity')").arg(tbName));
    if (query.next()) {
        if (query.value(0).toInt() == 1) {
            BaseQuery::queryPrimitiveThrowable(QString("set identity_insert %1 off").arg(tbName));
        }
    }
}

void SqlServerClient::dropAllIndexOnTable(const QString& tbName) {
    BaseQuery::setErrIfQueryFail(DbErrCode::SQLSERVER_DROP_OLD_INDEX_FAIL);
    auto query = BaseQuery::queryPrimitiveThrowable(
        QString("select a.name from sys.indexes a join sys.tables c ON (a.object_id = c.object_id) where c.name='%1' and a.name like 'index_%' group by a.name")
        .arg(tbName)
    );
    QStringList indexNames;
    while (query.next()) {
        indexNames << query.value(0).toString();
    }
    for (const auto& name : indexNames) {
        BaseQuery::queryPrimitiveThrowable(
            QString("drop index %1 on %2").arg(name, tbName)
        );
    }
}

QTDAO_END_NAMESPACE