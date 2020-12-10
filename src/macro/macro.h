#pragma once

#define QUERY_BUILDER_USE(T, name) \
template<typename... Args>\
T& name(const Args&... args) {\
    __super::name(args...);\
    return *this;\
}

#define QUERY_BUILDER_USE_THROWABLE(T)\
T& throwable() {\
    setThrowable = true;\
    return *this;\
}

#define QUERY_BUILDER_USE_SET(T)            QUERY_BUILDER_USE(T, set)
#define QUERY_BUILDER_USE_FILTER(T)         QUERY_BUILDER_USE(T, filter)
#define QUERY_BUILDER_USE_WITH(T)           QUERY_BUILDER_USE(T, with)
#define QUERY_BUILDER_USE_COLUMN(T)         QUERY_BUILDER_USE(T, column)
#define QUERY_BUILDER_USE_ON(T)             QUERY_BUILDER_USE(T, on)

#define QUERY_BUILDER_USE_QUERY_FROM(B)\
B& from(Select<T>& select){\
    __super::from(select);\
    return *this;\
}

#define BASE_QUERY_CONSTRUCTOR_DECLARE(Q)\
friend class Q##Builder<E>;\
Q(bool throwable, Q##Builder<E>* builder): BaseQuery(throwable, builder) {}

#define QUERY_BUILDER_BUILDER_DECLARE(Q)\
Q<T> build() {\
    return Q<T>(setThrowable, this);\
}