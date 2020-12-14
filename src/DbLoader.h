#pragma once

#include <qobject.h>

#include "DbConfig.h"
#include "DbExceptionHandler.h"
#include "dbclients/AbstractClient.h"

class DbLoader {
public:
    DbLoader() = delete;

    /// <summary>
    /// 加载配置，通过property获取配置数据
    /// </summary>
    /// <param name="config"></param>
    static void loadConfig(const QObject& config);

    /// <summary>
    /// 加载配置并初始化
    /// </summary>
    /// <param name="config"></param>
    /// <param name="exceptionHandler"></param>
    static void init(const QObject& config, DbExceptionHandler* exceptionHandler = nullptr);

    /// <summary>
    /// 获取数据库配置参数引用
    /// </summary>
    /// <returns></returns>
    static DbConfig& getConfig() {
        return config;
    }

    /// <summary>
    /// 获取数据库初始化器
    /// </summary>
    /// <returns></returns>
    static AbstractClient& getClient() {
        return *sqlClient;
    }

    /// <summary>
    /// 获取当前数据库版本号，不存在返回-1
    /// </summary>
    /// <returns></returns>
    static int getLocalVersion();

private:
    static DbConfig config;
    static AbstractClient* sqlClient;

private:
    static void init();
    static void init_priv();

    static void updateLocalVersion();

    static void invokeCreateTables();
    static void invokeTableUpgrade();

    static QByteArray getDelegateStr();
};
