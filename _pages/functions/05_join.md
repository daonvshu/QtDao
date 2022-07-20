---
title: join联表
category: functions
layout: post
---

join联表提供5种连接方式（`cross`、`inner`、`left`、`right`、`full`），但需要注意不同数据库的支持。使用`_join()`函数创建JoinBuilder开始join联表操作：

```cpp
template<typename... E>
JoinBuilder<E...> dao::_join<E...>();
```

模板参数列表传入参与join联表的所有表，其顺序决定了结果数据输出的顺序。通常，一个join查询具有以下基本形式：

```sql
select [字段列表] from [主表与别名] [连接类型] [字表与别名] on [连接条件] [查询条件] [输出限制]
```

设置输出字段
-------------

使用`column()`函数设置各个表需要输出的字段值，忽略调用时将导致所有字段输出。

```cpp
StudentTb::Field stuField;
TeacherTb::Field teaField;
ClassTb::Field clsField;

dao::_join<StudentTb, TeacherTb, ClassTb>()
    .column(stuField.name, stuField.score, teaField.name)
    .build().list();
```

与普通查询一样，`column()`也可以联级调用，使用`columnAll()`读出所有字段：

```cpp
StudentTb::Field stuField;
TeacherTb::Field teaField;
ClassTb::Field clsField;

dao::_join<StudentTb, TeacherTb, ClassTb>()
    .column(stuField.name, stuField.score)
    .column(teaField.name)
    .columnAll<ClassTb>()
    .build().list();
```

设置主表
-------------

使用`from()`函数设置查询主表：

```cpp
StudentTb::Field stuField;
TeacherTb::Field teaField;
ClassTb::Field clsField;

dao::_join<StudentTb, TeacherTb, ClassTb>()
    .column(stuField.name, stuField.score, teaField.name)
    .from<StudentTb>()
    .build().list();
```

设置字表连接类型与条件
-------------

5种连接类型用不同的函数调用：  
`crossJoin()`、`innerJoin()`、`leftJoin()`、`rightJoin()`、`fullJoin()`。使用`on()`设置连接条件，这里`on()`函数与普通select查询中`filter`使用方式一样，不同的是，`on()`函数中传入的条件可以是字段与字段之间的运算。

```cpp
StudentTb::Field stuField;
TeacherTb::Field teaField;
ClassTb::Field clsField;

dao::_join<StudentTb, TeacherTb, ClassTb>()
    .from<StudentTb>()
    .innerJoin<TeacherTb>().on(teaField.id == stuField.teaId, teaField.room == 3)
    .innerJoin<ClassTb>().on(clsField.id == stuField.clsId)
    .build().list();
```

设置主查询条件
-------------

与普通select查询使用完全一样，使用`filter()`函数设置主查询条件：

```cpp
StudentTb::Field stuField;
TeacherTb::Field teaField;
ClassTb::Field clsField;

dao::_join<StudentTb, TeacherTb, ClassTb>()
    .from<StudentTb>()
    .innerJoin<TeacherTb>().on(teaField.id == stuField.teaId)
    .innerJoin<ClassTb>().on(clsField.id == stuField.clsId)
    .filter(stuField.score >= 90)
    .build().list();
```

设置结果输出限制条件
-------------

同样的，使用`with()`函数设置输出限制条件：

```cpp
StudentTb::Field stuField;
TeacherTb::Field teaField;
ClassTb::Field clsField;

dao::_join<StudentTb, TeacherTb, ClassTb>()
    .from<StudentTb>()
    .innerJoin<TeacherTb>().on(teaField.id == stuField.teaId)
    .innerJoin<ClassTb>().on(clsField.id == stuField.clsId)
    .with(_orderBy(stuField.name.desc()))
    .build().list();
```

查询结果的获取
-------------

使用`list()`函数输出查询结果，其结果为QList列表：

```cpp
template<typename... E>
QList<std::tuple<E...>> dao::_join().build().list();
```

其中，`std::tuple`中的模板参数列表，正是`_join<E...>()`函数出入的模板参数列表。读取结果示例如下：

```cpp
StudentTb::Field stuField;
TeacherTb::Field teaField;
ClassTb::Field clsField;

auto result = dao::_join<StudentTb, TeacherTb, ClassTb>()
    .column(stuField.name, stuField.score, teaField.name)
    .from<StudentTb>()
    .innerJoin<TeacherTb>().on(teaField.id == stuField.teaId, teaField.room == 3)
    .innerJoin<ClassTb>().on(clsField.id == stuField.clsId)
    .filter(stuField.score >= 90)
    .with(_orderBy(stuField.name.desc()))
    .build().list();

for (const auto& r : result) {
    StudentTb dataStu = std::get<0>(r);
    TeacherTb dataTea = std::get<1>(r);
    ClassTb dataCls = std::get<2>(r);
    //...
}
```

自连接
-------------

有时执行一个简单的递归查询时需要join表自己，这时，自定义一个类继承自`dao::self<E>`类型进行一次重命名操作就能完成自连接：

```cpp
class StudentTmp : public dao::self<StudentTb> {};

StudentTb::Field stuField;
StudentTmp::Field tmpField;

auto result = dao::_join<StudentTb, StudentTmp>()
    .from<StudentTb>()
    .innerJoin<StudentTmp>().on(tmpField.cls2Id == stuField.clsId)
    .filter(stuField.score >= 90)
    .build().list();
```