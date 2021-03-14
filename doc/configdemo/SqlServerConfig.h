#pragma once

#include <qobject.h>

class SqlServerConfig : public QObject {
    Q_OBJECT

    Q_PROPERTY(int version READ getVersion)
    Q_PROPERTY(QString type READ getSqlType)
    Q_PROPERTY(QString dbName READ getDatabaseName)
    Q_PROPERTY(QString dbHost READ getHost)
    Q_PROPERTY(QString dbUName READ getUserName)
    Q_PROPERTY(QString dbPcc READ getPassword)

public:
    int getVersion() {
        return ver;
    }

    QString getSqlType() {
        return "sqlserver";
    }

    QString getDatabaseName() {
        return "sqlservertest";
    }

    QString getHost() {
        return "192.168.1.19\\sqlexpress";
    }

    QString getUserName() {
        return "root";
    }

    QString getPassword() {
        return "root";
    }

    int ver = 1;
};