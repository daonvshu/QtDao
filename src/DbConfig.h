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
    /// 数据库类型，sqlite,mysql,sqlserver其中之一
    /// </summary>
    QString dbType;

    /// <summary>
    /// 数据库驱动，默认QSQLITE,QMYSQL,QODBC其中之一
    /// </summary>
    QString dbDriver;

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
        return dbType == "sqlite";
    }

    /// <summary>
    /// 检测是否是mysql
    /// </summary>
    /// <returns></returns>
    bool isMysql() {
        return dbType == "mysql";
    }

    /// <summary>
    /// 检测是否是SQLServer
    /// </summary>
    /// <returns></returns>
    bool isSqlServer() {
        return dbType == "sqlserver";
    }
};