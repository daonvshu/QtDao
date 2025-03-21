---
sidebar_position: 2
---

# 插入

QtDao执行`insert`操作，使用函数`_insert<E>`返回一个`InsertBuilder<E>`对象，再调用`build()`函数创建`Insert<E>`对象进行插入数据操作，可以按字段插入，也可以插入表对象：

```cpp
template<typename E>
Insert<E> dao::_insert<E>().build()
```

## 1. 按字段插入

插入一条新数据，使用`set`函数指定插入列，对应其他字段将使用默认值（如果配置）或`null`。基本形式：

```cpp
dao::_insert<MyTable>()
    .set(...)
    .build()
    .insert();
```

### 1.1 插入一条数据

插入一条记录时，可以对指定的部分字段设置值，对`InsertBuilder<E>`使用`set`函数显式设置对应字段的值。使用如下：

```cpp
User::Fields uf;
dao::_insert<User>()
    .set(uf.name = "Alice", uf.age = 18)
    .build().insert();
```

使用该方法插入数据等同于使用以下sql语句：

```sql
insert into user (name, age) values ('Alice', 18)
```

:::warning
通常情况下，对于限制为`not null`的字段，必须显式设置其值，否则会抛出异常。
:::

### 1.2 插入多条数据

继承`EntityField<T>`的字段成员，除了与单一值构造条件，也可以使用`QList<T>`值列表构造条件，这种情况下，对`set`函数使用可以进行批量插入：

```cpp
auto names = QStringList() << "Alice" << "Bob";
auto ages = QList<int>() << 18 << 19;

User::Fields uf;
dao::_insert<User>()
    .set(uf.name = names, uf.age = ages)
    .build().insert();
```

## 2. 按对象插入

`insert`函数具有传入对象的重载函数，可以插入一个或多个对象，此时不应该使用`set`函数，否则会报语法错误。通过对象插入的基本形式：

```cpp
dao::_insert<MyTable>()
    .build()
    .insert(...)
    //.insert2(...)
    ;
```

### 2.1 插入一个对象

使用`insert(E& entity)`版本的函数插入一个对象，插入时QtDao内部会将所有字段按照语法`insert into table (...) values (...)`进行拼接，使用如下：

```cpp
User alice("Alice", 18, 100);
dao::_insert<User>().build().insert(alice);
```

:::note
对于具有自增字段的对象，插入时会将忽略对象的值，因此在插入时不需要设置自增长列的值。如果自增列只有一个，那么插入成功后还会设置回插入对象中。
:::

### 2.2 插入多个对象

使用`QList<E>`列表可以插入多个对象：

```cpp
UserList users;
users << User("Alice", 18, 100);
users << User("Bob", 19, 120);
dao::_insert<User>().build().insert(users);
```

### 2.3 快速插入多个对象

使用`insert2()`函数快速批量插入对象列表，对于大量数据的插入，该方法的执行效率更高。

```cpp
UserList users;
users << User("Alice", 18, 100);
users << User("Bob", 19, 120);
dao::_insert<User>().build().insert2(users);
```

与`insert()`不同的地方是，`insert2()`不会使用批处理执行，而是使用以下类似语句：

```sql
insert into user (name, age, score) values ('Alice', 18, 100),('Bob', 19, 120)
```

:::warning
使用快速插入方法时，需要注意当前使用的数据库SQL语句长度的限制。
:::

## 3. 插入或替换

使用`insertOrReplace()`函数执行‘插入或更新’操作，通常情况下数据表结构中需要有`unique`限制的字段或`unique index`索引配合使用。插入值时，对于`unique`字段已经存在表中，则更新其他非`unique`字段的值。对于自增字段的影响，需要注意的是不同数据库对自增字段的处理方式可能不同，在QtDao内部，`sqlite`使用的是`insert or replace into ...`语法，而`mysql`使用的是`replace into ...`语法。

:::note
该方法仅`sqlite`和`mysql`支持，其他数据库不支持。
:::

```cpp
User::Fields uf;
dao::_insert<User>()
    .set(uf.name = "Alice", uf.age = 18)
    .build().insertOrReplace();

User alice("Alice", 18, 100);
dao::_insert<User>().build().insertOrReplace(alice);

UserList users;
users << User("Alice", 18, 100);
users << User("Bob", 19, 120);
dao::_insert<User>().build().insertOrReplace(users);
```

## 4. 插入或更新

“插入或更新”在功能上与“插入或替换”是相同的，而该方法对于所有支持的数据库都可以使用。当插入一条（或批量插入）数据时，数据库引擎检查到字段冲突时使用`update`方式更新数据，否则使用`insert`方式添加数据。通常情况下，字段冲突使用`primary key/unique/unique index`约束检查。该操作与`insert or replace`不同的是，字段冲突时`insert or replace`会删除原有数据再插入新数据。基本形式：

```cpp
dao::_insertOrUpdate<MyTable>()
    .set(...)
    .conflictColumns(...)
    .updateColumns(...)
    .filter(...)
    .build()
    .insert(...)
```

QtDao内部对于不同数据库使用语法有所不同：

|数据库类型|语法结构|
|:--|:--|
|sqlite/psql|insert into ... values(...) on conflict(...) do update set ...|
|mysql|insert into ... values(...) on duplicate key update ...|
|sqlserver|merge into ... when matched then update set ... when not matched then insert ... values(...)|

- conflictColumns  
设置冲突检查的字段，仅传入字段即可。通常情况下，传入`primary key/unique/unique index`对应的字段（对象插入模式下，将忽略自增主键）。

- updateColumns  
设置遇到冲突时需要更新的字段。显式调用该函数指定冲突时更新的字段，未使用该函数时则使用冲突字段之外的其他所有字段。

### 4.1 按字段插入  
使用方法与`insert`操作一致，调用`set`函数传入部分字段进行插入：

```cpp
User::Fields uf;
//假设 name,classes 作为一个unique index
dao::_insertOrUpdate<User>()
    .set(uf.name = "Alice", uf.classes = "classA", uf.age = 18, uf.score = 90)
    .conflictColumns(uf.name, uf.classes)
    .updateColumns(uf.score)
    .build().insert();

//省略 updateColumns 则同时更新 age,score
dao::_insertOrUpdate<User>()
    .set(uf.name = "Alice", uf.classes = "classA", uf.age = 18, uf.score = 90)
    .conflictColumns(uf.name, uf.classes)
    .build().insert();

//与 insert 操作一样，可批量插入
auto names = QStringList() << "Alice" << "Bob";
auto classes = QStringList() << "classA" << "classA";
auto ages = QList<int>() << 18 << 19;
auto scores = QList<int>() << 90 << 91;

dao::_insertOrUpdate<User>()
    .set(uf.name = names, uf.classes = classes, uf.age = ages, uf.score = scores)
    .conflictColumns(uf.name, uf.classes)
    .build().insert();
```

### 4.2 按对象插入

与`insert`一样，`_insertOrUpdate`的`insert`函数也可以插入对象：

```cpp
User::Fields uf;
//假设 name,classes 作为一个unique index
User alice("Alice", "classA", 18, 90);
dao::_insertOrUpdate<User>()
    .conflictColumns(uf.name, uf.classes)
    .updateColumns(uf.score)
    .build().insert(alice);

//省略 updateColumns 则同时更新 age,score
dao::_insertOrUpdate<User>()
    .conflictColumns(uf.name, uf.classes)
    .build().insert(alice);

//批量插入
User bob("Bob", "classA", 19, 91);
auto users = UserList() << alice << bob;

dao::_insertOrUpdate<User>()
    .conflictColumns(uf.name, uf.classes)
    .build().insert(users);
```

### 4.3 条件更新

使用条件更新。当发生冲突时，是否执行`update`操作额外取决于`filter`条件的设置。要使用`sqlite`中的`upsert`特定表`excluded`字段，实例化`dao::UpsertExcluded<E>::Fields`即可，示例如下：

```cpp
auto names = QStringList() << "Alice" << "Bob";
auto classes = QStringList() << "classA" << "classA";
auto ages = QList<int>() << 12 << 20;
auto scores = QList<int>() << 90 << 91;

User::Fields uf;
dao::UpsertExcluded<User>::Fields excluded;

dao::_insertOrUpdate<User>()
    .set(uf.name = names, uf.classes = classes, uf.age = ages, uf.score = scores)
    .conflictColumns(uf.name, uf.classes)
    .filter(excluded.age > uf.age)
    .build().insert();
```

在上面的例子中，若`Alice`和`Bob`数据库中的`age`都为18，`Alice`不满足`filter`条件则忽略本次`Update`操作。

:::note
注意：仅支持在`sqlite/psql`中使用，其他数据库忽略该条件的设置。
:::

## 5. 插入或忽略

使用`insertOrIgnore()`函数执行‘插入或忽略’操作，与`insertOrReplace()`使用一样，在插入冲突时忽略该次插入数据。

```cpp
User::Fields uf;
dao::_insert<User>()
    .set(uf.name = "Alice", uf.age = 18)
    .build().insertOrIgnore();

User alice("Alice", 18, 100);
dao::_insert<User>().build().insertOrIgnore(alice);

UserList users;
users << User("Alice", 18, 100);
users << User("Bob", 19, 120);
dao::_insert<User>().build().insertOrIgnore(users);
```

## 6. insertIntoSelect

使用`insert into select`从一个表中的数据插入到另一个表中，该过程需要注意字段一致。使用`dao::_insertIntoSelect`可以执行`insert into select`语句，这时需要使用嵌套子查询实现：

```cpp
User1::Fields uf1;
User2::Fields uf2;
dao::_insertIntoSelect<User2>()
    .column(uf2.name, uf2.age)
    .from(
        dao::_select<User1>()
            .column(uf1.name, uf1.age)
            .filter(uf1.score < 12)
            .build()
    )
    .build().insert();
```