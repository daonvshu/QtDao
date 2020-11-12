#include "SqliteClient.h"

#include <qstandardpaths.h>
#include <qdir.h>

#include "../DbLoader.h"

#include "../query/BaseQuery.h"

void SqliteClient::testConnect() {
    auto appLocal = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir;
    if (!dir.exists(appLocal)) {
        if (!dir.mkdir(appLocal)) {
            throw DaoException("cannot create sqlite store path!");
        }
    }
}

void SqliteClient::createDatabase() {
    //auto create when connect database
}

void SqliteClient::dropDatabase() {
    auto appLocal = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    auto dbPath = appLocal + "/" + DbLoader::getConfig().dbName + ".db";
    QFile file(dbPath);
    if (file.exists()) {
        file.remove();
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

void SqliteClient::createTableIfNotExist(const QString& tbName, QStringList fieldsType) {
    QString str = "create table %1(";
    str = str.arg(tbName);
    for (const auto& ft : fieldsType) {
        str.append(ft).append(",");
    }
    str = str.left(str.length() - 1);
    str.append(")");
    
    BaseQuery::queryPrimitive(str);
}
