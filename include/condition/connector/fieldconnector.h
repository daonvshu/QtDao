#pragma once

#include "connectable.h"

#include "../entityfield.h"

QTDAO_BEGIN_NAMESPACE

class FieldConnector : public Connectable {
public:
    template<typename E>
    void setField(const EntityField<E>& field) {
        d->fields << getEntityFieldInfo(field);
    }

    void setField(const FieldInfo& fieldInfo) {
        d->fields << fieldInfo;
    }

    void combine() override {
        d->connectedStr = getField(0);
    }
};

QTDAO_END_NAMESPACE