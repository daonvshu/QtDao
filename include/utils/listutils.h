#pragma once

#include "../global.h"
#include <qlist.h>
#include <functional>

QTDAO_BEGIN_NAMESPACE

template<typename T, typename K>
static QList<T> listMap(const QList<K>& src, const std::function<T(const K&)>& transfer) {
    QList<T> data;
    for (const auto& s : src) {
        data << transfer(s);
    }
    return data;
}

template<typename T, typename K, typename L>
static QList<T> listMap(const QList<K>& src, L* context, T(L::*transfer)(const K&)) {
    QList<T> data;
    for (const auto& s : src) {
        data << (context->*transfer)(s);
    }
    return data;
}

QTDAO_END_NAMESPACE