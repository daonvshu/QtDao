#pragma once

#include "AbstractClient.h"

class SqliteClient : public AbstractClient {
public:
    void testConnect();
    void createDatabase();
    void dropDatabase();

    bool checkTableExist(const QString& tbName);
    void createTableIfNotExist(
        const QString& tbName,
        QStringList fieldsType,
        QStringList primaryKeys
    );

    void createIndex(
        const QString& tbName,
        QStringList fields,
        IndexType type = INDEX_NORMAL
    );
};