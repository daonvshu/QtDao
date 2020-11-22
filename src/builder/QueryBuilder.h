#pragma once

#include "../condition/EntityCondition.h"
#include "../condition/Connector.h"

#include "../query/BaseQuery.h"

template<typename T, template<typename> class Query>
class QueryBuilder {
public:
    QueryBuilder() : setThrowable(false) {
    }

    template<typename... Args>
    QueryBuilder& set(const EntityCondition& condition, const Args&... args);

    QueryBuilder& set();

    QueryBuilder& throwable();

    Query<T> build();

private:
    Connector connector;
    bool setThrowable;
};

template<typename T, template<typename> class Query>
template<typename ...Args>
inline QueryBuilder<T, Query>& QueryBuilder<T, Query>::set(const EntityCondition& condition, const Args & ...args) {
    connector.append(condition);
    return set(args...);
}

template<typename T, template<typename> class Query>
inline QueryBuilder<T, Query>& QueryBuilder<T, Query>::set() {
    return *this;
}

template<typename T, template<typename> class Query>
inline QueryBuilder<T, Query>& QueryBuilder<T, Query>::throwable() {
    setThrowable = true;
    return *this;
}

template<typename T, template<typename> class Query>
inline Query<T> QueryBuilder<T, Query>::build() {
    Q_STATIC_ASSERT((std::is_base_of<BaseQuery, Query<T>>::value));
    Query<T> query;
    query.connector = connector;
    query.queryThrowable = setThrowable;
    return query;
}
