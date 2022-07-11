---
title: 添加数据
category: functions
layout: post
---

QtDao的增删改操作，可以单独操作实体，也可以使用条件函数作用。使用函数`_insert<E>`返回一个`InsertBuilder<E>`对象，再调用`build()`函数创建`Insert<E>`对象进行插入数据操作：

```cpp
template<typename E>
Insert<E> dao::_insert<E>().build()
```

部分字段数据的填充
-------------

插入一条记录时，可以对指定的部分字段设置值，对`InsertBuilder<E>`使用`set`函数显式设置对应字段的值。通常情况下，对于没有设置默认值限制为`not null`的字段，必须显式设置其值。使用如下：

```cpp
User::Fields field;
dao::_insert<User>()
    .set(field.name = "Alice", field.age = 18)
    .build().insert();
```

使用该方法插入数据等同于使用以下sql语句：

```sql
insert into user (name, age) values ('Alice', 18)
```

继承`EntityField<T>`的字段成员，除了与单一值构造条件，也可以使用`QList<T>`值列表构造条件，这种情况下，对`set`函数使用可以进行批量插入：

```cpp
auto names = QStringList() << "Alice" << "Bob";
auto ages = QList<int>() << 18 << 19;

User::Fields field;
dao::_insert<User>()
    .set(field.name = names, field.age = ages)
    .build().insert();
```

插入一个实例
-------------

插入一个模型类的实例时，直接传入`insert()`函数即可。插入实例与使用`set`函数方法一致，所有的字段和值会自动填入（自增字段将被忽略），使用如下：

> 注意：通常情况下，插入一个实例时，执行插入后实例的自增长字段将被设置回插入的实例中。

```cpp
User alice("Alice", 18, 100);
dao::_insert<User>().build().insert(alice);
```

同样，插入一个`QList<E>`列表执行批量插入：

```cpp
UserList users;
users << User("Alice", 18, 100);
users << User("Bob", 19, 120);
dao::_insert<User>().build().insert(users);
```

快速插入实例
-------------

使用`insert2()`函数快速批量插入列表。

```cpp
UserList users;
users << User("Alice", 18, 100);
users << User("Bob", 19, 120);
dao::_insert<User>().build().insert2(users);
```

与`insert()`不同的地方是，`insert2()`不会使用批处理执行，而是使用以下类似语句：

```sql
insert into user (name, age, score) values ('Alice', 18, 100),("Bob", 19, 120)
```

使用快速插入方法时，需要注意对应数据库sql语句长度的限制。

插入或替换
-------------

使用`insertOrReplace()`函数执行‘插入或更新’操作，通常情况下数据表结构中需要有`unique`限制的字段或`unique index`索引配合使用。

```cpp
User::Fields field;
dao::_insert<User>()
    .set(field.name = "Alice", field.age = 18)
    .build().insertOrReplace();

User alice("Alice", 18, 100);
dao::_insert<User>().build().insertOrReplace(alice);

UserList users;
users << User("Alice", 18, 100);
users << User("Bob", 19, 120);
dao::_insert<User>().build().insertOrReplace(users);
```

insert into select
-------------

使用`dao::_insertIntoSelect`可以执行`insert into select`语句，这时需要使用嵌套子查询实现：

```cpp
User1::Fields field1;
User2::Fields field2;
dao::_insertIntoSelect<User2>()
    .column(field2.name, field2.age)
    .from(
        dao::_select<User1>().column(field1.name, field1.age).filter(field1.score < 12).build()
    )
    .build().insert();
```