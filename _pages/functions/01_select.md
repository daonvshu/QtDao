---
title: 查询
category: functions
layout: post
---

QtDao的查询以实体类（生成器插件根据配置xml文件生成的模型类）查询为主，使用函数`_select<E>`返回一个`SelectBuild<E>`对象：

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
SelectBuilder<E> dao::_select<E>().column(const EntityField<T>& field);
```

`column()`函数用于设置查询的实体类字段，通常，查询结果为实体的一个实例或列表，只有被设置到`column()`函数中字段的值才会被设置到实例对于字段中，其他字段将被设置为默认值（若配置）或随机值，相当于使用 `'select field from Table'` 而不是 `'select * from Table'`。
> 注意：默认情况下，忽略调用`column()`函数将导致所有字段被查询并设置。

如下，查询所有用户的姓名`name`和年龄`age`：

```cpp
User::Fields field;
UserList user = dao::_select<User>.column(field.name, field.age).build().list()
```

等同于sql语句：

```sql
select name, age from User
```

在此结果列表容器中，只有`name`和`age`字段有效，其他字段为默认值或随机值。

- #### 函数字段

```cpp
template<typename E>
SelectBuilder<E> dao::_select<E>().column(const FunctionCondition& field);
```

`column()`函数还可以用于读取`函数`的返回结果，如下计算所有用户分数`score`的总和：

```cpp
User::Fields field;
User user = dao::_select<User>
    .column(_fun("sum(%1) as sumscore").field(field.score))
    .build().unique()
```

等同于sql语句：

```sql
select sum(score) as sumscore from User
```

函数的使用后面将做详细说明，在这里，函数作为字段设置时，其`as`别名将设置到实例的`__extra`变量中，如下读取上述的总分数：

```cpp
int score = user.__getExtra("sumscore").toInt();
```

- #### 混合字段

当然，字段和函数可以混合使用，可以联级使用，若想读取所有字段又想添加函数，可以使用 `columnAll` 函数，如下几种用法：

```cpp
User::Fields field;

//字段函数混合使用
User user = dao::_select<User>
    .column(field.name, _fun("max(%1) as maxscore").field(field.score))
    .build().unique()

//联级调用
User user = dao::_select<User>
    .column(field.name)
    .column(field.age)
    .column(_fun("max(%1) as maxscore").field(field.score))
    .build().unique()

//所有字段与函数
User user = dao::_select<User>
    .columnAll()
    .column(_fun("max(%1) as maxscore").field(field.score))
    .build().unique()
```