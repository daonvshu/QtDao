---
title: 代码初始化
category: config
layout: post
---

创建连接配置文件
-------------
`QtDao`通过读取配置类的Q_PROPERTY属性获取需要的连接参数，可配置的属性如下表：

|属性|sqlite|mysql|sqlserver|
|:--:|:--:|:--:|:--:|
|`version`|✔️|✔️|✔️|
|`type`|✔️|✔️|✔️|
|`driver`|✔️|✔️|✔️|
|`dbName`|✔️|✔️|✔️|
|`dbHost`||✔️|✔️|
|`dbUName`||✔️|✔️|
|`dbPcc`|✔️|✔️|✔️|
|`dbPort`||✔️|✔️|
|`dbOption`|✔️|✔️|✔️|
|`saveDirectory`|✔️|||

- `version`

设置当前数据库版本号，若开启自动升级功能（默认），QtDao初始化时检测到版本号发生变化会自动执行数据表结构的升级，同时把旧的数据尝试插入到新的表结构中。数据库版本号应该在配置表结构发生变化时变更，变更后下次程序启动时自动执行升级过程。

- `type`

选择连接的数据库类型，必须是以下3种：`sqlite`、`mysql`、`sqlserver`

- `driver`

选择数据库驱动。默认情况下根据`type`自动设置，`sqlite` -> `QSQLITE`，`mysql` -> `QMYSQL`，`sqlserver` -> `QODBC`，当使用第三方驱动时可显式配置使用的驱动名称（如使用[QtCipherSqlitePlugin](https://github.com/devbean/QtCipherSqlitePlugin){:target="_blank"}时配置driver为 `SQLITECIPHER`）。

- `dbName`

设置连接的数据库名称，对于sqlite为数据库文件名。

- `dbHost`

设置连接数据库的地址

- `dbUName`

设置连接时登录的用户名

- `dbPcc`
  
设置连接时登录使用的密码

- `dbPort`

设置连接数据库的端口

- `dbOption`

设置连接参数，连接可用的参数可参考官方文档：[https://doc.qt.io/qt-5/qsqldatabase.html#setConnectOptions](https://doc.qt.io/qt-5/qsqldatabase.html#setConnectOptions){:target="_blank"}

- `saveDirectory`

仅sqlite支持，指定保存的sqlite文件路径，默认情况下sqlite保存到 [`QStandardPaths::AppDataLocation`](https://doc.qt.io/qt-5/qstandardpaths.html#StandardLocation-enum){:target="_blank"} 目录中。

各个数据库连接配置示例
-------------

- sqlite

```cpp
#pragma once

#include <qobject.h>
#include <qdir.h>

class SqliteConfig : public QObject {
    Q_OBJECT

    Q_PROPERTY(int version READ getVersion)
    Q_PROPERTY(QString type READ getSqlType)
    Q_PROPERTY(QString driver READ getDriver)
    Q_PROPERTY(QString dbName READ getDatabaseName)
    Q_PROPERTY(QString dbPcc READ getDbPcc)
    Q_PROPERTY(QString dbOption READ getOptions)
    Q_PROPERTY(QString saveDirectory READ getSaveDirectory)

public:
    int getVersion() {
        return ver;
    }

    QString getSqlType() {
        return "sqlite";
    }

    QString getDriver() {
        return "SQLITECIPHER";
    }

    QString getDatabaseName() {
        return "mysqlite";
    }

    QString getDbPcc() {
        return "H*u1RhdJW&tbmHCU*FPZ#58KcL1q@ZXP";
    }

    QString getOptions() {
        return "QSQLITE_BUSY_TIMEOUT=100";
    }

    QString getSaveDirectory() {
        return QDir::currentPath();
    }

    int ver = 1;
};
```

- mysql

```cpp
#pragma once

#include <qobject.h>

class MysqlConfig : public QObject {
    Q_OBJECT

    Q_PROPERTY(int version READ getVersion())
    Q_PROPERTY(QString type READ getSqlType())
    Q_PROPERTY(QString dbName READ getDatabaseName())
    Q_PROPERTY(QString dbHost READ getHost())
    Q_PROPERTY(QString dbUName READ getUserName())
    Q_PROPERTY(QString dbPcc READ getPassword())
    Q_PROPERTY(int dbPort READ getPort())
    Q_PROPERTY(QString dbOption READ getOption())

public:
    int getVersion() {
        return ver;
    }

    QString getSqlType() {
        return "mysql";
    }

    QString getDatabaseName() {
        return "mysqltest";
    }

    QString getHost() {
        return "192.168.1.20";
    }

    QString getUserName() {
        return "root";
    }

    QString getPassword() {
        return "root";
    }

    int getPort() {
        return 3306;
    }

    QString getOption() {
        return "MYSQL_OPT_CONNECT_TIMEOUT=3;MYSQL_OPT_READ_TIMEOUT=3;MYSQL_OPT_WRITE_TIMEOUT=3";
    }

    int ver = 1;
};
```

- sqlserver

```cpp
#pragma once

#include <qobject.h>

class SqlServerConfig : public QObject {
    Q_OBJECT

    Q_PROPERTY(int version READ getVersion)
    Q_PROPERTY(QString type READ getSqlType)
    Q_PROPERTY(QString dbName READ getDatabaseName)
    Q_PROPERTY(QString dbHost READ getHost)
    Q_PROPERTY(QString dbUName READ getUserName)
    Q_PROPERTY(QString dbPcc READ getPassword)

public:
    int getVersion() {
        return ver;
    }

    QString getSqlType() {
        return "sqlserver";
    }

    QString getDatabaseName() {
        return "sqlservertest";
    }

    QString getHost() {
        return "192.168.1.20\\sqlexpress";
    }

    QString getUserName() {
        return "root";
    }

    QString getPassword() {
        return "root";
    }

    int ver = 1;
};
```

异常处理
-------------

创建统一异常处理类，处理在数据库执行时遇到的任何错误。继承 `DbExceptionHandler` 类，重载4类异常：初始化异常，创建连接异常，执行失败，执行警告。
```cpp
class CustomDbExceptionHandler : public DbExceptionHandler {
public:
    using DbExceptionHandler::DbExceptionHandler;

    void initDbFail(DbErrCode errcode, const QString& reason) override {}

    void databaseOpenFail(DbErrCode errcode, const QString& failReason) override {}

    void execFail(DbErrCode errcode, const QString& lastErr) {}

    void execWarning(const QString& info) override {}
};
```
在异常回调中，会得到一个`DbErrCode`类型枚举值表明在哪种情况下出现的异常，`reason`字符串通常返回数据库驱动输出的错误。

初始化连接
-------------

创建数据库连接配置类和异常处理回调后，在必要的位置初始化连接，如下在主函数开始初始化数据库：

```cpp
#include <QApplication>
#include <qdebug.h>

#include "dao.h"

void SqlLogPrinter(const QString& sql, const QVariantList& values) {
#ifdef QT_DEBUG
    qDebug() << "sql:" << sql << " values:" << values;
#endif
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    daoSetQueryLogPrinter(SqlLogPrinter);
    DbLoader::init(SqliteConfig(), new CustomDbExceptionHandler);
    
    //do something ...

    a.exec();
}
```

其中，使用 `daoSetQueryLogPrinter` 可以注册数据库执行语句的打印回调函数。