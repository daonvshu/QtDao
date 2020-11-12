#pragma once

#include <qobject.h>

struct DbConfig {
    /// <summary>
    /// 目标数据库版本号
    /// </summary>
    int version;

    /// <summary>
    /// 读取本地版本号后设置，为false时需要版本升级
    /// </summary>
    bool versionValid;

    /// <summary>
    /// 数据库类型，QSQLITE,QMYSQL,QODBC其中之一
    /// </summary>
    QString dbType;

    /// <summary>
    /// 数据库名称
    /// </summary>
    QString dbName;

    /// <summary>
    /// 数据库地址
    /// </summary>
    QString dbHost;

    /// <summary>
    /// 连接用户名
    /// </summary>
    QString dbUName;

    /// <summary>
    /// 连接密码
    /// </summary>
    QString dbPcc;

    /// <summary>
    /// 连接端口
    /// </summary>
    int dbPort;

    /// <summary>
    /// 连接配置参数
    /// </summary>
    QString dbOption;

    /// <summary>
    /// 检查是否是sqlite
    /// </summary>
    /// <returns></returns>
    bool isSqlite() {
        return dbType == "QSQLITE";
    }

    /// <summary>
    /// 检测是否是mysql
    /// </summary>
    /// <returns></returns>
    bool isMysql() {
        return dbType == "QMYSQL";
    }

    /// <summary>
    /// 检测是否是SQLServer
    /// </summary>
    /// <returns></returns>
    bool isSqlServer() {
        return dbType == "QODBC";
    }
};