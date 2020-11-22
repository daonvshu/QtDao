#pragma once

#include "QueryBuilder.h"

#include "../query/Select.h"

template<typename T>
class SelectBuilder : public QueryBuilder<T, Select> {
public:
    template<typename... Args>
    SelectBuilder& filter(const EntityCondition& condition, const Args&... args);

    template<typename... Args>
    SelectBuilder& filter(const Connector& condition, const Args&... args);

    template<typename... Args>
    SelectBuilder& with(const ConditionConstraint& constaint, const Args&... args);

    SelectBuilder& filter();

    Select<T> build() override;

private:
    Connector filterCondition, constraintCondition;
};