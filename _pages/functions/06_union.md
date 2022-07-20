---
title: union联表
category: functions
layout: post
---

`union`操作用于合并多个查询结果，需要注意的是，多个查询结果的列数必须相同。在`QtDao`中，`dao::_select()`和`dao::_join()`两个类型都提供了`unionSelect`函数。

```cpp
template<typename E>
unionSelect(Select<E>& select, bool unionAll = false)

template<typename... E>
unionSelect(Join<E...>& join, bool unionAll = false);
```

- union select

```cpp
StudentTb::Field stuField;
TeacherTb::Field teaField;

auto query0 = dao::_select<StudentTb>()
    .column(stuField.name, stuField.score)
    .filter(stuField.score > 100)
    .build();

auto result = dao::_select<TeacherTb>()
    .column(teaField.name, teaField.score)
    .filter(teaField.age > 20)
    .unionSelect(query0)
    .build().list();
```

- union join

```cpp
StudentTb::Field stuField;
TeacherTb::Field teaField;

auto query0 = dao::_join<StudentTb, TeacherTb>()
    .from<StudentTb>()
    .column(stuField.name, stuField.score)
    .innerJoin<TeacherTb>().on(teaField.id == stuField.teaId)
    .filter(stuField.score >= 90)
    .build();

auto result = dao::_select<StudentTb>()
    .column(stuField.name, stuField.score)
    .filter(stuField.score > 100)
    .unionSelect(query0)
    .build().list();
```