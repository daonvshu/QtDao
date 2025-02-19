#pragma once

#include "configbuilder.h"

QTDAO_BEGIN_NAMESPACE

class ConfigPSqlBuilder : public ConfigBuilder {
public:
    ConfigPSqlBuilder();

    ConfigPSqlBuilder& version(int ver) override;

    ConfigPSqlBuilder& driver(const QString &driver) override;

    ConfigPSqlBuilder& databaseName(const QString &name) override;

    ConfigBuilder& configAlias(const QString &alias) override;

    ConfigBuilder& session(qint64 sessionId) override;

    ConfigPSqlBuilder& password(const QString &password) override;

    ConfigPSqlBuilder& options(const QString &options) override;

    ConfigPSqlBuilder& host(const QString& host);

    ConfigPSqlBuilder& user(const QString& user);

    ConfigPSqlBuilder& port(int port);

    ConfigPSqlBuilder& owner(const QString& owner);

    ConfigPSqlBuilder& encoding(const QString& encoding);

    ConfigPSqlBuilder& collate(const QString& collate);

    ConfigPSqlBuilder& ctype(const QString& ctype);

    ConfigPSqlBuilder& tableSpace(const QString& tableSpace);

    QSqlDatabase getConnection(const QString &connectionName, const QString &databaseName) override;

    void initializeDatabase() override;

private:
    QString mHost;
    QString mUser;
    int mPort;
    QString mOwner;
    QString mEncoding;
    QString mCollate;
    QString mCType;
    QString mTableSpace;

    friend class PSqlClient;
};

QTDAO_END_NAMESPACE
