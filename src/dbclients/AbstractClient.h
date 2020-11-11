#pragma once

class AbstractClient {
public:
    virtual void testConnect() = 0;
    virtual void createDatabase() = 0;
    virtual void dropDatabase() = 0;
};

