#pragma once

#include <qobject.h>
#include <qstandardpaths.h>

struct DbConfig {
    /// <summary>
    /// expect database version
    /// </summary>
    int version;

    /// <summary>
    /// auto set false if need to upgrade
    /// </summary>
    bool versionValid;

    /// <summary>
    /// database type, one of sqlite,mysql,sqlserver
    /// </summary>
    QString dbType;

    /// <summary>
    /// database driver, default one of QSQLITE,QMYSQL,QODBC
    /// </summary>
    QString dbDriver;

    /// <summary>
    /// database name
    /// </summary>
    QString dbName;

    /// <summary>
    /// database address
    /// </summary>
    QString dbHost;

    /// <summary>
    /// database connect user name
    /// </summary>
    QString dbUName;

    /// <summary>
    /// database connect user password
    /// </summary>
    QString dbPcc;

    /// <summary>
    /// database connect used port
    /// </summary>
    int dbPort;

    /// <summary>
    /// database connect options
    /// </summary>
    QString dbOption;

    /// <summary>
    /// db file store directory, only support for sqlite
    /// </summary>
    QString saveDirectory;

    QString getDbStoreDirectory() const {
        auto savePath = saveDirectory;
        if (savePath.isEmpty()) {
            savePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        }
        return savePath;
    }

    QString getDbStorePath() const {
        return getDbStoreDirectory() + "/" + dbName + ".db";
    }

    /// <summary>
    /// current is sqlite
    /// </summary>
    /// <returns></returns>
    bool isSqlite() {
        return dbType == "sqlite";
    }

    /// <summary>
    /// current is mysql
    /// </summary>
    /// <returns></returns>
    bool isMysql() {
        return dbType == "mysql";
    }

    /// <summary>
    /// current is SQLServer
    /// </summary>
    /// <returns></returns>
    bool isSqlServer() {
        return dbType == "sqlserver";
    }

    bool createDbEnabled;
    bool createTableEnabled;

    DbConfig() {
        versionValid = true;

        createDbEnabled = true;
        createTableEnabled = true;
    }
};
