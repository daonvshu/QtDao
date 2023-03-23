#pragma once

#include "../../global.h"

#include <qobject.h>
#include <qvariant.h>
#include <functional>

QTDAO_BEGIN_NAMESPACE

struct FieldInfo {
    QString name;
    QString bindTable;

    bool operator==(const FieldInfo& other) const {
        return name == other.name && bindTable == other.bindTable;
    }
};

typedef std::function<QString(const QString& tbName)> FieldPrefixGetter;

template<typename T>
class EntityField;

class Connectable {
public:
    void setFieldPrefixGetter(const FieldPrefixGetter& getter) {
        this->fieldPrefixGetter = getter;
    }

    virtual void combine() = 0;

    virtual QList<FieldInfo> getUsedFields();

    virtual QString getConditionSegment();

    virtual QVariantList getValueList();

    virtual bool isEmpty();

    virtual ~Connectable() = default;

protected:
    QString getField(int index) const;

protected:
    FieldPrefixGetter fieldPrefixGetter;
    QList<FieldInfo> fields;
    QVariantList values;
    QString connectedStr;

    template<typename T>
    FieldInfo getEntityFieldInfo(const EntityField<T>& entityField);
};

QTDAO_END_NAMESPACE