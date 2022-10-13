#pragma once

#include "../global.h"

#include "abstractclient.h"

QTDAO_BEGIN_NAMESPACE

class MysqlClient : public AbstractClient {
public:
    void testConnect() override;
    void createDatabase() override;
    void dropDatabase() override;

    bool checkTableExist(const QString& tbName) override;
    void createTableIfNotExist(
        const QString& tbName,
        const QString& engine,
        QStringList fieldsType,
        QStringList primaryKeys
    ) override;

    void createIndex(
        const QString& tbName,
        QStringList fields,
        IndexType type
    ) override;

    void renameTable(const QString& oldName, const QString& newName) override;

    void dropTable(const QString& tbName) override;

    void truncateTable(const QString& tbName) override;

    QStringList getTagTableFields(const QString& tbName) override;

    void dropAllIndexOnTable(const QString& tbName) override;
};

QTDAO_END_NAMESPACE