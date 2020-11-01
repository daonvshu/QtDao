#pragma once

#include <qobject.h>

#include "DbConfig.h"
#include "DbExceptionHandler.h"

class DbInitClient;
class DbLoader {
public:
    DbLoader() = delete;

    /// <summary>
    /// 加载配置文件
    /// </summary>
    /// <param name="configPath">配置文件路径</param>
    /// <param name="exceptionHandler">错误处理</param>
    static void loadConfig(const QString& configPath, DbExceptionHandler* exceptionHandler = nullptr);

    /// <summary>
    /// 初始化数据库
    /// </summary>
    static void init();

    /// <summary>
    /// 加载配置文件并初始化
    /// </summary>
    /// <param name="configPath">配置文件路径</param>
    /// <param name="exceptionHandler">错误处理</param>
    static void init(const QString& configPath, DbExceptionHandler* exceptionHandler = nullptr);

    /// <summary>
    /// 获取数据库配置参数引用
    /// </summary>
    /// <returns></returns>
    static DbConfig& getConfig() {
        return config;
    }

private:
    static DbConfig config;
    static DbInitClient* initClient;

private:
    static void init_priv();
};

class DbInitClient {
public:
    virtual void testConnect() = 0;
    virtual void createDatabase() = 0;
};