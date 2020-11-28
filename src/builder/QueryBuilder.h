#pragma once

#include "../condition/EntityCondition.h"
#include "../condition/Connector.h"
#include "../condition/FunctionCondition.h"
#include "../condition/EntityField.h"

#include "../query/BaseQuery.h"

template<typename T, template<typename> class Query>
class QueryBuilder {
public:
    QueryBuilder() : setThrowable(false) {
    }

    QueryBuilder& throwable();

    virtual Query<T> build() = 0;

protected:
    bool setThrowable;
};

template<typename T, template<typename> class Query>
inline QueryBuilder<T, Query>& QueryBuilder<T, Query>::throwable() {
    setThrowable = true;
    return *this;
}
