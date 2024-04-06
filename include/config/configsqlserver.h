#pragma once

#include "configbuilder.h"

QTDAO_BEGIN_NAMESPACE

class ConfigSqlServerBuilder : public ConfigBuilder {
public:
    explicit ConfigSqlServerBuilder();

    ConfigSqlServerBuilder& version(int ver) override;

    ConfigSqlServerBuilder& driver(const QString& driver) override;

    ConfigSqlServerBuilder& databaseName(const QString& name) override;

    ConfigBuilder & configAlias(const QString &alias) override;

    ConfigBuilder & session(qint64 sessionId) override;

    ConfigSqlServerBuilder& password(const QString& password) override;

    ConfigSqlServerBuilder& options(const QString& options) override;

    ConfigSqlServerBuilder& host(const QString& host);

    ConfigSqlServerBuilder& user(const QString& user);

    ConfigSqlServerBuilder& port(int port);

    QSqlDatabase getConnection(const QString& connectionName, const QString& databaseName) override;

    void initializeDatabase() override;

private:
    QString mHost;
    QString mUser;
    int mPort;
};

QTDAO_END_NAMESPACE