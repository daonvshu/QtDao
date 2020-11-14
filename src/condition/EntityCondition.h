#pragma once

#include <qobject.h>
#include <qvariant.h>

class Connector;
class EntityCondition {
public:
    EntityCondition(
        const QString& fieldName,
        const QString& op,
        const QVariantList& values,
        bool selfOperate = false
    );

private:
    QString fieldName;
    QString op;
    QVariantList values;
    bool selfOperate;
    QString combineStr;

private:
    void combine(const QString& fieldPrefix);

    friend class Connector;
};

