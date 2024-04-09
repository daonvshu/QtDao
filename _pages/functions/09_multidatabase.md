---
title: 多数据库连接
category: functions
layout: post
---

添加配置
-------------

在编写xml配置文件时，使用`alias`属性为配置绑定一个别名用于区分不同数据库的连接：

```xml
<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE dao SYSTEM "../../../../entity/sqlite_entity.xsd">
<dao prefix="ts_" db="sqlite" namespace="SqlV1" alias="sql1">
    <tb name="Artist" declaremetatype="true">
        <item name="id" type="long" constraints="primary key" />
        <item name="name" type="text" />
        <item name="size" type="int" />
        <index type="unique index">
            <field>id</field>
            <field>size</field>
        </index>
    </tb>
</dao>
```

```xml
<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE dao SYSTEM "../../../../entity/sqlite_entity.xsd">
<dao prefix="ts_" db="sqlite" namespace="SqlV2" alias="sql2">
    <tb name="Track">
        <item name="id" type="long" default="-1" constraints="primary key" />
        <item name="name" type="text" />
    </tb>
</dao>
```

```xml
<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE dao SYSTEM "../../../entity/mysql_entity.xsd">
<dao db="mysql" namespace="Mys" alias="mys">
    <tb name="Artist" declaremetatype="true" engine="InnoDB">
        <item name="id" type="bigint" constraints="primary key" />
        <item name="name" type="text" />
        <item name="size" type="int" />
        <index type="unique index">
            <field>id</field>
            <field>size</field>
        </index>
    </tb>
</dao>
```

在初始化连接时，使用`configAlias`函数为数据库指定使用的配置名称，并且使用`session`函数为连接绑定查询id：

```cpp
dao::_config<dao::ConfigSqliteBuilder>()
        .version(1)
        .databaseName("multi_sqlitev1test")
        .saveDir(QDir::currentPath())
        .configAlias("sql1")
        .session(SESSION_SQLITE1)
        .initializeDatabase();

dao::_config<dao::ConfigSqliteBuilder>()
        .version(1)
        .databaseName("multi_sqlitev2test")
        .saveDir(QDir::currentPath())
        .configAlias("sql2")
        .session(SESSION_SQLITE2)
        .initializeDatabase();

dao::_config<dao::ConfigMysqlBuilder>()
        .version(1)
        .databaseName("multi_mysqltest")
        .host("localhost")
        .port(3306)
        .user("root")
        .password("root")
        .configAlias("mys")
        .session(SESSION_MYSQL)
        .initializeDatabase();
```

在进行数据库操作时，传入`sessionid`来选择操作的数据库：

```cpp
using namespace SqlV1;
using namespace SqlV2;

Artist artistData1(1, "The Beatles", 4);
Artist artistData2(2, "Led Zeppelin", 5);
dao::_insert<Artist>(SESSION_SQLITE1).build().insert({ artistData1, artistData2 });

Track trackData1(2, "Stairway to Heaven");
Track trackData2(3, "Another Brick in the Wall");
dao::_insert<Track>(SESSION_SQLITE2).build().insert({ trackData1, trackData2 });
```

当执行查询时可以不使用`sessionid`，这个时候`QtDao`会查找默认的数据库配置。对应的，使用`dao::_config`初始化数据库时不能调用`session`指定id，这个时候所有的查询就像与单个数据库连接一样的表现。

> 注意：sessionid用于判断未指定使用的是值`-1`，因此在初始化和查询时，不能传入值为`-1`的`sessionid`。


基于作用域的操作
-------------

当我们需要对某个数据库执行一系列操作时，对每个查询指定`sessionid`将会导致代码变得繁杂，使用`_beginSession`和`_endSession`，为一系列操作指定数据库：

```cpp
dao::_beginSession(SESSION_SQLITE1);
auto artistQuery = dao::_select<Artist>().filter(af.id == 1).build().unique();
dao::_endSession();

dao::_beginSession(SESSION_SQLITE2);
auto trackQuery = dao::_select<Track>().filter(tf.id == 2).build().unique();
dao::_endSession();
```

也可以使用宏`DAO_SCOPED_SESSION_BEGIN`，指定在当前作用域下的操作数据库：

```cpp
{
    DAO_SCOPED_SESSION_BEGIN(SESSION_SQLITE1);
    artistQuery1 = dao::_select<SqlV1::Artist>().filter(af.id == 2).build().unique();
    {
        DAO_SCOPED_SESSION_BEGIN(SESSION_MYSQL);
        artistQuery2 = dao::_select<Mys::Artist>().filter(af.id == 3).build().unique();
    }
    artistQuery1 = dao::_select<SqlV1::Artist>().filter(af.id == 1).build().unique();
}
```

作用域与多线程
-------------

`QtDao`中的每个查询是线程安全的，直接在查询时指定`sessionid`是安全的。同样的，在使用基于作用域的函数时也是线程安全的，每个线程相互不影响：

```cpp
QThread::create([&] {
    dao::_beginSession(SESSION_SQLITE1);

    SqlV1::Artist::Fields af;
    auto artistQuery1 = dao::_select<SqlV1::Artist>().filter(af.id == 2).build().unique();

    dao::_endSession();
})->start();

QThread::create([&] {
    DAO_SCOPED_SESSION_BEGIN(SESSION_MYSQL);

    Mys::Artist::Fields af;
    auto artistQuery2 = dao::_select<Mys::Artist>().filter(af.id == 3).build().unique();
})->start();
```

