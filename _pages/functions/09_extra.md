---
title: 其他一些功能
category: functions
layout: post
---

异常捕捉
-------------

`QtDao`中所有的增删改查操作遇到错误时都会抛出`DaoException`异常，此时通过`try-catch`可以捕捉到。另外，在调试（Debug）模式下，遇到错误抛出异常之前，会调用`qFatal`函数以便于调试时命中错误位置。建议只有在不确定的查询时使用`try-catch`，调试模式下忽略`qFatal`调用`disableFatalMsg()`禁用。

```cpp
User::Fields field;

try {
    auto user = dao::_select<User>()
        .filter(field.name == "Alice")
        //.disableFatalMsg() //禁用qFatal调用
        .build().unique();
} catch (DaoException& e) {
    qDebug() << e.reason;
}
```

事务
-------------

`QtDao`中启用事务功能使用`dao::transcation()`函数，通常情况下与`throwable()`函数一起使用，当查询出现异常时回滚当前操作。

```cpp

dao::transcation();

try {
    User alice("Alice", 18, 100);
    dao::_insert<User>().disableFatalMsg().build().insert(alice);
    //提交
    dao::commit();
} catch (DaoException&) {
    //异常时回滚
    dao::rollback();
} 

```

explain
-------------

不同的数据库操作提供了不同的sql解释信息的输出，调用`explain<T>()`函数输出当前驱动对sql语句的解释信息。

|数据库类型|解释信息结构体|
|:--:|:--:|
|`sqlite`|`SqliteExplainInfo`|
|`mysql`|`MysqlExplainInfo`|
|`sqlserver`|`SqlServerExplainInfo`|

如下，当前数据库使用`sqlite`时的解释信息输出：

```cpp
User::Fields field;
auto queryExplain = dao::_select<User>()
        .filter(field.name == "Alice")
        .build()
        .explain<SqliteExplainInfo>();
```

