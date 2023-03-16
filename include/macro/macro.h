#pragma once

#define BASE_QUERY_CONSTRUCTOR_DECLARE(Q)\
friend class Q##Builder<E>;\
Q(Q##Builder<E>* builder): Q##Impl(builder) {}
