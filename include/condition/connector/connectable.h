#pragma once

#include "../../global.h"

#include <qobject.h>
#include <qvariant.h>
#include <functional>
#include <qshareddata.h>

QTDAO_BEGIN_NAMESPACE

struct FieldInfo {
    QString name;
    QString bindTable;

    bool operator==(const FieldInfo& other) const {
        return name == other.name && bindTable == other.bindTable;
    }
};

#if QT_VERSION_MAJOR >= 6
inline size_t qHash(const FieldInfo& type, size_t seed) {
    return qHashMulti(seed, type.name, type.bindTable);
}
#else
inline uint qHash(const FieldInfo& type, uint seed) {
    return ::qHash(type.name, seed) ^ ::qHash(type.bindTable, seed);
}
#endif

typedef std::function<QString(const QString& tbName)> FieldPrefixGetter;

template<typename T>
class EntityField;

struct ConnectableData : QSharedData {
    FieldPrefixGetter fieldPrefixGetter;
    QList<FieldInfo> fields;
    QVariantList values;
    QString connectedStr;

    void clear() {
        fields.clear();
        values.clear();
        connectedStr = QString();
    }
};

class Connectable {
public:
    Connectable() {
        d = new ConnectableData;
    }

    void setFieldPrefixGetter(const FieldPrefixGetter& getter) {
        d->fieldPrefixGetter = getter;
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
    QExplicitlySharedDataPointer<ConnectableData> d;

    template<typename T>
    FieldInfo getEntityFieldInfo(const EntityField<T>& entityField);
};

QTDAO_END_NAMESPACE