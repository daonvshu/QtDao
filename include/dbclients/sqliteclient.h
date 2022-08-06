#pragma once

#include "../global.h"

#include "abstractclient.h"

QTDAO_BEGIN_NAMESPACE

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

    void renameTable(const QString& oldName, const QString& newName);

    void dropTable(const QString& tbName);

    void truncateTable(const QString& tbName);

    QStringList getTagTableFields(const QString& tbName);

    void dropAllIndexOnTable(const QString& tbName);
};

QTDAO_END_NAMESPACE