#pragma once

#include "configbuilder.h"

QTDAO_BEGIN_NAMESPACE

class ConfigMysqlBuilder : public ConfigBuilder {
public:
    explicit ConfigMysqlBuilder();

    ConfigMysqlBuilder& version(int ver) override;

    ConfigMysqlBuilder& driver(const QString& driver) override;

    ConfigMysqlBuilder& databaseName(const QString& name) override;

    ConfigMysqlBuilder& password(const QString& password) override;

    ConfigMysqlBuilder& options(const QString& options) override;

    ConfigMysqlBuilder& host(const QString& host);

    ConfigMysqlBuilder& user(const QString& user);

    ConfigMysqlBuilder& port(int port);

    QSqlDatabase getConnection(const QString& connectionName, const QString& databaseName) override;

    void initializeDatabase() override;

private:
    QString mHost;
    QString mUser;
    int mPort;
};

QTDAO_END_NAMESPACE