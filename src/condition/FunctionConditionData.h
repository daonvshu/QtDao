#pragma once

#include <qsharedpointer.h>
#include <qstring.h>
#include <qvariant.h>

#include "EntityConditionData.h"

struct FunctionConditionData : public EntityConditionData {
    QString expressions;

    QList<FieldInfo> getUsedFields() override {
        int asIndex = expressions.indexOf(" as ");
        if (asIndex == -1) {
            if (fields.isEmpty()) {
                return QList<FieldInfo>() << FieldInfo{ expressions.mid(asIndex), "" }; //empty tablename
            } else {
                return QList<FieldInfo>() << FieldInfo{ expressions.mid(asIndex), fields.at(0).bindTable }; //use first bind table
            }
        }
        if (fields.isEmpty()) {
            return QList<FieldInfo>() << FieldInfo{ expressions, "" }; //empty tablename
        }
        QString str = expressions;
        for (int i = 0; i < fields.size(); i++) {
            str = str.arg(fields.at(i).name);
        }
        return QList<FieldInfo>() << FieldInfo{ str, fields.at(0).bindTable }; //use first bind table
    }
};