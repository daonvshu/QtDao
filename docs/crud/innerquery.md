---
sidebar_position: 6
---

# 嵌套子查询

在`QtDao`中，`dao::_select()`和`dao::_join()`两个类型都提供了嵌套查询函数。

## 1. from嵌套

`QtDao`中的嵌套查询一般形式为：`'select from (select ...)'`。

```cpp
template<typename E>
from(Select<E>& select);

template<typename... E>
from(Join<E...>& join);
```

嵌套`select`，其内外层查询模板参数必须一致，查询如下：

```cpp
StudentTb::Field sf;

auto query0 = dao::_select<StudentTb>()
    .column(sf.name, sf.score)
    .filter(sf.score > 100)
    .build();

auto result = dao::_select<StudentTb>()
    .column(sf.name)
    .from(query0)
    .filter(sf.score < 200)
    .build().list();
```

嵌套`join`，外层`select`模板参数必须是`join`（且查询列`column`中有`select`表列出表列）模板参数中之一，查询如下：

```cpp
StudentTb::Field sf;
TeacherTb::Field tf;
ClassTb::Field cf;

auto query0 = dao::_join<StudentTb, TeacherTb, ClassTb>()
    .columnAll<StudentTb>()
    .from<StudentTb>()
    .innerJoin<TeacherTb>().on(tf.id == sf.teaId)
    .innerJoin<ClassTb>().on(cf.id == sf.clsId)
    .build();

auto result = dao::_select<StudentTb>()
    .column(sf.name)
    .from(query0)
    .filter(sf.score >= 90)
    .build().list();
```

`join`可以使用主表嵌套和子表嵌套：

```cpp
StudentTb::Field sf;
TeacherTb::Field tf;
ClassTb::Field cf;

auto stuQuery = dao::_select<StudentTb>().build();

auto teaQuery = dao::_select<TeacherTb>().build();

auto result = dao::_join<StudentTb, TeacherTb, ClassTb>()
    .columnAll<StudentTb>()
    .from(stuQuery)
    .innerJoin(teaQuery).on(tf.id == sf.teaId)
    .innerJoin<ClassTb>().on(cf.id == sf.clsId)
    .build().list();
```

`count`函数与`select`一样，可以使用嵌套，其模板参数与`filter`等查询条件相关：

```cpp
StudentTb::Field sf;
TeacherTb::Field tf;
ClassTb::Field cf;

auto query0 = dao::_join<StudentTb, TeacherTb, ClassTb>()
    .columnAll<StudentTb>()
    .from<StudentTb>()
    .innerJoin<TeacherTb>().on(tf.id == sf.teaId)
    .innerJoin<ClassTb>().on(cf.id == sf.clsId)
    .build();

int count = dao::_count<StudentTb>()
    .from(query0)
    .filter(sf.score >= 90)
    .count();
```

## 2. filter条件嵌套

自定义函数条件`function`功能中可以使用嵌套查询，同样的，提供`from`函数支持：

```cpp
StudentTb::Field sf;

auto namesQuery = dao::_select<StudentTb>()
    .column(sf.name)
    .filter(sf.name.like("Alice%"))
    .build();

auto result = dao::_select<StudentTb>()
    .column(sf.name, sf.score)
    .filter(
        _fun("%1 in %2").field(sf.name).from(namesQuery)
    )
    .build().list();
```

## 3. 递归查询

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

:::warning
注意：这里查询结构决定了递归查询不适用于`mysql`。
:::

### 3.1 递归主查询

使用`_recursive(bool unionAll)`函数构造递归主查询，调用基本形式：

```cpp
auto recursive = dao::_recursive(true)
                    .tmp<TmpTb>()
                    .initialSelect(initQuery)
                    .recursiveSelect(recursiveQuery);
```

使用递归查询前，自定义一个类继承自`dao::self<E>`类型构造临时表：

```cpp
class StudentTmp : public dao::self<StudentTb> {}
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

### 3.2 嵌套查询

使用一般嵌套查询中的`from()`函数将整个递归查询语句作为子查询即可。使用示例如下：

```cpp
class StudentTmp : public dao::self<StudentTb> {};

StudentTb::Field sf;
StudentTmp::Field tpf;

auto recursive = dao::_recursive()
    .tmp<StudentTmp>()
    .initialSelect(
        dao::_select<StudentTb>().filter(sf.number == 50).build()
    )
    .recursiveSelect(
        dao::_join<StudentTb, StudentTmp>()
            .columnAll<StudentTb>()
            .from<StudentTb>()
            .innerJoin<StudentTmp>().on(tpf.number2 == sf.number)
            .build()
    );

auto result = dao::_select<StudentTmp>()
    .from(recursive)
    .with(_orderBy(tpf.score))
    .build().list();
```

## 4. Union联表

`union`操作用于合并多个查询结果，需要注意的是，多个查询结果的列数和名称必须相同。

```cpp
template<typename E>
unionSelect(Select<E>& select, bool unionAll = false);

template<typename... E>
unionSelect(Join<E...>& join, bool unionAll = false);
```

select之间联表

```cpp
StudentTb::Field sf;
TeacherTb::Field tf;

auto query0 = dao::_select<StudentTb>()
    .column(sf.name, sf.score)
    .filter(sf.score > 100)
    .build();

auto result = dao::_select<TeacherTb>()
    .column(tf.name, tf.score)
    .filter(tf.age > 20)
    .unionSelect(query0)
    .build().list();
```

select与join联表

```cpp
StudentTb::Field sf;
TeacherTb::Field tf;

auto query0 = dao::_join<StudentTb, TeacherTb>()
    .from<StudentTb>()
    .column(sf.name, sf.score)
    .innerJoin<TeacherTb>().on(tf.id == sf.teaId)
    .filter(sf.score >= 90)
    .build();

auto result = dao::_select<StudentTb>()
    .column(sf.name, sf.score)
    .filter(sf.score > 100)
    .unionSelect(query0)
    .build().list();
```