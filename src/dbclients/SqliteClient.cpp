#include "SqliteClient.h"

#include <qstandardpaths.h>
#include <qdir.h>

#include "../DbLoader.h"

void SqliteClient::testConnect() {
    auto appLocal = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir;
    if (!dir.exists(appLocal)) {
        if (!dir.mkdir(appLocal)) {
            throw "cannot create sqlite store path!";
        }
    }
}

void SqliteClient::createDatabase() {
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
