#pragma once

#include <qsharedpointer.h>
#include <qstring.h>
#include <qvariant.h>

enum ConditionType {
    TypeNormal,
    TypeIn,
    TypeBetween,
    //constraint
    TypeLimit,
    TypeOrderBy,
    TypeGroupBy,
    TypeHaving,
};

class EntityConditionData : public QSharedData {
public:
    QString fieldName;
    QString op;
    QVariantList values;
    bool selfOperate;
    QString combineStr;

    ConditionType conditionType;
};