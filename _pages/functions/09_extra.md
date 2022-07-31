---
title: 其他一些功能
category: functions
layout: post
---

异常捕捉
-------------

`QtDao`中所有的增删改查操作都提供了`throwable()`函数。调用该函数后，当前的数据库操作失败会抛出异常，此时通过`try-catch`可以捕捉到，未使用该函数将直接回调到[统一的异常处理回调]({{site.baseurl}}/pages/config/03_init/#异常处理)中。

```cpp
User::Fields field;

try {
    auto user = dao::_select<User>()
        .filter(field.name == "Alice")
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
    dao::_insert<User>().build().insert(alice);
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

