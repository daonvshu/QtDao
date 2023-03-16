#pragma once

#include "../../global.h"

#include "../../condition/connector.h"

QTDAO_BEGIN_NAMESPACE

template<typename T>
class ConstraintBuilder {
public:
    /**
     * add constraint conditions, example:
     * _limit(5, 20), _orderBy(field.name, field.age)
     * @tparam Args type of ConditionConstraint
     * @param constraint condition
     * @param args other types of constraint condition
     * @return this
     */
    template<typename... Args>
    T& with(const ConditionConstraint& constraint, const Args&... args) {
        constraintCondition.append(constraint);
        return with(args...);
    }

    /**
     * add constraint condition, using enabled to add condition optional
     * @param enabled add condition if enabled
     * @param constraint condition
     * @return this
     */
    T& with(bool enabled, const ConditionConstraint& constraint) {
        if (enabled) {
            constraintCondition.append(constraint);
        }
        return static_cast<T&>(*this);
    }

    /**
     * end function recursion
     * @return this
     */
    T& with() {
        return static_cast<T&>(*this);
    }

private:
    //use default connect conditions
    Connector constraintCondition;
};

QTDAO_END_NAMESPACE