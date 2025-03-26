---
sidebar_position: 8
---

# 自定义操作

## 1. 自定义类型

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

Qt中有默认有许多类型自身实现了`QDataStream`序列化操作符，参考Qt文档[Serializing Qt Data Types](https://doc.qt.io/qt-5/datastreamformat.html)，因此在使用时可以直接使用，无需再实现`"operator<<"`和`"operator>>"`操作符。

## 2. 自定义条件

`QtDao`无法直接使用绝大多数的sql内置函数，这时使用自定义条件构造函数来解决此问题。使用`_fun()`函数构造自定义条件查询语句，可以用于select查询中的`column`、`filter`、`on`（join查询）、`_having`等函数。如下示例调用sqlite的random函数：

```cpp
dao::_select<User>()
    .column(_fun("random() as Random"))
    .build().one();
```

### 2.1 使用字段和值

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

### 2.2 嵌套子查询

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

### 2.3 读取结果

如果在字段列表`column`函数中构造表达式，读取结果使用对应实例的`__getExtra`获取，若使用`as`关键字，则读取时使用as重命名后的字符串读取值：

```cpp
User::Field field;

auto user = dao::_select<User>()
    .column(_fun("sum(%1)").field(field.score))
    .build().one();

int sumScore = user.__getExtra<int>("sum(score)");

auto user2 = dao::_select<User>()
    .column(_fun("sum(%1) as sumScore").field(field.score))
    .build().one();

sumScore = user.__getExtra<int>("sumScore");
```

## 3. 自定义查询语句

`select`查询中除了使用`raw`函数手动处理查询结果的方法外，使用`BaseQuery::queryPrimitive`完全自定义查询：

```cpp
static QSqlQuery BaseQuery::queryPrimitive(const QString& statement,
                                    const QVariantList& values = QVariantList(),
                                    qint64 sessionId = -1,
                                    LoggingCategoryPtr logging = nullptr);
```

`statement`传入sql语句，如果sql语句中使用占位符`'?'`，则使用`values`传入值列表，`sessionId`用于多数据库查询，`logging`则用于日志筛选器。该重载函数受内置的连接池管理：

```cpp
{
    auto query = dao::BaseQuery::queryPrimitive("select ? + ?", QVariantList() << 10 << 20);
    if (query.next()) {
        int result = query.value(0).toInt();
    }
}
```

另外一个重载函数版本可以直接指定数据库和连接名，该函数不受连接池管理：

```cpp
static void BaseQuery::queryPrimitive(const QString& statement,
                                      qint64 sessionId = -1,
                                      const QString& databaseName = QString(),
                                      QString connectionName = QString(),
                                      const std::function<void(QSqlQuery&)>& resultReader = nullptr);
```

`statement`传入sql语句，`sessionId`用于多数据库查询，`databaseName`用于指定数据库（忽略时使用当前session上下文配置中的数据库），`connectionName`用于指定当前查询的连接名（忽略时使用固定名称`temp_execute_query`），`resultReader`回调函数读取数据：

```cpp
dao::BaseQuery::queryPrimitive("select 10 + 20", -1, "", "", [&](QSqlQuery& query) {
    if (query.next()) {
        int result = query.value(0).toInt();
    }
});
```
