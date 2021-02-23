#pragma once

#include <qobject.h>

struct SqliteExplainInfo {
    qint64 addr;
    QString opcode;
    qint64 p1;
    qint64 p2;
    qint64 p3;
    QString p4;
    QString p5;
    QString comment;
};

struct SqliteExplainQueryPlanInfo {
    QList<QPair<QString, QVariant>> data;
};

struct MysqlExplainInfo {
    quint64 id;
    QString selectType;
    QString table;
    QString partitions;
    QString type;
    QString possibleKeys;
    QString key;
    QString keyLen;
    QString ref;
    quint64 rows;
    qreal filtered;
    QString extra;
};

struct SqlServerExplainInfo {
    
};