#pragma once

#include "../global.h"

QTDAO_BEGIN_NAMESPACE

struct VariantCastUtil {
    template<typename E>
    static QVariant getSupportedValue(const typename std::enable_if<std::is_constructible<QVariant, E>::value, E>::type& v) {
        return v;
    }

    template<typename E>
    static QVariant getSupportedValue(const typename std::enable_if<!std::is_constructible<QVariant, E>::value, E>::type& v) {
        qFatal("used error type!");
        return {};
    }

    template<typename E>
    static QVariant getSupportedValue(const typename std::enable_if<std::is_constructible<QVariant, E>::value, QList<E>>::type& v) {
        return v;
    }

    template<typename E>
    static QVariant getSupportedValue(const typename std::enable_if<!std::is_constructible<QVariant, E>::value, QList<E>>::type& v) {
        qFatal("used error type!");
        return {};
    }

    template<typename E>
    static const QList<E>& getSupportedValues(const typename std::enable_if<std::is_constructible<QVariant, E>::value, QList<E>>::type& v) {
        return v;
    }

    template<typename E>
    static QList<E> getSupportedValues(const typename std::enable_if<!std::is_constructible<QVariant, E>::value, QList<E>>::type& v) {
        qFatal("used error type!");
        return {};
    }
};

QTDAO_END_NAMESPACE