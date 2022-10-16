---
title: 代码初始化
category: config
layout: post
---

初始化连接
-------------
`QtDao`连接初始化数据库时，可配置的属性如下表：

|属性|sqlite|mysql|sqlserver|
|:--:|:--:|:--:|:--:|
|`version`|✔️|✔️|✔️|
|`driver`|✔️|✔️|✔️|
|`databaseName`|✔️|✔️|✔️|
|`host`||✔️|✔️|
|`user`||✔️|✔️|
|`password`|✔️|✔️|✔️|
|`port`||✔️|✔️|
|`options`|✔️|✔️|✔️|
|`saveDir`|✔️|||

- `version`

设置当前数据库版本号，若开启自动升级功能（默认），QtDao初始化时检测到版本号发生变化会自动执行数据表结构的升级，同时把旧的数据尝试插入到新的表结构中。数据库版本号应该在配置表结构发生变化时变更，变更后下次程序启动时自动执行升级过程。

- `driver`

选择数据库驱动。默认情况下根据`type`自动设置，`sqlite` -> `QSQLITE`，`mysql` -> `QMYSQL`，`sqlserver` -> `QODBC`，当使用第三方驱动时可显式配置使用的驱动名称（如使用[QtCipherSqlitePlugin](https://github.com/devbean/QtCipherSqlitePlugin){:target="_blank"}时配置driver为 `SQLITECIPHER`）。

- `databaseName`

设置连接的数据库名称，对于sqlite为数据库文件名。

- `host`

设置连接数据库的地址

- `user`

设置连接时登录的用户名

- `password`
  
设置连接时登录使用的密码

- `port`

设置连接数据库的端口

- `options`

设置连接参数，连接可用的参数可参考官方文档：[https://doc.qt.io/qt-5/qsqldatabase.html#setConnectOptions](https://doc.qt.io/qt-5/qsqldatabase.html#setConnectOptions){:target="_blank"}

- `saveDir`

仅sqlite支持，指定保存的sqlite文件路径，默认情况下sqlite保存到 [`QStandardPaths::AppDataLocation`](https://doc.qt.io/qt-5/qstandardpaths.html#StandardLocation-enum){:target="_blank"} 目录中。

各个数据库初始化连接示例
-------------

- sqlite

```cpp
try {
    dao::_config<dao::ConfigSqliteBuilder>()
        .version(1)
        .databaseName("sqlitetest")
        //.driver("SQLITECIPHER")
        //.password("H*u1RhdJW&tbmHCU*FPZ#58KcL1q@ZXP")
        .options("QSQLITE_BUSY_TIMEOUT=100")
        .saveDir(QDir::currentPath())
        .initializeDatabase();
} catch (dao::DaoException& e) {
    Q_UNUSED(e)
    qFatal("setup database fail!");
}
```

- mysql

```cpp
try {
    dao::_config<dao::ConfigMysqlBuilder>()
        .version(1)
        .databaseName("mysqltest")
        .host("localhost")
        .port(3306)
        .user("root")
        .password("root")
        .options("MYSQL_OPT_CONNECT_TIMEOUT=3;MYSQL_OPT_READ_TIMEOUT=3;MYSQL_OPT_WRITE_TIMEOUT=3")
        .initializeDatabase();
} catch (dao::DaoException& e) {
    Q_UNUSED(e)
    qFatal("setup database fail!");
}
```

- sqlserver

```cpp
try {
    dao::_config<dao::ConfigSqlServerBuilder>()
        .version(version)
        .databaseName("sqlservertest")
        .host("localhost")
        .user("sa")
        .password("root")
        .initializeDatabase();
} catch (dao::DaoException& e) {
    Q_UNUSED(e)
    qFatal("setup database fail!");
}
```

初始化连接
-------------

在必要的位置初始化连接，如下在主函数开始初始化数据库：

```cpp
#include <QApplication>
#include <qdebug.h>

#include "dao.h"

QTDAO_USING_NAMESPACE

void SqlLogPrinter(const QString& sql, const QVariantList& values) {
#ifdef QT_DEBUG
    qDebug() << "sql:" << sql << " values:" << values;
#endif
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    daoSetQueryLogPrinter(SqlLogPrinter);
    try {
        dao::_config<dao::ConfigSqliteBuilder>()
            .version(1)
            .databaseName("sqlitetest")
            .initializeDatabase();
    } catch (dao::DaoException& e) {
        Q_UNUSED(e)
        qFatal("setup database fail!");
    }
    
    //do something ...

    a.exec();
}
```

其中，使用 `daoSetQueryLogPrinter` 可以注册数据库执行语句的打印回调函数。