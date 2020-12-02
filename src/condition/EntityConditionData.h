#pragma once

#include <qsharedpointer.h>
#include <qstring.h>
#include <qvariant.h>
#include <functional>

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

struct FieldInfo {
    QString name;
    QString bindTable;
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

    QString getField(int index) {
        if (fieldPrefixGetter == nullptr) {
            return fields.at(index).name;
        }
        return fieldPrefixGetter(fields.at(index).bindTable) + fields.at(index).name;
    }

    QStringList getUsedFields() {
        QStringList names;
        for (const auto& field : fields) {
            names << field.name;
        }
        return names;
    }
};