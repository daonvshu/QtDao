---
title: 其他一些功能
category: functions
layout: post
---

异常捕捉
-------------

`QtDao`中所有的增删改查操作遇到错误时都会抛出`DaoException`异常，此时通过`try-catch`可以捕捉到。

```cpp
User::Fields field;

try {
    auto user = dao::_select<User>()
        .filter(field.name == "Alice")
        .build().unique();
} catch (DaoException& e) {
    qDebug() << e.reason << e.sql << e.values;
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

自定义数据库升级
-------------

默认情况下，数据库版本变更时会执行以下升级策略：  

1. 开启事务
2. 创建以原表名一样的 `tem_<原表名>` 临时表
3. 复制旧表的数据到临时表
4. 删除旧表
5. 重名名临时表为原表名
6. 为原表创建索引
7. 提交事务

继承 `dao::DatabaseUpgrader` 类可以自定义升级过程，如下所示：
```cpp
//mydbupgrader.h
#include <qobject.h>

#include "dao.h"

class MyDbUpgrader : public dao::DatabaseUpgrader {
public:
    void onUpgrade(int oldVersion, int curVersion) override;

private:
    void upgrade1To2();
    void upgrade2To3();
};

//mydbupgrader.cpp
#include "mydbupgrader.h"

#include "table1.h"

void MyDbUpgrader::onUpgrade(int oldVersion, int curVersion) {
    if (entityReader->isTable<Table1>()) {
        return;
    }
    if (oldVersion == 1 && curVersion == 2) {
        upgrade1To2();
    } else if (oldVersion == 2 && curVersion == 3) {
        upgrade2To3();
    } else {
        upgradeWithDataRecovery();
    }
}

void MyDbUpgrader::upgrade1To2() {
    if (config->isSqlite()) {
        upgradeWithDataRecovery();
        return;
    }
    dao::transcation();
    try {
        //drop index
        client->dropIndex(entityReader->getTableName(), "index_number2");
        client->dropIndex(entityReader->getTableName(), "index_number_number2");
        //remove unique index
        client->dropIndex(entityReader->getTableName(), "index_name_number");
        if (TEST_DB == QLatin1String("sqlserver")) {
            client->createIndex(entityReader->getTableName(), entityReader->getNonClusteredIndexFields()[0], dao::IndexType::INDEX_NORMAL);
        } else {
            client->createIndex(entityReader->getTableName(), entityReader->getIndexFields()[0], dao::IndexType::INDEX_NORMAL);
        }
        //drop column
        client->dropField(entityReader->getTableName(), "number2");
        //add new column 'age'
        client->addField(entityReader->getTableName(), entityReader->getFieldsType()[4]);
        dao::commit();
    } catch (dao::DaoException& e) {
        Q_UNUSED(e)
        dao::rollback();
        qFatal("database upgrade fail!");
    }
}

void MyDbUpgrader::upgrade2To3() {
    if (config->isSqlite()) {
        upgradeWithDataRecovery();
        return;
    }
    dao::transcation();
    try {
        //drop index
        client->dropIndex(entityReader->getTableName(), "index_name_number");
        //drop column
        client->dropField(entityReader->getTableName(), "name");
        //add new column 'name'
        client->addField(entityReader->getTableName(), entityReader->getFieldsType()[2]);
        //rename 'number' to 'score'
        client->renameField(entityReader->getTableName(), "number", "score");
        dao::commit();
    } catch (dao::DaoException& e) {
        Q_UNUSED(e)
        dao::rollback();
        qFatal("database upgrade fail!");
    }
}

```

在上述例子中，`client`代表当前配置的数据库操作接口，`entityReader`代表当前表的读取接口，需要注意的是，使用列和索引相关函数操作数据表的列和索引时，不同数据库表现可能不一致，sqlite对列进行操作时对版本有要求，在自定义升级步骤时注意备份数据并完成测试是个必要的步骤。使用时在初始化时传入自定义升级类，例如sqlite初始化：

```cpp
dao::_config<dao::ConfigSqliteBuilder>()
    .version(1)
    .databaseName("mydb")
    .setDatabaseUpgrader(new MyDbUpgrader)
    .initializeDatabase();
```