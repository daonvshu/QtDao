---
sidebar_position: 3
---

# 更新

QtDao执行`update`操作，使用函数`_update<E>`返回一个`UpdateBuilder<E>`对象，再调用`build()`函数创建`Update<E>`对象进行更新数据操作，可以按字段更新，也可以按表对象更新：

```cpp
template<typename E>
Insert<E> dao::_update<E>().build()
```

## 1. 按字段更新

按字段更新指定列的值，使用`set`函数指定更新列，基本形式：

```cpp
dao::_update<MyTable>()
    .set(...)
    .filter(...)
    .build()
    .update() //.updateBatch()
```

### 1.1 按条件更新

使用`set`函数设置更新列，并且使用`filter`函数设置条件，来按条件更新多个数据。对于更新操作，`EntityField<T>`类型的字段具有多种运算符支持，例如`=`、`%`、`+`、`-`、`*`、`/`、`++`、`--`。 例如：

```cpp
User::Fields uf;
int effectRows = dao::_update<User>()
                    .set(uf.age = 19, uf.score + 120)
                    .filter(uf.name == "Alice")
                    .build().update();
```

等同于使用如下sql语句：

```sql
update user set age = 19, score = score + 120 where name = 'Alice'
```

:::note
执行更新后的返回值`effectRows`表示影响的数据行数，并不能表示是否更新成功。
:::

### 1.2 批量更新

使用`updateBatch()`函数进行批量更新，`EntityField<T>`类型的字段与列表组合使用可以按多个条件更新多个数据，例如：

```cpp
auto userNames = QStringList() << "Alice" << "Bob";
auto ages = QList<int>() << 19 << 20;
auto scores = QList<int>() << 120 << 130;

User::Fields uf;
int effectRows = dao::_update<User>()
                    .set(uf.age = ages, uf.score = scores)
                    .filter(uf.name == userNames)
                    .build().updateBatch();
```

## 2. 按对象更新

可以按照表对象进行更新数据，使用对象更新时就不能使用`set`和`filter`，基本形式：

```cpp
dao::_update<MyTable>()
    .build()
    .update(...) //.updateBatch(...)
```

### 2.1 更新一个对象

按对象更新与按字段更新的原理相同，按对象更新时，QtDao内部会自动提取表对象`primary key`的列作为更新条件，其他列作为设置条件，因此，在配置表结构时必须包含`primary key`的列（可以是一个，也可以是多个）。通常情况下，使用对象更新是通过`select`的结果再进行更新，如下面例子：

```cpp
User::Fields uf;
auto user = dao::_select<User>().filter(uf.name == "Alice").build().unique();

user.setAge(19);
user.setScore(120);
int effectRows = dao::_update<User>().build().update(user);
```

### 2.2 更新多个对象

使用`updateBatch()`函数进行批量更新，同样的，传入一个对象列表进行更新，例如：

```cpp
UserList users;
users << User("Alice", 18, 100);
users << User("Bob", 19, 120);
dao::_update<User>().build().updateBatch(users);
```