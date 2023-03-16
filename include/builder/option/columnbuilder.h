#pragma once

#include "../../global.h"

#include "../../condition/connector.h"

QTDAO_BEGIN_NAMESPACE

template<typename T>
class ColumnBuilder {
public:
    /**
     * use fields or custom function to add select columns, example:
     * fields.name, _fun("sum(%1) as scores").field(fields.score)
     * @tparam Args condition type, Entity/FunctionCondition
     * @param args fields or custom function
     * @return this
     */
    template<typename...Args>
    T& column(const Args &...args) {
        column(args...);
        return static_cast<T&>(*this);
    }

    /**
     * use fields or custom function to add select columns, using enabled to add column optional
     * @tparam Arg type of fields or custom function
     * @param enabled add column if enabled
     * @param arg fields or custom function
     * @return this
     */
    template<typename Arg>
    T& column(bool enabled, const Arg &arg) {
        column(enabled, arg);
        return static_cast<T&>(*this);
    }

    /**
     * add target entity all columns
     * @tparam E entity type
     */
    template<typename E>
    void columnAll(){
        QString tbName = E::Info::getTableName();
        QStringList fields = E::Info::getFields();
        for (const auto& field : fields) {
            columnCondition.appendCol(FieldInfo{ field, tbName });
        }
    }

private:
    /**
     * use fields or custom function to add select columns, example:
     * fields.name, _fun("sum(%1) as scores").field(fields.score)
     * @tparam Col type of fields or custom function
     * @tparam Args condition type, Entity/FunctionCondition
     * @param function fields or custom function
     * @param args other types of columns
     */
    template<typename Col, typename... Args>
    void column(const Col& function, const Args&... args){
        columnCondition.appendCol(function);
        column(args...);
    }

    /**
     * use fields or custom function to add select columns, using enabled to add column optional
     * @tparam Col type of fields or custom function
     * @param enabled add column if enabled
     * @param function fields or custom function
     */
    template<typename Col>
    void column(bool enabled, const Col& function){
        if (enabled) {
            columnCondition.appendCol(function);
        }
    }

    /**
     * end function recursion
     */
    void column() {}

private:
    //use ',' connect conditions
    Connector columnCondition{","};
};

QTDAO_END_NAMESPACE