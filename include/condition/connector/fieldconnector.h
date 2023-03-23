#pragma once

#include "connectable.h"

#include "../entityfield.h"

QTDAO_BEGIN_NAMESPACE

class FieldConnector : public Connectable {
public:
    template<typename E>
    void setField(const EntityField<E>& field) {
        fields << getEntityFieldInfo(field);
    }

    void setField(const FieldInfo& fieldInfo) {
        fields << fieldInfo;
    }

    void combine() override {
        connectedStr = getField(0);
    }
};

QTDAO_END_NAMESPACE