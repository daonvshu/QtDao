#pragma once

#include "AbstractClient.h"

class SqliteClient : public AbstractClient {
public:
    void testConnect();
    void createDatabase();
    void dropDatabase();
};