#pragma once

#include "../global.h"
#include "../builder/option/fromselectbuilder.h"

#include "entityfield.h"

QTDAO_BEGIN_NAMESPACE

class FunctionConnector : Connectable, public FromE2SelectBuilder<FunctionConnector> {
public:
    template<typename T, typename... E>
    FunctionConnector& field(const EntityField<T>& f, const EntityField<E>&... n) {
        fields << getEntityFieldInfo(f);
        return field(n...);
    }

    FunctionConnector& field() {
        return *this;
    }

    template<typename... Args>
    FunctionConnector& value(const QVariant& v, Args... args) {
        values << v;
        return value(args...);
    }

    FunctionConnector& value() {
        return *this;
    }

    QList<FieldInfo> getUsedFields() override;

    QString getConditionSegment() override;

    QVariantList getValueList() override;

    void combine() override;

    Connectable* ptr() {
        return this;
    }

protected:
    void fromSelect(SelectImpl& select) override;

    void fromJoin(JoinImpl& join) override;

    void fromBuilder(RecursiveQueryBuilder& builder) override;

    void solveFromQueryBuildResult();

    QString getField(int index) const;

private:
    QString expressions;
    QList<FieldInfo> fields;
    QVariantList values;
    QString connectedStr;
};

QTDAO_END_NAMESPACE