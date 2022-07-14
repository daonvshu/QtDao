---
title: 查询
category: functions
layout: post
---

QtDao的查询以实体类（生成器插件根据配置xml文件生成的模型类）查询为主，使用函数`_select<E>`返回一个`SelectBuild<E>`对象，通过传入模板参数指定需要查询的表：

```cpp
template<typename E>
SelectBuilder<E> dao::_select<E>()
```

通过`SelectBuild<E>`设置查询条件，如 `column()`、`filter()`、`with()`，最后调用`build()`函数创建`Select<E>`对象完成最终查询：

```cpp
template<typename E>
Select<E> dao::_select<E>().build()
```

column()
-------------

- #### 字段设置

```cpp
template<typename E>
template<typename T>
SelectBuilder<E> dao::_select<E>().column(const EntityField<T>& field, ...);
```

`column()`函数用于设置查询的实体类字段，通常，查询结果为实体的一个实例或列表，只有被设置到`column()`函数中字段的值才会被设置到实例对于字段中，其他字段将被设置为默认值（若配置）或随机值，相当于使用 `'select field from Table'` 而不是 `'select * from Table'`。
> 注意：默认情况下，忽略调用`column()`函数将导致所有字段被查询并设置。

如下，查询所有用户的姓名`name`和年龄`age`：

```cpp
User::Fields field;
UserList user = dao::_select<User>().column(field.name, field.age).build().list();
```

等同于sql语句：

```sql
select name, age from User
```

在此结果列表容器中，只有`name`和`age`字段有效，其他字段为默认值或随机值。

- #### 函数字段

```cpp
template<typename E>
SelectBuilder<E> dao::_select<E>().column(const FunctionCondition& field, ...);
```

`column()`函数还可以用于读取`_fun()`的返回结果，如下计算所有用户分数`score`的总和：

```cpp
User::Fields field;
User user = dao::_select<User>()
    .column(_fun("sum(%1) as sumscore").field(field.score))
    .build().unique();
```

等同于sql语句：

```sql
select sum(score) as sumscore from User
```

`_fun()`的使用后面将做详细说明，在这里，函数作为字段设置时，其`as`别名将设置到实例的`__extra`变量中，如下读取上述的总分数：

```cpp
int score = user.__getExtra("sumscore").toInt();
```

- #### 混合字段

当然，字段和函数可以混合使用，可以联级使用，若想读取所有字段又想添加函数，可以使用 `columnAll` 函数，如下几种用法：

```cpp
User::Fields field;

//字段函数混合使用
User user = dao::_select<User>()
    .column(field.name, _fun("max(%1) as maxscore").field(field.score))
    .build().unique();

//联级调用
User user = dao::_select<User>()
    .column(field.name)
    .column(field.age)
    .column(_fun("max(%1) as maxscore").field(field.score))
    .build().unique();

//所有字段与函数
User user = dao::_select<User>()
    .columnAll()
    .column(_fun("max(%1) as maxscore").field(field.score))
    .build().unique();
```

filter()
-------------

- #### 查询条件设置

```cpp
template<typename E>
SelectBuilder<E> dao::_select<E>().filter(const EntityCondition& condition, ...);
```

`filter()`函数用于设置查询条件，传入的参数为字段与值运算的`EntityCondition`类型结果。在设置条件时，应该使用生成器生成的模型类中`E::Fields`成员表示的字段，其中每个成员都是`EntityField<T>`类型，该类型重载了部分运算符如：`=`, `!=`, `>`, `>=`, `<`, `<=` 。

> 注意：重载后的运算符与值运算时严格使用与字段一致的参数类型，字段声明为`QString`类型，则运算对象必须是`QString`类型。

如下，查询分数`score`大于100的用户：

```cpp
User::Fields field;
UserList user = dao::_select<User>()
    .filter(field.score > 100)
    .build().list();
```

- #### 查询条件的组合

filter函数传入的条件可以连续设置多个，多个之间用逗号连接等同于sql中 `and` 连接符，显式使用 `_and()` 和 `_or()` 函数可以组合多个条件。另外，filter也可以联级调用，等同于使用 `and` 连接符。

> 注意：`_and()` 和 `_or()` 函数连接时是对函数各个参数条件之间使用对应的连接符进行连接，并且连接后会自动添加括号运算符。

```cpp
User::Fields field;
UserList user = dao::_select<User>()
    .filter(field.name.like("Alice%"))
    .filter(
        field.age >= 18, 
        _or(field.score > 200, _and(field.score > 50, field.score <= 100))
    )
    .build().list();
```

等同于sql语句：

```sql
select *from User where name like 'Alice%' and age >= 18 and (score > 200 or (score > 50 and score <= 100))
```

`_and()` 和 `_or()` 函数还可以用于临时条件的存储：

```cpp
UserList findPassingUsers(QString keywords) {
    User::Fields field;

    auto condition = _and(
        field.age >= 18, 
        _or(field.score > 200, _and(field.score > 50, field.score <= 100))
    );
    if (!keywords.isEmpty()) {
        condition = _and(field.name.like("Alice%"), condition);
    }

    return dao::_select<User>().filter(condition).build().list();
}

```

`EntityField<T>`类型还支持以下预设的条件函数：

- like/glob

同sql语句中的`like/glob`，用于模糊查找条件，通常情况下，仅QString类型才会使用它，如下所示：

```cpp
User::Fields field;
UserList user = dao::_select<User>()
    .filter(field.name.like("%Alice%"))
    .build().list();
```

- in

同sql语句中的`in`字句，用于范围查找。传入类型为`QList<T>`，其中`T`与字段类型相匹配。如下所示：

```cpp
auto scores = QList<int>() << 50 << 100 << 150;

User::Fields field;
UserList user = dao::_select<User>()
    .filter(field.score.in(scores))
    .build().list();
```

- between

同sql语句中的`between`字句，用于范围查找。传入字段相匹配类型参数，如下所示：

```cpp
User::Fields field;
UserList user = dao::_select<User>()
    .filter(field.score.between(50, 100))
    .build().list();
```

- lsNull/notNull

判断null的条件：

```cpp
User::Fields field;
UserList user = dao::_select<User>()
    .filter(field.name.notNull())
    .build().list();
```

- #### 自定义条件

```cpp
template<typename E>
SelectBuilder<E> dao::_select<E>().filter(const FunctionCondition& function, ...);
```

当需要使用自定义的条件时，使用`_fun()`创建自定义查询条件，如下所示：

```cpp
User::Fields field;
UserList user = dao::_select<User>()
    .filter(field.name == "Alice"))
    .filter(_fun("sum(%1) as sumscore").field(field.score))
    .build().one();
```

with()
-------------

```cpp
template<typename E>
SelectBuilder<E> dao::_select<E>().with(const ConditionConstraint& constaint, ...);
```

`with()`函数用于对查询结果的限制，如sql原语中的`'limit'`、`'order by'`、`'group by'`、`'having'`子句等。

- limit

`_limit()`函数用于限制返回结果数量，同sql语句`limit`，如下：

```cpp
//输出20条结果
User::Fields field;
UserList user = dao::_select<User>()
    .filter(field.score > 100)
    .with(_limit(20))
    .build().list();

//从第5个结果开始，输出20条结果
User::Fields field;
UserList user = dao::_select<User>()
    .filter(field.score > 100)
    .with(_limit(5, 20))
    .build().list();
```

- order by

`_orderBy()`函数用于排序，同sql语句`order by`，如下：

```cpp
User::Fields field;
UserList user = dao::_select<User>()
    .filter(field.score > 100)
    .with(_orderBy(field.name, field.age))
    .build().list();
```

- group by / having

`_groupBy()`函数用于对查询结果进行分组，`_having()`函数用于设置分组结果的过滤条件，如下：

```cpp
User::Fields field;
UserList user = dao::_select<User>()
    .with(_groupBy(field.name, field.age), _having(field.score > 100))
    .build().list();

User::Fields field;
UserList user = dao::_select<User>()
    .with(_groupBy(field.name))
    .with(_having(_fun("count(%1) > 2").field(field.id)))
    .build().list();
```

- _constraint()

`_constraint()`函数用于连接结果限制函数。通常情况下，多个结果限制函数可以连续使用：

```cpp
User::Fields field;
UserList user = dao::_select<User>()
    .with(_groupBy(field.name), _limit(5))
    .build().list();
```

在需要临时存储限制条件时，使用`_constraint()`函数组合：

```cpp
UserList getUserNames(int limitSize) {
    User::Fields field;

    auto condition = _groupBy(field.name);
    if (limitSize > 0) {
        condition = _constraint(condition, _limit(limitSize));
    }

    return dao::_select<User>().with(condition).build().list();
}
```