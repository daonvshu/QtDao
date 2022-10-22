#pragma once

#include "../global.h"

#include <qsharedpointer.h>
#include <qstring.h>
#include <qvariant.h>
#include <functional>

QTDAO_BEGIN_NAMESPACE

enum ConditionType {
    TypeNormal,
    TypeIn,
    TypeBetween,
    TypeIs,
    //constraint
    TypeLimit,
    TypeOrderBy,
    TypeGroupBy,
    TypeHaving,
};

struct FieldInfo {
    QString name;
    QString bindTable;

    bool operator==(const FieldInfo& other) const {
        return name == other.name && bindTable == other.bindTable;
    }
};

struct EntityConditionData : public QSharedData {
    QList<FieldInfo> fields;
    QString op;

    bool selfOperate;

    QVariantList values;
    QString combineStr;

    ConditionType conditionType;

    std::function<QString(const QString&)> fieldPrefixGetter;

    EntityConditionData() {
        fieldPrefixGetter = nullptr;
        selfOperate = false;
        conditionType = TypeNormal;
    }

    virtual ~EntityConditionData() {}

    QString getField(int index) {
        if (fieldPrefixGetter == nullptr) {
            return fields.at(index).name;
        }
        return fieldPrefixGetter(fields.at(index).bindTable) + fields.at(index).name;
    }

    virtual QList<FieldInfo> getUsedFields();
};

QTDAO_END_NAMESPACE