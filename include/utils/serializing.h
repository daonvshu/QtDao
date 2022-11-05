#pragma once

#include "../global.h"

#include <qdatastream.h>

QTDAO_BEGIN_NAMESPACE

template<typename T>
static QByteArray serializeCustomTypeToBinary(const T& data) {
    QByteArray serializedData;
    QDataStream load(&serializedData, QIODevice::WriteOnly);
    load << data;
    return serializedData;
}

template<typename T>
static T deserializeBinaryToCustomType(const QByteArray& data) {
    T t;
    QDataStream load(data);
    load >> t;
    return t;
}

QTDAO_END_NAMESPACE