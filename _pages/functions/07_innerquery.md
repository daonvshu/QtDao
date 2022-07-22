---
title: 嵌套查询
category: functions
layout: post
---

select/join查询嵌套
-------------

`QtDao`中的嵌套查询指的是：`'select from (select ...)'` 的一般形式。  `dao::_select()`和`dao::_join()`两个类型中的`from()`函数提供了嵌套查询的能力。

```cpp
template<typename E>
from(Select<E>& select);

template<typename... E>
from(Join<E...>& join);
```

嵌套`select`，其内外层查询模板参数必须一致，查询如下：

```cpp
StudentTb::Field stuField;

auto query0 = dao::_select<StudentTb>()
    .column(stuField.name, stuField.score)
    .filter(stuField.score > 100)
    .build();

auto result = dao::_select<StudentTb>()
    .column(stuField.name)
    .from(query0)
    .filter(stuField.score < 200)
    .build().list();
```

嵌套`join`，外层`select`模板参数必须是`join`（且查询列`column`中有`select`表列出表列）模板参数中之一，查询如下：

```cpp
StudentTb::Field stuField;
TeacherTb::Field teaField;
ClassTb::Field clsField;

auto query0 = dao::_join<StudentTb, TeacherTb, ClassTb>()
    .columnAll<StudentTb>()
    .from<StudentTb>()
    .innerJoin<TeacherTb>().on(teaField.id == stuField.teaId)
    .innerJoin<ClassTb>().on(clsField.id == stuField.clsId)
    .build();

auto result = dao::_select<StudentTb>()
    .column(stuField.name)
    .from(query0)
    .filter(stuField.score >= 90)
    .build().list();
```

`join`可以使用主表嵌套和字表嵌套：

```cpp
StudentTb::Field stuField;
TeacherTb::Field teaField;
ClassTb::Field clsField;

auto stuQuery = dao::_select<StudentTb>().build();

auto teaQuery = dao::_select<TeacherTb>().build();

auto result = dao::_join<StudentTb, TeacherTb, ClassTb>()
    .columnAll<StudentTb>()
    .from(stuQuery)
    .innerJoin(teaQuery).on(teaField.id == stuField.teaId)
    .innerJoin<ClassTb>().on(clsField.id == stuField.clsId)
    .build().list();
```

`count`函数与`select`一样，可以使用嵌套，其模板参数与`filter`等查询条件相关：

```cpp
StudentTb::Field stuField;
TeacherTb::Field teaField;
ClassTb::Field clsField;

auto query0 = dao::_join<StudentTb, TeacherTb, ClassTb>()
    .columnAll<StudentTb>()
    .from<StudentTb>()
    .innerJoin<TeacherTb>().on(teaField.id == stuField.teaId)
    .innerJoin<ClassTb>().on(clsField.id == stuField.clsId)
    .build();

int count = dao::_count<StudentTb>()
    .from(query0)
    .filter(stuField.score >= 90)
    .count();
```

function查询嵌套
-------------

自定义函数条件`function`功能中可以使用嵌套查询，同样的，提供`from`函数支持：

```cpp
StudentTb::Field stuField;

auto namesQuery = dao::_select<StudentTb>()
    .column(stuField.name)
    .filter(stuField.name.like("Alice%"))
    .build();

auto result = dao::_select<StudentTb>()
    .column(stuField.name, stuField.score)
    .filter(
        _fun("%1 in %2").field(stuField.name).from(namesQuery)
    )
    .build().list();
```

递归查询
-------------

`QtDao`中递归查询属于嵌套查询类别，其使用的sql语句一般形式为：

```sql
with [临时表] as
(
    [初始条件查询语句]
    union (all)
    [递归条件查询语句]
)
[select/join查询] from [临时表] ...
```

> 注意：这里查询结构决定了递归查询只适用于`sqlite`和`sqlserver`。

使用递归查询前，自定义一个类继承自`dao::self<E>`类型构造临时表：

```cpp
class StudentTmp : public dao::self<StudentTb> {}
```

使用`dao::_recursive()`开始递归查询：

```cpp
RecursiveQueryBuilder dao::_recursive(bool unionAll = false);
```

使用`tmp()`函数传入临时表：

```cpp
template<typename E>
RecursiveQueryBuilder& RecursiveQueryBuilder::tmp();
```

使用`initialSelect()`函数设置查询初始条件，与嵌套查询方式一致，可以传入`select`或`join`子查询：

```cpp
template<typename E>
RecursiveQueryBuilder& RecursiveQueryBuilder::initialSelect(Select<E>& select);

template<typename ...E>
RecursiveQueryBuilder& RecursiveQueryBuilder::initialSelect(Join<E...>& join);
```

使用`recursiveSelect()`函数设置递归查询条件，同样的可以传入`select`或`join`子查询，通常情况下，其子查询与临时表相关构造递归条件：

```cpp
template<typename E>
RecursiveQueryBuilder& RecursiveQueryBuilder::recursiveSelect(Select<E>& select);

template<typename ...E>
RecursiveQueryBuilder& RecursiveQueryBuilder::recursiveSelect(Join<E...>& join);
```

最后，使用一般嵌套查询，使用`from()`函数将整个递归查询语句作为子查询即可。使用示例如下：

```cpp
class StudentTmp : public dao::self<StudentTb> {};

StudentTb::Field stuField;
StudentTmp::Field tmpField;

auto recursive = dao::_recursive()
    .tmp<StudentTmp>()
    .initialSelect(
        dao::_select<StudentTb>().filter(stuField.number == 50).build()
    )
    .recursiveSelect(
        dao::_join<StudentTb, StudentTmp>()
            .columnAll<StudentTb>()
            .from<StudentTb>()
            .innerJoin<StudentTmp>().on(tmpField.number2 == stuField.number)
            .build()
    );

auto result = dao::_select<StudentTmp>()
    .from(recursive)
    .with(_orderBy(tmpField.score))
    .build().list();
```