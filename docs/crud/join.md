---
sidebar_position: 5
---

# Join查询

join联表提供5种连接方式（`cross`、`inner`、`left`、`right`、`full`），但需要注意不同数据库的支持。使用`_join()`函数创建JoinBuilder开始join联表操作：

```cpp
template<typename... E>
JoinBuilder<E...> dao::_join<E...>();
```

## 1. 基本形式

模板参数列表传入参与join联表的所有表，其顺序决定了结果数据输出的顺序。通常，一个join查询具有以下基本形式：

```sql
select [字段列表] from [主表与别名] [连接类型] [子表与别名] on [连接条件] [查询条件] [输出限制]
```

函数调用的基本形式：

```cpp
auto result = dao::_join<MyTb1, MyTb2, MyTb3>()
                .column(...) //.columnAll<>()
                .from<MyTb1>()
                .innerJoin<MyTb2>().on(...) //.leftJoin<>()/.rightJoin<>()/.fullJoin<>()/.crossJoin<>()
                .innerJoin<MyTb3>().on(...)
                .filter(...)
                .with(...)
                .build().list(...)
```

### 1.1 选择列

join查询使用`column`函数设置输出列的方式与`select`查询一样，不同的是，join可以设置多个表的列，如下所示：

```cpp
StudentTb::Field sf;
TeacherTb::Field tf;
ClassTb::Field cf;

dao::_join<StudentTb, TeacherTb, ClassTb>()
    .column(sf.name, sf.score, tf.name)
    .build().list();
```

使用`columnAll`函数读出指定表所有字段：

```cpp
StudentTb::Field sf;
TeacherTb::Field tf;
ClassTb::Field cf;

dao::_join<StudentTb, TeacherTb, ClassTb>()
    .column(sf.name, sf.score)
    .column(tf.name)
    .columnAll<ClassTb>()
    .build().list();
```

### 1.2 设置主表

使用`from()`函数设置查询主表：

```cpp
StudentTb::Field sf;
TeacherTb::Field tf;
ClassTb::Field cf;

dao::_join<StudentTb, TeacherTb, ClassTb>()
    .column(sf.name, sf.score, tf.name)
    .from<StudentTb>()
    .build().list();
```

### 1.3 设置子表连接类型与条件

5种连接类型用不同的函数调用：`crossJoin()`、`innerJoin()`、`leftJoin()`、`rightJoin()`、`fullJoin()`。使用`on()`设置连接条件，这里`on()`函数与普通select查询中`filter`使用方式一样，不同的是，`on()`函数中传入的条件可以是字段与字段之间的运算。

```cpp
StudentTb::Field sf;
TeacherTb::Field tf;
ClassTb::Field cf;

dao::_join<StudentTb, TeacherTb, ClassTb>()
    .from<StudentTb>()
    .innerJoin<TeacherTb>().on(tf.id == sf.teaId, tf.room == 3)
    .innerJoin<ClassTb>().on(cf.id == sf.clsId)
    .build().list();
```

### 1.4 设置主查询条件

与普通select查询使用完全一样，使用`filter()`函数设置主查询条件：

```cpp
StudentTb::Field sf;
TeacherTb::Field tf;
ClassTb::Field cf;

dao::_join<StudentTb, TeacherTb, ClassTb>()
    .from<StudentTb>()
    .innerJoin<TeacherTb>().on(tf.id == sf.teaId)
    .innerJoin<ClassTb>().on(cf.id == sf.clsId)
    .filter(sf.score >= 90)
    .build().list();
```

:::note
主查询条件和`on`设置的条件都可以设置同样的筛选条件，但需要注意在不同数据库引擎的执行上有所区别，这会造成不同的输出结果。
:::

### 1.5 设置限制条件

同样的，使用`with()`函数设置输出限制条件，使用方法与`select`操作一致：

```cpp
StudentTb::Field sf;
TeacherTb::Field tf;
ClassTb::Field cf;

dao::_join<StudentTb, TeacherTb, ClassTb>()
    .from<StudentTb>()
    .innerJoin<TeacherTb>().on(tf.id == sf.teaId)
    .innerJoin<ClassTb>().on(cf.id == sf.clsId)
    .with(_orderBy(sf.name.desc()))
    .build().list();
```

## 2. 读取结果

使用`list()`函数获取结果，这里一定包含多个结果的QList类型列表。

### 2.1 按表获取结果

`list()`函数的输出结果每一条数据的类型是`std::tuple<...>`，其中模版顺序与`_join`函数传入的模板参数列表顺序一致：

```cpp
template<typename... E>
QList<std::tuple<E...>> dao::_join<E...>().build().list();
```

读取结果如下：

```cpp
StudentTb::Field sf;
TeacherTb::Field tf;
ClassTb::Field cf;

auto result = dao::_join<StudentTb, TeacherTb, ClassTb>()
                .column(sf.name, sf.score, tf.name)
                .from<StudentTb>()
                .innerJoin<TeacherTb>().on(tf.id == sf.teaId, tf.room == 3)
                .innerJoin<ClassTb>().on(cf.id == sf.clsId)
                .filter(sf.score >= 90)
                .with(_orderBy(sf.name.desc()))
                .build().list();

for (const auto& r : result) {
    StudentTb dataStu = std::get<0>(r);
    TeacherTb dataTea = std::get<1>(r);
    ClassTb dataCls = std::get<2>(r);
    //...
}
```

### 2.2 按列获取结果

与`select`操作一样，`list`函数可以传入指定字段读取列，`list`函数的声明如下：

```cpp
template<typename T>
QList<T> list(const EntityField<T>& field);

template<typename T, typename K>
QList<QPair<T, K>> list(const EntityField<T>& field1, const EntityField<K>& field2);

template<typename T, typename... Args>
QList<std::tuple<T, typename Args::Type...>> list(const EntityField<T>& field, const Args&... args);
```

使用时如下所示：

```cpp
StudentTb::Field sf;
TeacherTb::Field tf;
ClassTb::Field cf;

QList<QString> names = dao::_join<StudentTb, TeacherTb, ClassTb>()
                        .column(sf.name)
                        .from<StudentTb>()
                        .innerJoin<TeacherTb>().on(tf.id == sf.teaId)
                        .innerJoin<ClassTb>().on(cf.id == sf.clsId)
                        .build().list(sf.name);

QList<QPair<QString, int>> data = dao::_join<StudentTb, TeacherTb, ClassTb>()
                                    .column(sf.name, tf.age)
                                    .from<StudentTb>()
                                    .innerJoin<TeacherTb>().on(tf.id == sf.teaId)
                                    .innerJoin<ClassTb>().on(cf.id == sf.clsId)
                                    .build().list(sf.name, tf.age);

QList<std::tuple<QString, int, int>> data = dao::_join<StudentTb, TeacherTb, ClassTb>()
                                            .column(sf.name, tf.age, cf.room)
                                            .from<StudentTb>()
                                            .innerJoin<TeacherTb>().on(tf.id == sf.teaId)
                                            .innerJoin<ClassTb>().on(cf.id == sf.clsId)
                                            .build().list(sf.name, tf.age, cf.room);
```

## 3. 自连接

有时执行一个简单的递归查询时需要join表自身，这时，自定义一个类继承自`dao::self<E>`类型进行一次重命名操作就能完成自连接：

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