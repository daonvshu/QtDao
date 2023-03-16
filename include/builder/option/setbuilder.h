#pragma once

#include "../../global.h"

#include "../../condition/entitycondition.h"
#include "../../condition/connector.h"

QTDAO_BEGIN_NAMESPACE

template<typename T>
class SetBuilder {
public:
    /**
     * add some set conditions, example:
     * fields.name = "Alice", fields.age = 18
     * @tparam Args condition type, EntityCondition
     * @param args set conditions
     * @return this
     */
    template<typename...Args>
    T& set(const Args &...args) {
        set(args...);
        return static_cast<T&>(*this);
    }

    /**
     * add a set condition, using enabled to add optional
     * @tparam Arg condition type, EntityCondition
     * @param enabled add condition if enabled
     * @param args set conditions
     * @return this
     */
    template<typename Arg>
    T& set(bool enabled, const Arg &arg) {
        set(enabled, arg);
        return static_cast<T&>(*this);
    }

private:
    /**
     * use fields to add set conditions, example:
     * fields.name = "Alice", fields.age = 18
     * @tparam Args EntityCondition
     * @param condition set conditions
     * @param args other fields of conditions
     */
    template<typename... Args>
    void set(const EntityCondition& condition, const Args&... args){
        setCondition.append(condition);
        return set(args...);
    }

    /**
     * add a set conditions, using enabled to add optional
     * @param enabled add condition if enabled
     * @param condition set conditions
     */
    void set(bool enabled, const EntityCondition& condition){
        if (enabled) {
            setCondition.append(condition);
        }
    }

    /**
     * end function recursion
     */
    void set() {}

private:
    //use ',' connect conditions
    Connector setCondition{","};
};

QTDAO_END_NAMESPACE