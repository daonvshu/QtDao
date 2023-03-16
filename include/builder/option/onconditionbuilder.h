#pragma once

#include "../../global.h"

#include "../../condition/entitycondition.h"
#include "../../condition/connector.h"
#include "../../condition/functioncondition.h"

QTDAO_BEGIN_NAMESPACE

//same as FilterBuilder
template<typename T>
class OnConditionBuilder {
public:
    /**
     * add some on filter conditions, example:
     * fields.name == "Alice",  _and(fields.name == "Alice", fields.age == 1),
     * _fun("%1 + %2 > 100").field(fields.score1, fields.score2)
     * @tparam Args condition type, EntityCondition/Connector/FunctionCondition
     * @param args conditions
     * @return this
     */
    template<typename...Args>
    T& on(const Args &...args) {
        on(args...);
        return static_cast<T&>(*this);
    }

    /**
     * add a on filter condition, using enabled to add optional
     * @tparam Arg condition type, EntityCondition/Connector/FunctionCondition
     * @param enabled add condition if enabled
     * @param arg conditions
     * @return this
     */
    template<typename Arg>
    T& on(bool enabled, const Arg &arg) {
        on(enabled, arg);
        return static_cast<T&>(*this);
    }

protected:
    /**
     * use fields to add on filter conditions, example: fields.name == "Alice"
     * @tparam Args condition type, EntityCondition/Connector/FunctionCondition
     * @param condition field condition
     * @param args other types of conditions
     */
    template<typename... Args>
    void on(const EntityCondition& condition, const Args&... args) {
        onCondition.append(condition);
        on(args...);
    }

    /**
     * use fields to add on filter conditions, using enabled to add optional
     * @param enabled add condition if enabled
     * @param condition field condition
     */
    void on(bool enabled, const EntityCondition& condition) {
        if (enabled) {
            onCondition.append(condition);
        }
    }

    /**
     * use combination condition to add on filter conditions,
     * example: _and(fields.name == "Alice", fields.age == 1)
     * @tparam Args condition type, EntityCondition/Connector/FunctionCondition
     * @param condition combination condition
     * @param args other types of conditions
     */
    template<typename... Args>
    void on(const Connector& condition, const Args&... args) {
        onCondition.append(condition);
        on(args...);
    }

    /**
     * use combination condition to add on filter conditions, using enabled to add optional
     * @param enabled add condition if enabled
     * @param condition combination condition
     */
    void on(bool enabled, const Connector& condition) {
        if (enabled) {
            onCondition.append(condition);
        }
    }

    /**
     * use custom condition to add on filter conditions,
     * example: _fun("%1 + %2 > 100").field(fields.score1, fields.score2)
     * @tparam Args condition type, EntityCondition/Connector/FunctionCondition
     * @param condition function condition
     * @param args other types of conditions
     */
    template<typename... Args>
    void on(const FunctionCondition& condition, const Args&... args) {
        onCondition.append(condition);
        on(args...);
    }

    /**
     * use custom condition to add on filter conditions, using enabled to add optional
     * @param enabled add condition if enabled
     * @param condition function condition
     */
    void on(bool enabled, const FunctionCondition& condition) {
        if (enabled) {
            onCondition.append(condition);
        }
    }

    /**
     * end function recursion
     */
    virtual void on() {}

private:
    //use 'and' connect conditions
    Connector onCondition{"and"};
};

QTDAO_END_NAMESPACE