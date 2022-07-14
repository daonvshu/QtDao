---
title: 删除数据
category: functions
layout: post
---

QtDao删除数据可以按条件删除，也可以按实体对象删除。使用函数`_delete<E>`返回一个`DeleteBuilder<E>`对象，再调用`build()`函数创建`Delete<E>`对象进行删除数据操作：

```cpp
template<typename E>
Delete<E> dao::_delete<E>().build()
```

条件删除
-------------

按条件删除记录时，对`DeleteBuilder<E>`使用`filter`函数设置条件。使用如下：

```cpp
User::Fields field;
bool success = dao::_delete<User>()
    .filter(field.name == "Alice")
    .build().deleteBy();
```

等同于使用如下sql语句：

```sql
delete from User where name = 'Alice'
```

实体对象删除
-------------

按实体对象删除记录时，直接将对象传入`update`函数即可：

```cpp
User::Fields field;
auto user = dao::_select<User>().filter(field.name == "Alice").build().unique();

bool success = dao::_delete<User>().build().deleteBy(user);
```

> 注意：按实体对象删除时内部将自动提取表`primary key`字段作为条件，因此，使用实体对象删除方法时，必须确保操作的表设置了`primary key`字段。

批量删除
-------------

使用`deleteBatch()`函数进行批量删除。对于使用条件删除，对`field`成员使用`QList<T>`列表即可：

```cpp
auto userNames = QStringList() << "Alice" << "Bob";

User::Fields field;
bool success = dao::_delete<User>()
    .filter(field.name == userNames)
    .build().deleteBatch();
```

对于使用实体对象删除，传入对象列表即可：

```cpp
User::Fields field;
UserList users = dao::_select<User>().filter(field.age < 18).build().list();

dao::_delete<User>().build().deleteBatch(users);
```

清空数据
-------------

通常情况下，不设置`filter`条件就能实现对表的清空：

```cpp
dao::_delete<User>().build().deleteBy();
```

但使用该方法不能重置自增长字段，使用`_truncate()`函数可以清空表数据并重置自增字段：

```cpp
dao::_truncate<User>();
```