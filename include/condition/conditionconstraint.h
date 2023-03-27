#pragma once

#include "../global.h"

#include "connector/constraintconnector.h"
#include "functioncondition.h"

QTDAO_BEGIN_NAMESPACE

template<typename T>
class EntityField;

class LimitConstraintConnector : public ConstraintConnector {
public:
    explicit LimitConstraintConnector(int rows);

    explicit LimitConstraintConnector(int offset, int rows);

    QVariantList getValueList() override;

    void combine() override;

private:
    QVariantList values;
};

class OrderByConstraintConnector : public ConstraintConnector {
public:
    template<typename T, typename... E>
    void orderBy(const EntityField<T>& field, const EntityField<E>&... next) {
        addField(getEntityFieldInfo(field));
        orderBy(next...);
    }

    void orderBy() {}

    void combine() override;
};

class GroupByConstraintConnector : public ConstraintConnector {
public:
    template<typename T, typename... E>
    void groupBy(const EntityField<T>& field, const EntityField<E>&... next) {
        addField(getEntityFieldInfo(field));
        groupBy(next...);
    }

    void groupBy() {}

    void combine() override;
};

QTDAO_END_NAMESPACE