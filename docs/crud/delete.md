---
sidebar_position: 4
---

# 删除

QtDao执行`delete`操作，使用函数`_delete<E>`返回一个`DeleteBuilder<E>`对象，再调用`build()`函数创建`Delete<E>`对象进行删除数据操作，可以按条件，也可以按表对象删除：

```cpp
template<typename E>
Delete<E> dao::_delete<E>().build()
```

## 1. 按条件删除

按条件删除记录时，使用`filter`函数设置条件。基本形式：

```cpp
dao::_delete<MyTable>()
    .filter(...)
    .build()
    .deleteBy(); //.deleteBatch()
```

### 1.1 按条件删除

使用`filter`函数设置匹配条件，删除时的`filter`与`select`中的条件构造使用方法相同，在逻辑上能够匹配数据就能执行删除操作。使用如下：

```cpp
User::Fields uf;
dao::_delete<User>()
    .filter(uf.name == "Alice")
    .build().deleteBy();
```

等同于使用如下sql语句：

```sql
delete from user where name = 'Alice'
```

### 1.2 批量条件删除

使用`deleteBatch()`函数进行批量删除。`filter`条件中传入列表即可：

```cpp
auto userNames = QStringList() << "Alice" << "Bob";

User::Fields uf;
dao::_delete<User>()
    .filter(uf.name == userNames)
    .build().deleteBatch();
```

## 2. 按对象删除

使用`deleteBy()`函数，传入对象即可删除该条数据，这时不能使用`filter`函数设置条件，基本形式：

```cpp
dao::_delete<MyTable>()
    .build()
    .deleteBy(...); //.deleteBatch(...)
```

### 2.1 删除一个对象

与`update`操作一样，删除对象时QtDao自动提取表对象`primary key`字段来构造删除条件，使用如下：

```cpp
User::Fields uf;
auto user = dao::_select<User>()
                .filter(uf.name == "Alice")
                .build().unique();

dao::_delete<User>().build().deleteBy(user);
```

### 2.2 删除多个对象

使用`deleteBatch()`函数批量删除对象，出入对象列表即可，使用如下：

```cpp
User::Fields uf;
UserList users = dao::_select<User>()
                    .filter(uf.age < 18)
                    .build().list();

dao::_delete<User>().build().deleteBatch(users);
```

## 3. 清空数据

通常情况下，不设置`filter`条件就能实现对表的清空：

```cpp
dao::_delete<User>().build().deleteBy();
```

但使用该方法不能重置自增长字段，使用`_truncate()`函数可以清空表数据并重置自增字段：

```cpp
dao::_truncate<User>();
```