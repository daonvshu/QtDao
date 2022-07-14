---
title: 更新数据
category: functions
layout: post
---

QtDao更新数据可以按条件更新，也可以按实例更新。使用函数`_update<E>`返回一个`UpdateBuilder<E>`对象，再调用`build()`函数创建`Update<E>`对象进行更新数据操作：

```cpp
template<typename E>
Insert<E> dao::_update<E>().build()
```

条件更新
-------------

按条件更新记录时，对`UpdateBuilder<E>`使用`set`函数设置需要更新的字段，使用`filter`函数设置更新条件。使用如下：

```cpp
User::Fields field;
bool success = dao::_update<User>()
    .set(field.age = 19, field.score = 120)
    .filter(field.name == "Alice")
    .build().update();
```

等同于使用如下sql语句：

```sql
update User set age = 19, score = 120 where name = 'Alice'
```

实例更新
-------------

按实例更新记录时，不能使用`set`与`filter`函数，直接将实例传入`update`函数即可：

```cpp
User::Fields field;
auto user = dao::_select<User>().filter(field.name == "Alice").build().unique();

user.setAge(19);
user.setScore(120);
bool success = dao::_update<User>().build().update(user);
```

按实例更新采用的方法和条件更新一致，不同的是，按实例更新时内部将自动提取表`primary key`字段作为更新条件，其他字段作为更新的字段，因此，使用实例更新方法时，必须确保操作的表设置了`primary key`字段。

批量更新
-------------

使用`updateBatch()`函数进行批量更新。对于使用条件更新，对`field`成员使用`QList<T>`列表即可，但需要确保`set`条件和`filter`条件值列表大小一致：

```cpp

auto userNames = QStringList() << "Alice" << "Bob";
auto ages = QList<int>() << 19 << 20;
auto scores = QList<int>() << 120 << 130;

User::Fields field;
bool success = dao::_update<User>()
    .set(field.age = ages, field.score = scores)
    .filter(field.name == userNames)
    .build().updateBatch();
```

对于使用实例更新，传入对象列表即可：

```cpp
UserList users;
users << User("Alice", 18, 100);
users << User("Bob", 19, 120);
dao::_update<User>().build().updateBatch(users);
```