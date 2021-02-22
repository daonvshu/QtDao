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
        QStringList fieldsType,
        QStringList primaryKeys
    );

    void createIndex(
        const QString& tbName,
        QStringList fields,
        IndexType type,
        const std::function<QString(const QString&)>& optionGet
    );

    void renameTable(const QString& oldName, const QString& newName);

    void dropTable(const QString& tbName);

    void truncateTable(const QString& tbName);

    QStringList getTagTableFields(const QString& tbName);

    void restoreDataBefore(const QString& tbName);

    void restoreDataAfter(const QString& tbName);

    void dropAllIndexOnTable(const QString& tbName);
};