#pragma once

#include <qobject.h>
#include <qdir.h>

class SqliteConfig : public QObject {
    Q_OBJECT

    Q_PROPERTY(int version READ getVersion)
    Q_PROPERTY(QString type READ getSqlType)
    Q_PROPERTY(QString dbName READ getDatabaseName)
    Q_PROPERTY(QString dbPcc READ getDbPcc)
    Q_PROPERTY(QString dbOption READ getOptions)
    Q_PROPERTY(QString saveDirectory READ getSaveDirectory)

public:
    int getVersion() {
        return ver;
    }

    QString getSqlType() {
        return "sqlite";
    }

    QString getDatabaseName() {
        return "sqlitetest";
    }

    QString getDbPcc() {
        return "H*u1RhdJW&tbmHCU*FPZ#58KcL1q@ZXP";
    }

    QString getOptions() {
        return "QSQLITE_BUSY_TIMEOUT=100";
    }

    QString getSaveDirectory() {
        return QDir::currentPath();
    }

    int ver = 1;
};
