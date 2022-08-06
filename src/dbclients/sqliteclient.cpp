#include "dbclients/sqliteclient.h"

#include <qstandardpaths.h>
#include <qdir.h>
#include <qfileinfo.h>

#include "dbloader.h"

#include "query/basequery.h"

QTDAO_BEGIN_NAMESPACE

bool createPath(QString path) {
    QDir dir;
    QFileInfo info(path);
    if (!info.exists(path)) {
        if (!dir.mkdir(path)) {
            int index = path.lastIndexOf('/');
            auto target = path.mid(0, index);
            if (!createPath(target)) {
                return false;
            }
            return dir.mkdir(path);
        }
    }
    return true;
}

void SqliteClient::testConnect() {
    auto appLocal = DbLoader::getConfig().getDbStoreDirectory();
    QDir dir;
    if (!dir.exists(appLocal)) {
        if (!createPath(appLocal)) { // create directories recursively
            throw DaoException(DbErrCode::SQLITE_CREATE_DB_PATH_FAIL, "cannot create sqlite store path! applocal = " + appLocal);
        }
    }
}

void SqliteClient::createDatabase() {
    //auto create when connect database
}

void SqliteClient::dropDatabase() {
    auto appLocal = DbLoader::getConfig().getDbStoreDirectory();
    QFile file(DbLoader::getConfig().getDbStorePath());
    if (file.exists()) {
        if (!file.remove()) {
            throw DaoException(DbErrCode::SQL_EXEC_FAIL, "unable remove database file!");
        }
    }
    QDir dir(appLocal);
    if (dir.exists()) {
        dir.rmdir(appLocal);
    }
}

bool SqliteClient::checkTableExist(const QString& tbName) {
    auto str = QString("select *from sqlite_master where type='table' and name = '%1'").arg(tbName);

    bool exist = false;
    BaseQuery::queryPrimitive(str, [&](QSqlQuery& query) {
        if (query.next()) {
            exist = true;
        }
    });
    return exist;
}

void SqliteClient::createTableIfNotExist(const QString& tbName, QStringList fieldsType, QStringList primaryKeys) {
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

    BaseQuery::setErrIfQueryFail(DbErrCode::SQLITE_CREATE_TABLE_FAIL);
    BaseQuery::queryPrimitiveThrowable(str);
}

void SqliteClient::createIndex(const QString& tbName, QStringList fields, IndexType type) {
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

    BaseQuery::setErrIfQueryFail(DbErrCode::SQLITE_CREATE_INDEX_FAIL);
    BaseQuery::queryPrimitiveThrowable(str);
}

void SqliteClient::renameTable(const QString& oldName, const QString& newName) {
    auto str = QString("alter table %1 rename to %2").arg(oldName, newName);

    BaseQuery::setErrIfQueryFail(DbErrCode::SQLITE_CREATE_TMP_TABLE_FAIL);
    BaseQuery::queryPrimitiveThrowable(str);
}

void SqliteClient::dropTable(const QString& tbName) {
    auto str = QString("drop table if exists %1").arg(tbName);
    BaseQuery::queryPrimitive(str);
}

void SqliteClient::truncateTable(const QString& tbName) {
    auto str = QString("delete from %1").arg(tbName);
    BaseQuery::queryPrimitive(str);

    if (checkTableExist("sqlite_sequence")) {
        str = QString("delete from sqlite_sequence where name = \"%1\"").arg(tbName);
        BaseQuery::queryPrimitive(str);
    }
}

QStringList SqliteClient::getTagTableFields(const QString& tbName) {
    QStringList fields;

    BaseQuery::setErrIfQueryFail(DbErrCode::SQLITE_DUMP_FIELD_FAIL);
    auto query = BaseQuery::queryPrimitiveThrowable(QString("pragma table_info('%1')").arg(tbName));
    while (query.next()) {
        fields << query.value(1).toString();
    }
    return fields;
}

void SqliteClient::dropAllIndexOnTable(const QString& tbName) {
    auto query = BaseQuery::queryPrimitiveThrowable(
        QString("select *from sqlite_master where type='index' and tbl_name = '%1'").arg(tbName)
    );
    QStringList indexNames;
    while (query.next()) {
        QString indexName = query.value(1).toString();
        if (!indexName.startsWith("index_"))
            continue;
        indexNames << indexName;
    }

    BaseQuery::setErrIfQueryFail(DbErrCode::SQLITE_DROP_OLD_INDEX_FAIL);
    for (const auto& name : indexNames) {
        BaseQuery::queryPrimitiveThrowable(
            QString("drop index %1").arg(name)
        );
    }
}

QTDAO_END_NAMESPACE