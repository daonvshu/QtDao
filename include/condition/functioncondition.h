#pragma once

#include "../global.h"
#include "../builder/option/fromselectbuilder.h"

#include "connector/connectable.h"

QTDAO_BEGIN_NAMESPACE

template<typename T>
class EntityField;

class FunctionConnector : Connectable, public FromE2SelectBuilder<FunctionConnector> {
public:
    explicit FunctionConnector(QString expressions): expressions(std::move(expressions)) {}

    template<typename T, typename... E>
    FunctionConnector& field(const EntityField<T>& f, const EntityField<E>&... n) {
        d->fields << getEntityFieldInfo(f);
        return field(n...);
    }

    FunctionConnector& field() {
        return *this;
    }

    template<typename... Args>
    FunctionConnector& value(const QVariant& v, Args... args) {
        d->values << v;
        return value(args...);
    }

    FunctionConnector& value() {
        return *this;
    }

    QList<FieldInfo> getUsedFields() override;

    void combine() override;

    Connectable* ptr() {
        return this;
    }

private:
    bool isEmpty() override;

protected:
    void fromSelect(SelectImpl& select) override;

    void fromJoin(JoinImpl& join) override;

    void fromBuilder(RecursiveQueryBuilder& builder) override;

    void solveFromQueryBuildResult();

private:
    QString expressions;
};

QTDAO_END_NAMESPACE