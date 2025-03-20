---
sidebar_position: 1
---

# 查询

QtDao执行`select`查询，使用函数`_select<E>`返回一个`SelectBuild<E>`对象，通过传入模板参数指定需要查询的表：

```cpp
template<typename E>
SelectBuilder<E> dao::_select<E>();
```

## 1. 基本形式

构造`select`查询的几个基本部分：选择查询列、设置筛选条件、设置限制条件、设置输出类型：

```cpp
auto result = dao::_select<MyTable>()
                .column(...)//.columnAll()
                .filter(...)
                .with(...)
                .build()
                .one();//.unique()/.list()
```

## 2. 选择列

```cpp
SelectBuilder<E> dao::_select<E>()
    .column(const EntityField<T>& field, ...)
    .column(const FunctionCondition& field, ...)
    .column(bool enabled, const EntityField<T>& field)
    .column(bool enabled, const FunctionCondition& field)
    ;
```

`column()`函数用于设置查询的表字段，通常，查询结果为实体的一个对象或列表，只有被设置到`column()`函数中字段的值才会被设置到对象对应字段中，其他字段将被设置为默认值（若配置）或随机值，相当于使用 `'select field from mytable'` 而不是 `'select * from mytable'`。

:::note
`column()`函数的调用不是必要的，当忽略时，该表的字段会被全部取出，相当于执行`'select * from mytable'`。因此，在有性能要求的情况下，设置查询列既可以减少查询输出的时间也可以避免不需要的字段进行内存的复制。`columnAll()`函数如字面意思，取出所有字段，与忽略调用的操作不同的是，select列设置为所有字段，而非`*`匹配。
:::

### 2.1 选择字段

使用`EntityField<T>`类型的字段作为选择列，该类型为生成器生成的类文件中的一个类型。例如，生成的`class User`对应`user`表，那么类中包含一个`User::Fields`类型的字段列表，该字段列表包含该表所有字段，并且字段与对应的表关联，使用选择字段时要使用该表对应的类下面的字段。例如下面的例子：

```cpp
User::Fields uf;
UserList results = dao::_select<User>()
                        .column(uf.id)
                        .build().list();
```

等同于sql语句：

```sql
select id from user
```

字段列表可以组合，也可以连级调用：

```cpp
User::Fields uf;
UserList results = dao::_select<User>()
                        .column(uf.id, uf.name)
                        .column(uf.number)
                        .build().list();
```

可根据条件是否选择某字段，这种情况下只能选择一个字段，但是可以连级调用其他字段例如：

```cpp
bool selectNumber = true;
User::Fields uf;
UserList results = dao::_select<User>()
                        .column(uf.id, uf.name)
                        .column(selectNumber, uf.number)
                        .build().list();
```

使用`distinct()`函数构造字段，对结果可以去重：

```cpp
bool selectNumber = true;
User::Fields uf;
UserList results = dao::_select<User>()
                        .column(uf.name.distinct())
                        .build().list();
```

### 2.2 选择函数

使用`FunctionCondition`类型的字段作为选择列可以传递一个函数条件。使用`_fun()`函数构造函数字段，例如，计算所有用户分数`score`的总和：

```cpp
User::Fields uf;
User user = dao::_select<User>()
    .column(_fun("sum(%1) as sumscore").field(uf.score))
    .build().unique();
```

等同于sql语句：

```sql
select sum(score) as sumscore from user
```

函数的使用见[自定义操作篇](/QtDao/docs/custom#自定义类型)说明，使用函数作为字段时，其`as`别名将设置到实体对象的`__extra`变量中，如下读取上述的总分数：

```cpp
int score = user.__getExtra<int>("sumscore");
```

函数和字段可以混合使用：

```cpp
User::Fields uf;

//字段函数混合使用
User user = dao::_select<User>()
    .column(uf.name, _fun("max(%1) as maxscore").field(uf.score))
    .build().unique();

//联级调用
User user = dao::_select<User>()
    .column(uf.name)
    .column(uf.age)
    .column(_fun("max(%1) as maxscore").field(uf.score))
    .build().unique();

//所有字段与函数
User user = dao::_select<User>()
    .columnAll()
    .column(_fun("max(%1) as maxscore").field(uf.score))
    .build().unique();
```

## 3. 设置筛选条件

```cpp
SelectBuilder<E> dao::_select<E>()
    .filter(EntityConnector* condition, ...)
    .filter(const FunctionCondition& field, ...)
    .filter(bool enabled, EntityConnector* field)
    .filter(bool enabled, const FunctionCondition& field)
    ;
```

`filter()`函数用于设置`where`查询条件，可以传入由字段构造的`EntityConnector*`类型的条件，也可以传入函数类型条件。

### 3.1 设置字段条件

使用字段构造条件，同样的使用`EntityField<T>`类型的表字段，`EntityField<T>`类型重载部分运算符和函数用来快速构造条件：`=`, `!=`, `>`, `>=`, `<`, `<=`，`like`，`glob`，`in`，`between`，`lsNull`、`notNull`。`EntityField<T>`对运算符有多个重载函数，运算符构造条件时，需要使用`EntityField<T>::operator==(const T& v)`这个函数，其他版本重载函数用于其他类型数据库操作。

:::note
重载后的运算符与值运算时严格使用与字段一致的参数类型，字段声明为`QString`类型，则运算对象必须是`QString`类型。
:::

如下，查询分数`score`大于100的用户：

```cpp
User::Fields uf;
UserList user = dao::_select<User>()
    .filter(uf.score > 100)
    .build().list();
```

条件可以组合，也可以连级调用：

```cpp
User::Fields uf;
UserList results = dao::_select<User>()
                        .filter(uf.name.like("Alice%"))
                        .filter(uf.age > 12, uf.score >= 60)
                        .build().list();
```

组合或连级调用时等同于使用`and`连接，上面的查询等同于：

```sql
select *from user where name like 'Alice%' and age > 12 and score >= 60
```

使用`_and()`和`_or()`函数组合复杂条件：

```cpp
User::Fields uf;
UserList user = dao::_select<User>()
    .filter(uf.name.like("Alice%"))
    .filter(
        uf.age >= 18, 
        _or(uf.score > 200, _and(uf.score > 50, uf.score <= 100))
    )
    .build().list();
```

`_and()`和`_or()`函数会自动拼接括号，上面的查询等同于：

```sql
select *from user where name like 'Alice%' and age >= 18 and (score > 200 or (score > 50 and score <= 100))
```

可根据条件是否使用字段条件：

```cpp
bool filterWithScore = true;
User::Fields uf;
UserList results = dao::_select<User>()
                        .filter(uf.name.like("Alice%"))
                        .filter(filterWithScore, uf.score >= 60)
                        .build().list();
```

### 3.2 设置函数条件

也可以使用`_fun()`创建自定义查询条件，例如：

```cpp
User::Fields uf;
User user = dao::_select<User>()
    .filter(uf.name == "Alice"))
    .filter(_fun("%1*2 + %2 > ?").field(uf.age, uf.score).value(200))
    .build().one();
```

等同于：

```sql
select *from user where name = 'Alice' and age*2 + score > 200
```

## 4. 设置限制条件

```cpp
SelectBuilder<E> dao::_select<E>()
    .with(ConstraintConnector* connector, ...)
    .with(ConstraintGroupGroupConnector* connector, ...)
    .with(HavingGroupConnector* connector, ...)
    .with(bool enabled, ConstraintConnector* connector)
    .with(bool enabled, ConstraintGroupGroupConnector* connector)
    .with(bool enabled, HavingGroupConnector* connector)
    ;
```

`with()`函数用于对查询结果的限制，如sql原语中的`'limit'`、`'order by'`、`'group by'`、`'having'`子句等。

### 4.1 limit

`_limit()`函数用于限制返回结果数量，同sql语句`limit`，如下：

```cpp
//输出20条结果
User::Fields uf;
UserList user = dao::_select<User>()
    .filter(uf.score > 100)
    .with(_limit(20))
    .build().list();

//从第5个结果开始，输出20条结果
User::Fields uf;
UserList user = dao::_select<User>()
    .filter(uf.score > 100)
    .with(_limit(5, 20))
    .build().list();
```

`SqlServer`中不支持使用`limit`，代替方法是调用`top()`函数，例如：

```cpp
//输出20条结果
User::Fields uf;
UserList user = dao::_select<User>()
    .filter(uf.score > 100)
    .build().top(20).list();
```

### 4.2 order by

`_orderBy()`函数用于排序，同sql语句`order by`，如下：

```cpp
User::Fields uf;
UserList user = dao::_select<User>()
    .filter(uf.score > 100)
    .with(_orderBy(uf.name, uf.age))
    .build().list();
```

使用字段的`desc()`函数可倒序排序：

```cpp
User::Fields uf;
UserList user = dao::_select<User>()
    .filter(uf.score > 100)
    .with(_orderBy(uf.name.desc(), uf.age))
    .build().list();
```

### 4.3 group by / having

聚合函数`_groupBy()`用于对查询结果进行分组，`_having()`函数用于设置分组结果的过滤条件，如下：

```cpp
User::Fields uf;
UserList user = dao::_select<User>()
    .with(_groupBy(uf.name, uf.age), _having(uf.score > 100))
    .build().list();

User::Fields uf;
UserList user = dao::_select<User>()
    .with(_groupBy(uf.name))
    .with(_having(_fun("count(%1) > 2").field(uf.id)))
    .build().list();
```

### 4.4 _constraint()

`_constraint()`函数用于连接限制条件。通常情况下，多个结果限制函数可以连续使用：

```cpp
User::Fields uf;
UserList user = dao::_select<User>()
    .with(_groupBy(uf.name), _limit(5))
    .build().list();
```

在需要临时存储限制条件时，使用`_constraint()`函数组合：

```cpp
UserList getUserNames(int limitSize) {
    User::Fields uf;

    auto condition = _groupBy(uf.name);
    if (limitSize > 0) {
        condition = _constraint(condition, _limit(limitSize));
    }

    return dao::_select<User>().with(condition).build().list();
}
```

## 5. 获取结果

调用`build()`之后就可以读取结果了，实际查询在其后的读取方式进行的。例如使用`one()/unique()`读取单个结果，使用`list()`读取多个结果。

### 5.1 读取单个结果

使用`one()/unique()`读取单个结果，结果自动转化为表对象，除了`column()`函数从结果中赋值，表对象其他字段将使用默认值（如果配置了）或随机值。如果查询没有结果，则使用表对象默认构造函数返回一个对象：

```cpp
User::Fields uf;
User user = dao::_select<User>()
            .column(uf.name, uf.score)
            .filter(uf.score > 100)
            .build().one();
```

`one()`与`unique()`函数的区别是，`one()`函数如果有多个结果则只取一个结果，`unique()`则会检查结果至多有一个，存在多个则抛出异常。

### 5.2 读取多个结果

使用`list()`函数获取多个结果：

```cpp
User::Fields uf;
UserList user = dao::_select<User>()
                .column(uf.name, uf.score)
                .filter(uf.score > 100)
                .build().list();
```

### 5.3 按列读取结果

Select查询的 `unique`、`one`、`list` 和Join查询的 `list` 函数添加了使用列提取结果的重载函数，例如`list`的声明如下：
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
User::Fields uf;
QList<QString> user = dao::_select<User>()
    .filter(uf.score > 100)
    .with(_limit(5, 20))
    .build().list(uf.name);

QList<QPair<QString, int>> user = dao::_select<User>()
    .filter(uf.score > 100)
    .with(_limit(5, 20))
    .build().list(uf.name, uf.age);

QList<std::tuple<QString, int, int>> user = dao::_select<User>()
    .filter(uf.score > 100)
    .with(_limit(5, 20))
    .build().list(uf.name, uf.age, uf.score);
```

按列读取结果与返回一个对象的查询相比，减少了对象的创建，通常情况下这并不会带来过多的效率提升，仅仅是简化了返回的结果集，在大量数据读取时考虑配合使用`column()`函数以提升查询效率。

### 5.4 读取表所有数据

使用以下代码可以将表所有数据读出：

```cpp
UserList users = dao::_selectAll<User>();
```

### 5.5 count计数

使用`dao::_count<T>()`函数对查询结果计数，查询条件的使用与`dao::_select<T>()`函数一致。对于`_count`函数，其返回类型为`CountBuilder<E>`，直接调用`count()`函数读取结果记数，其内部使用了`count(*)`作为查询列：

```cpp
User::Fields field;
int count = dao::_count<User>()
    .filter(field.score > 100)
    .count();
```

## 6 自定义结果处理

`QtDao`可以方便的将查询结果直接转换为实体对象列表，但对于查询结果数据量太大时，大量的时间会被花费在对象转换操作上。对于这种情况下，使用`column()`函数明确指定查询部分字段将大幅减少查询时间，即便如此，有时其转换效率也不尽如意。当不需要转换为对象，自主处理查询结果时，可以极大的减少查询时间。

```cpp
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