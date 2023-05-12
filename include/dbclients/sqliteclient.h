#pragma once

#include "../global.h"

#include "abstractclient.h"

QTDAO_BEGIN_NAMESPACE

class SqliteClient : public AbstractClient {
public:
    void testConnect() override;

    void createDatabase() override;

    void dropDatabase() override;

    bool checkTableExist(const QString& tbName) override;

    void createTable(dao::EntityReaderInterface *reader) override;

    void renameTable(const QString& oldName, const QString& newName) override;

    void dropTable(const QString& tbName) override;

    void truncateTable(const QString& tbName) override;

    QStringList getTagTableFields(const QString& tbName) override;

    void dropAllIndexOnTable(const QString& tbName) override;

    static void createTableIfNotExist(
            const QString& tbName,
            const QStringList& fieldsType,
            const QStringList& primaryKeys
    );

    static void createIndex(
            const QString& tbName,
            const QStringList& fields,
            IndexType type
    );
};

QTDAO_END_NAMESPACE