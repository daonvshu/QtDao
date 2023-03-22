#pragma once

#include "../../global.h"

#include <qobject.h>
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

    virtual QList<FieldInfo> getUsedFields() = 0;

    virtual QString getConditionSegment() = 0;

    virtual QVariantList  getValueList() = 0;

    virtual ~Connectable() = default;

protected:
    FieldPrefixGetter fieldPrefixGetter;

    template<typename T>
    FieldInfo getEntityFieldInfo(const EntityField<T>& entityField);
};

QTDAO_END_NAMESPACE