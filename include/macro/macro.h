#pragma once

#define QUERY_BUILDER_USE(Builder, name) \
template<typename... Args>\
Builder& name(const Args&... args) {\
    BaseQueryBuilder::name(args...);\
    return *this;\
}

#define QUERY_BUILDER_USE_THROWABLE(Builder)\
Builder& throwable() {\
    this->setThrowable = true;\
    return *this;\
}

#define QUERY_BUILDER_USE_SET(Builder)            QUERY_BUILDER_USE(Builder, set)
#define QUERY_BUILDER_USE_FILTER(Builder)         QUERY_BUILDER_USE(Builder, filter)
#define QUERY_BUILDER_USE_WITH(Builder)           QUERY_BUILDER_USE(Builder, with)
#define QUERY_BUILDER_USE_COLUMN(Builder)         QUERY_BUILDER_USE(Builder, column)
#define QUERY_BUILDER_USE_ON(Builder)             QUERY_BUILDER_USE(Builder, on)

#define QUERY_BUILDER_USE_COLUMNALL(Builder)\
Builder& columnAll() {\
    BaseQueryBuilder::columnAll<E>();\
    return *this;\
}

#define QUERY_BUILDER_USE_QUERY_FROM_SELECT(Builder)\
Builder& from(Select<E>& select){\
    BaseQueryBuilder::from(select);\
    return *this;\
}

#define QUERY_BUILDER_USE_QUERY_FROM_SELECT2(Builder)\
template<typename E2>\
Builder& from(Select<E2>& select){\
    BaseQueryBuilder::from(select);\
    return *this;\
}

#define QUERY_BUILDER_USE_QUERY_FROM_JOIN(Builder)\
Builder& from(Join<E...>& join) {\
    BaseQueryBuilder::from(join);\
    return *this;\
}

#define QUERY_BUILDER_USE_QUERY_FROM_JOIN2(Builder)\
template<typename... E2>\
Builder& from(Join<E2...>& join) {\
    BaseQueryBuilder::from(join);\
    return *this;\
}

#define QUERY_BUILDER_USE_QUERY_FROM_RECURSIVE(Builder)\
Builder& from(RecursiveQueryBuilder& builder) {\
    BaseQueryBuilder::from(builder);\
    return *this;\
}

#define QUERY_BUILDER_USE_QUERY_UNION_SELECT(Builder)\
template<typename E2>\
Builder& unionSelect(Select<E2>& select, bool unionAll = false) {\
    BaseQueryBuilder::unionSelect(select, unionAll);\
    return *this;\
}

#define QUERY_BUILDER_USE_QUERY_UNION_JOIN(Builder)\
template<typename... E2>\
Builder& unionSelect(Join<E2...>& join, bool unionAll = false) {\
    BaseQueryBuilder::unionSelect(join, unionAll);\
    return *this;\
}


#define BASE_QUERY_CONSTRUCTOR_DECLARE(Q)\
friend class Q##Builder<E>;\
Q(bool throwable, Q##Builder<E>* builder): BaseQuery(throwable, builder) {}

#define BASE_QUERY_CONSTRUCTOR_DECLARE_W(Q)\
friend class Q##Builder<E>;\
Q(bool throwable, Q##Builder<E>* builder): BaseQuery(throwable, builder, true) {}

#define QUERY_BUILDER_BUILDER_DECLARE(Q)\
Q<E> build() {\
    return Q<E>(setThrowable, this);\
}
