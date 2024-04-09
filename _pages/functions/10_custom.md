---
title: 自定义数据库操作
category: functions
layout: post
---

自定义类型
-------------

在`QtDao`中允许使用自定义类型，定义数据表时配置如下：

```xml
<?xml version="1.0" encoding="utf-8"?>
<dao prefix="ts_" db="mysql">
    <tb name="TestTb">
        <item name="qtType" type="custom" customType="QRect" note="qt default supported type for serialize">
        <item name="myStruct" type="custom" customtype="MyStruct">
        <item name="collectionType" type="custom" customtype="QHash&lt;QString, QList&lt;MyStruct&gt;&gt;">
    </tb>
</dao>
```

其工作原理是，使用`QDataStream`存储时进行序列化为`QByteArray`类型，读取时反序列化为定义类型，使用的数据库类型为`blob/binary`。因此，使用自定义类型时确保定义的类型可反序列化，通常情况下，你需要实现`"QDataStream::operator<<"`和`"QDataStream::operator>>"`操作符，另外还需要实现自身的`"operator=="`用于模型类等于操作符。如下示例：

```cpp
#include <qdatastream.h>
#include <qdatetime.h>

struct MyStruct {
    QString name;
    int id;
    QDateTime dateTime;

    bool operator==(const MyStruct& other) const {
        return other.name == this->name 
            && other.id == this->id 
            && other.dateTime == this->dateTime;
    }
};

inline QDataStream& operator<<(QDataStream& out, const MyStruct& data) {
    out << data.name << data.id << data.dateTime;
    return out;
}

inline QDataStream& operator>>(QDataStream& in, MyStruct& data) {
    in >> data.name >> data.id >> data.dateTime;
    return in;
}
```

Qt中有默认有许多类型自身实现了`QDataStream`序列化操作符，参考Qt文档[Serializing Qt Data Types](https://doc.qt.io/qt-5/datastreamformat.html){:target="_blank"}，因此在使用时可以直接使用，无需再实现`"operator<<"`和`"operator>>"`操作符。

自定义条件
-------------

`QtDao`无法直接使用绝大多数的sql内置函数，这时使用自定义条件构造函数来解决此问题。使用`_fun()`函数构造自定义条件查询语句，可以用于select查询中的`column`、`filter`、`on`（join查询）、`_having`等函数。如下示例调用sqlite的random函数：

```cpp
dao::_select<User>()
    .column(_fun("random() as Random"))
    .build().one();
```

- 使用字段和值

`QtDao`中使用`QString`的arg函数拼接字符串，因此，在条件表达式中使用`'%'`占位符为字段占位。使用`field()`函数传入字段：

```cpp
User::Field field;

dao::_select<User>()
    .column(_fun("sum(%1) + max(%2)").field(field.score, field.age))
    .build().one();
```

值绑定使用的是位置占位符`'?'`，使用`value()`函数传入值：

```cpp
User::Field field;

dao::_select<User>()
    .column(_fun("sum(%1) + ?").field(field.score).value(60))
    .build().one();
```

- 嵌套子查询

使用`from`函数可以构造子查询条件，其中使用`'%'`占位符对应子查询语句：

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

- 读取结果

如果在字段列表`column`函数中构造表达式，读取结果使用对应实例的`__getExtra`获取，若使用`as`关键字，则读取时使用as重命名后的字符串读取值：

```cpp
User::Field field;

auto user = dao::_select<User>()
    .column(_fun("sum(%1)").field(field.score))
    .build().one();

int sumScore = user.__getExtra("sum(score)").toInt();

auto user2 = dao::_select<User>()
    .column(_fun("sum(%1) as sumScore").field(field.score))
    .build().one();

sumScore = user.__getExtra("sumScore").toInt();
```

自定义结果处理
-------------

`QtDao`可以方便的将查询结果直接转换为实体对象列表，但对于查询结果数据量太大时，大量的时间会被花费在对象转换操作上。对于这种情况下，使用`column()`函数明确指定查询部分字段将大幅减少查询时间，即便如此，有时其转换效率也不尽如意。当不需要转换为对象，自主处理查询结果时，可以极大的减少查询时间。

```cpp
template<typename E>
Select<E> dao::_select<E>().build().raw(std::function<void(QSqlQuery&)> callback);
```

使用`raw()`函数，不经过对象转换，直接读取结果：

```cpp
User::Fields field;
dao::_select<User>()
    .filter(field.age < 18)
    .build().raw([&](QSqlQuery& query) {
        while (query.next()) {
            //...
        }
    });
```

自定义查询语句
-------------

除了使用`raw`函数手动处理查询结果的方法外，使用`BaseQuery::queryPrimitive`完全自定义查询：

```cpp
static void BaseQuery::queryPrimitive(
    const QString& statement, 
    std::function<void(QSqlQuery& query)> callback = nullptr,
    std::function<void(QString)> failCallback = nullptr
);

static void BaseQuery::queryPrimitive(
    const QString& statement,
    const QVariantList& values,
    std::function<void(QSqlQuery& query)> callback = nullptr,
    std::function<void(QString)> failCallback = nullptr,
);
```

`statement`传入sql语句，如果sql语句中使用占位符`'?'`，则使用`values`传入值列表。`callback`回调用于处理查询结果，`failCallback`用于处理失败情况，若省略则交给统一异常处理回调：

```cpp
dao::BaseQuery::queryPrimitive("select 10 + 20", [&](QSqlQuery& query) {
    if (query.next()) {
        int result = query.value(0).toInt();
    }
}, [&](QString err) {
    //...
});
```

> 与其他查询一样，该查询方式中的数据库连接受连接池管理。