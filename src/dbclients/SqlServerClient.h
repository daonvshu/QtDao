#pragma once

#include "AbstractClient.h"

class SqlServerClient : public AbstractClient {
public:
    void testConnect();
    void createDatabase();
    void dropDatabase();

    bool checkTableExist(const QString& tbName);
    void createTableIfNotExist(
        const QString& tbName,
        QStringList fieldsType
    );
};