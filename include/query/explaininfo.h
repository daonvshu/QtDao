#pragma once

#include "../global.h"

#include <qobject.h>

QTDAO_BEGIN_NAMESPACE

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
    qint64 Rows;
    qint64 Executes;
    QString StmtText;
    int StmtId;
    int NodeId;
    int Parent;
    QString PhysicalOp;
    QString LogicalOp;
    QString Argument;
    QString DefinedValues;
    qreal EstimateRows;
    qreal EstimateIO;
    qreal EstimateCPU;
    int AvgRowSize;
    qreal TotalSubtreeCost;
    QString OutputList;
    QString Warnings;
    QString Type;
    uint Parallel;
    qreal EstimateExecutions;
};

struct PSqlExplainInfo {
    QString nodeType;
    bool parallelAware;
    bool asyncCapable;
    QString relationName;
    QString alias;
    double startupCost;
    double totalCost;
    double planRows;
    double planWidth;
};

QTDAO_END_NAMESPACE