#pragma once

#include <qsharedpointer.h>
#include <qstring.h>
#include <qvariant.h>

struct FunctionConditionData : public QSharedData {
    QString expressions;
    QStringList fieldNames;
    QVariantList values;

    QString combineStr;
};