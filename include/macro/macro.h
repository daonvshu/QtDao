#pragma once

#define BASE_QUERY_CONSTRUCTOR_DECLARE(Q)\
friend class Q##Builder<E>;\
explicit Q(Q##Builder<E> builder): BuilderReaderProvider<Q##Builder, E>(std::move(builder)) {}
