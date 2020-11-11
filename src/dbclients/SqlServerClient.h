#pragma once

#include "AbstractClient.h"

class SqlServerClient : public AbstractClient {
public:
    void testConnect();
    void createDatabase();
    void dropDatabase();
};