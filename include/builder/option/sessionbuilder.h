#pragma once

#include "../../global.h"

QTDAO_BEGIN_NAMESPACE

template<typename T>
class SessionBuilder {
public:
    explicit SessionBuilder(qint64 sessionId = -1) : querySessionId(sessionId) {}

public:
    //query used session id
    qint64 querySessionId;
};

QTDAO_END_NAMESPACE