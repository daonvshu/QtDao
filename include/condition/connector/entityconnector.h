#pragma once

#include "connectable.h"

#include <qobject.h>
#include <qvariant.h>

QTDAO_BEGIN_NAMESPACE

class EntityConnector : public Connectable {
public:
    virtual void addField(const FieldInfo& field);

    virtual void addValue(const QVariant& value);

    template<typename T>
    void addCustomValue(const T& value) {
        addValue(serializeCustomTypeToBinary(value));
    }

    //only used for single inheritance structure!
    EntityConnector* ptr() {
        return this;
    }
};

QTDAO_END_NAMESPACE
