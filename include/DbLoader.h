#pragma once

#include <qobject.h>

#include "DbConfig.h"
#include "DbExceptionHandler.h"
#include "dbclients/AbstractClient.h"

class DbLoader {
public:
    DbLoader() = delete;

    /// <summary>
    /// load config, use object's property to get config parameters
    /// </summary>
    /// <param name="config"></param>
    static void loadConfig(const QObject& config);

    /// <summary>
    /// load config and init, use object's property to get config parameters
    /// <para>properties:</para>
    /// <para>type: database type, one of sqlite,mysql,sqlserver</para>
    /// <para>driver(optional): database driver, default one of QSQLITE,QMYSQL,QODBC</para>
    /// <para>version: expect database version</para>
    /// <para>dbName: database name</para>
    /// <para>dbHost(optional): database connect address</para>
    /// <para>dbUName(optional): database connect user name</para>
    /// <para>dbPcc(optional): database connect user password</para>
    /// <para>dbPort(optional): database connect used port</para>
    /// <para>dbOption(optional): database connect options</para>
    /// </summary>
    /// <param name="config"></param>
    /// <param name="exceptionHandler"></param>
    static bool init(const QObject& config, DbExceptionHandler* exceptionHandler = nullptr);

    /// <summary>
    /// get config ref
    /// </summary>
    /// <returns></returns>
    static DbConfig& getConfig() {
        return config;
    }

    /// <summary>
    /// get init client
    /// </summary>
    /// <returns></returns>
    static AbstractClient& getClient() {
        return *sqlClient;
    }

    /// <summary>
    /// get current database version, return -1 if not exist
    /// </summary>
    /// <returns></returns>
    static int getLocalVersion();

    /// <summary>
    /// disable auto create database, need call before 'init'
    /// </summary>
    static void disableCreateDatabase();

    /// <summary>
    /// disable auto create tables, need call before 'init'
    /// </summary>
    static void disableCreateTable();

private:
    static DbConfig config;
    static AbstractClient* sqlClient;

private:
    static bool init();
    static void init_priv();

    static void updateLocalVersion();

    static void invokeCreateTables();
    static void invokeTableUpgrade();

    static QByteArray getDelegateStr();
};
