#pragma once

#include "AbstractClient.h"

class MysqlClient : public AbstractClient {
public:
    void testConnect();
    void createDatabase();
    void dropDatabase();
};