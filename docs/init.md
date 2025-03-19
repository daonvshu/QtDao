---
sidebar_position: 4
---

# 初始化

## 连接参数

数据库初始化使用`dao::_config`函数，可配置的属性如下表：

|属性|sqlite|mysql|sqlserver|postgresql|可选|
|:--:|:--:|:--:|:--:|:--:|:--:|
|[version](#version)|✔️|✔️|✔️|✔️||
|[driver](#driver)|✔️|✔️|✔️|✔️|✔️|
|[databaseName](#databasename)|✔️|✔️|✔️|✔️||
|[alias](#alias)|✔️|✔️|✔️|✔️|✔️|
|[session](#session)|✔️|✔️|✔️|✔️|✔️|
|[host](#host)||✔️|✔️|✔️|✔️|
|[port](#port)||✔️|✔️|✔️|✔️|
|[user](#user)||✔️|✔️|✔️|✔️|
|[owner](#owner)||||✔️|✔️|
|[password](#password)|✔️|✔️|✔️|✔️|✔️|
|[options](#options)|✔️|✔️|✔️|✔️|✔️|
|[encoding](#encoding)||||✔️|✔️|
|[character](#character)||✔️|||✔️|
|[collate](#collate)||✔️||✔️|✔️|
|[ctype](#ctype)||||✔️|✔️|
|[tableSpace](#tableSpace)||||✔️|✔️|
|[saveDir](#savedir)|✔️||||✔️|

### version

设置当前数据库版本号，若开启自动升级功能（默认），QtDao初始化时检测到版本号发生变化会自动执行数据表结构的升级，同时把旧的数据尝试插入到新的表结构中。数据库版本号应该在配置表结构发生变化时变更，变更后下次程序启动时自动执行升级过程。

### driver

选择数据库驱动。默认情况下根据`type`自动设置，`sqlite` -> `QSQLITE`，`mysql` -> `QMYSQL`，`sqlserver` -> `QODBC`，`psql` -> `QPSQL`，当使用第三方驱动时可显式配置使用的驱动名称（如使用[QtCipherSqlitePlugin](https://github.com/devbean/QtCipherSqlitePlugin)时配置driver为 `SQLITECIPHER`）。

### databaseName

设置连接的数据库名称，对于sqlite为数据库文件名。

### alias

多数据库操作时，对应的配置表。同一种或者不同类型的数据库存在多个时，为了在自动创建数据库和数据表时，来区分使用哪个配置进行初始化。config传递的类型必须要和配置表一致，该属性的名称和想要初始化的配置表里面的[`alias`](/QtDao/docs/config_xml#dao标签)字段名称也要一致。

### session

一个`qint64`类型的值，用于选择使用哪个数据库来进行操作。

### host

设置连接数据库的地址，默认值为`localhost`。

### user

设置连接时登录的用户名，默认值为`root(mysql)`、`sa(sqlserver)`、`postgres(psql)`。

### owner

设置PSql的所有者。

### password
  
设置连接时登录使用的密码。

### port

设置连接数据库的端口，默认值为`3306(mysql)`、`1433(sqlserver)`、`5432(psql)`。

### options

设置连接参数，连接可用的参数可参考官方文档：[https://doc.qt.io/qt-5/qsqldatabase.html#setConnectOptions](https://doc.qt.io/qt-5/qsqldatabase.html#setConnectOptions)

### encoding

设置psql数据库的默认编码，默认值为`UTF8`。

### character

设置mysql数据库的默认字符集，默认值为`utf8mb4`。

### collate

设置数据库的默认字符排序规则，默认值为`utf8mb4_general_ci(mysql)`。

### ctype

设置PSql的字符类型。

### tableSpace

设置PSql的表空间。

### saveDir

仅sqlite支持，指定保存的sqlite文件路径，默认情况下sqlite保存到 [`QStandardPaths::AppDataLocation`](https://doc.qt.io/qt-5/qstandardpaths.html#StandardLocation-enum)目录中。

## 各个数据库连接示例

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

- psql

```cpp
try {
    dao::_config<dao::ConfigPSqlBuilder>()
        .version(version)
        .databaseName("psqltest")
        .host("localhost")
        .port("5432")
        .user("postgres")
        .password("root")
        .initializeDatabase();
} catch (dao::DaoException& e) {
    Q_UNUSED(e)
    qFatal("setup database fail!");
}
```

## 禁用自动创建

如果要禁用数据库和数据表自动创建，使用`disableCreateDatabase`和`disableCreateTables`函数。

```cpp
try {
    dao::_config<dao::ConfigSqliteBuilder>()
        .databaseName("sqlitetest")
        .saveDir(QDir::currentPath())
        .disableCreateDatabase() //禁止创建数据库
        .disableCreateTables() //禁止创建数据表
        .initializeDatabase();
} catch (dao::DaoException& e) {
    Q_UNUSED(e)
    qFatal("setup database fail!");
}
```

## 基本使用

`QtDao`不需要手动建立连接，在初始化之后，进行数据库查询时动态创建连接，并且每个连接与当前线程相关的。根据Qt文档数据库模块与线程相关所述，不同线程的数据库连接是不能够共用的，因此，在QtDao建立查询时，当前线程与数据库的连接名是绑定在一起的，同一个线程共用一个连接，否则创建新的连接。如下所示：
```cpp
class MyWorkThread : public QThread {
protected:
    void run() override {
        //...
        User::Field uf;
        auto users = dao::_select<User>().filter(uf.age >= 18).build().list();
    }
}
```

## 日志输出

`QtDao`内部使用`QLoggingCategory`打印日志，如果要查看某一个查询执行的sql语句，在执行`build`之前调用`logging`函数传入`category`即可：

```cpp

Q_LOGGING_CATEGORY(userCategory, "query.user")

UserList listUsers() {
    User::Fields field;
    return dao::_select<User>()
        .filter(field.score > 100)
        .logging(userCategory)
        .build().list();
}
```

在不需要打印指定查询时，关闭其日志即可：

```cpp
int main(int argc, char *argv[]) {
    //...
    QLoggingCategory::setFilterRules("query.user.debug=false");
    //...
}
```

使用函数`dao::loggingUseDefault()`启用默认日志输出，此时未使用logging的查询将使用默认的`category`：
```cpp
dao::loggingUseDefault();
//关闭默认 category
QLoggingCategory::setFilterRules("qtdao.query.debug=false");
```

打印时，将看到如下信息：

```log
>>>--------------------------------------------------------<<<
category:        qtdao.query
build the query: select * from ts_sqlitetest1 where number>? and name is not null limit ?
values:          [12 3]
translated:      select * from ts_sqlitetest1 where number>12 and name is not null limit 3
--------------------------------------------------------------
```