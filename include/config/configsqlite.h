#pragma once

#include "configbuilder.h"

QTDAO_BEGIN_NAMESPACE

class ConfigSqliteBuilder : public ConfigBuilder {
public:
    explicit ConfigSqliteBuilder();

    ConfigSqliteBuilder& version(int ver) override;

    ConfigSqliteBuilder& driver(const QString& driver) override;

    ConfigSqliteBuilder& databaseName(const QString& name) override;

    ConfigSqliteBuilder& password(const QString& password) override;

    ConfigSqliteBuilder& options(const QString& options) override;

    ConfigSqliteBuilder& saveDir(const QString& path);

    QSqlDatabase getConnection(const QString& connectionName, const QString& databaseName) override;

    void initializeDatabase() override;

    QString getDbStoreDirectory() const;

    QString getDbStorePath() const;

private:
    QString mSaveDir;

    friend class SqliteClient;
};

QTDAO_END_NAMESPACE