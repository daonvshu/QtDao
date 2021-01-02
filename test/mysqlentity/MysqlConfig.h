#pragma once

#include <qobject.h>

class MysqlConfig : public QObject {
    Q_OBJECT

    Q_PROPERTY(int version READ getVersion())
    Q_PROPERTY(QString type READ getSqlType())
    Q_PROPERTY(QString dbName READ getDatabaseName())
    Q_PROPERTY(QString dbHost READ getHost())
    Q_PROPERTY(QString dbUName READ getUserName())
    Q_PROPERTY(QString dbPcc READ getPassword())
    Q_PROPERTY(int dbPort READ getPort())
    Q_PROPERTY(QString dbOption READ getOption())

public:
    int getVersion() {
        return ver;
    }

    QString getSqlType() {
        return "mysql";
    }

    QString getDatabaseName() {
        return "mysqltest";
    }

    QString getHost() {
        return "localhost";
    }

    QString getUserName() {
        return "root";
    }

    QString getPassword() {
        return "root";
    }

    int getPort() {
        return 3306;
    }

    QString getOption() {
        return "MYSQL_OPT_CONNECT_TIMEOUT=3;MYSQL_OPT_READ_TIMEOUT=3;MYSQL_OPT_WRITE_TIMEOUT=3";
    }

    int ver = 1;
};