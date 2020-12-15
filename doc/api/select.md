### 1.object(s) select
```c++
SqliteTest1::Fields sf1;

//select one object
auto d1 = dao::_select<SqliteTest1>()
    .filter(sf1.name == "client", _or(sf1.number == 12, sf1.name == "bob"))
    .build().one();

//unique
auto d2 = dao::_select<SqliteTest1>()
    .filter(sf1.id <= 3)
    .with(_orderBy(sf1.name.desc()), _limit(1))
    .build().unique();

//throwable
try {
    auto d3 = dao::_select<SqliteTest1>()
        .filter(sf1.number == 12)
        .throwable().build().unique();
}
catch (DaoException&) {
}

//list objects
auto d4 = dao::_select<SqliteTest1>()
    .filter(sf1.number == 12)
    .build().list();

//select all
auto d5 = dao::_selectAll<SqliteTest1>();
```

### 2.function select
```c++
SqliteTest1::Fields sf1;
auto data = dao::_select<SqliteTest1>()
    .column(sf1.id, _fun("sum(%1) + ? as sumnumber").field(sf1.number).value(5), sf1.name)
    .filter(_or(sf1.name.like("a%"), _fun("%1 > case when %2 = ? then ? else ? end").field(sf1.number, sf1.name).value("client", 12, 10)))
    .build().one();

//function with sub query
auto data = dao::_select<SqliteTest1>()
    .filter(_fun("%1 in %2").field(sf1.number).from(
        dao::_select<SqliteTest2>().column(sf2.number).filter(sf2.number < 15).build()
    ))
    .with(_orderBy(sf1.number))
    .build().list();
```

### 3.count
```c++
int count = dao::_count<SqliteTest1>()
    .filter(sf1.name.like("%l%")).count();
```

### 4.join
```c++
//three table join
auto result = dao::_join<SqliteTest1, SqliteTest2, SqliteTest3>()
    .column(sf2.name, _fun("sum(%1) as sumn").field(sf1.number))
    .from<SqliteTest1>()
    .innerJoin<SqliteTest2>().on(sf2.id == sf3.tbi2)
    .innerJoin<SqliteTest3>().on(sf3.tbi1 == sf1.id)
    .with(_groupBy(sf2.name), _orderBy(sf2.name))
    .build().list();
for (const auto& r : result2) {
    int sum = std::get<0>(r).__getExtra("sumn").toInt();
    SqliteTest2 test2 = std::get<1>(r);
}

//join self
class SqliteTest2Tmp : public dao::self<SqliteTest2> {};
SqliteTest2::Fields sf1;
SqliteTest2Tmp::Fields sf2;
auto result = dao::_join<SqliteTest2, SqliteTest2Tmp>()
    .column(sf1.name, sf2.name)
    .from<SqliteTest2>()
    .innerJoin<SqliteTest2Tmp>().on(sf2.number2 == sf1.number)
    .build().list();

class SqliteTest2Tmp2 : public dao::self<SqliteTest2Tmp> {};
SqliteTest2Tmp2::Fields sf3;
auto result2 = dao::_join<SqliteTest2, SqliteTest2Tmp, SqliteTest2Tmp2>()
    .column(sf1.name, sf2.name, sf3.name)
    .from<SqliteTest2>()
    .with(_orderBy(sf1.name.desc()))
    .innerJoin<SqliteTest2Tmp>().on(sf2.number2 == sf1.number)
    .innerJoin<SqliteTest2Tmp2>().on(sf3.number2 == sf2.number)
    .build().list();
```

### 5.select from select
```c++
SqliteTest1::Fields sf1;
SqliteTest2::Fields sf2;
SqliteTest3::Fields sf3;

//select from select
auto select = dao::_select<SqliteTest1>().filter(sf1.number > 10).build();
try {
    auto select1 = dao::_select<SqliteTest1>()
        .from(select)
        .filter(sf1.name.like("%b%"))
        .throwable()
        .build();

    //from select
    auto result = select1.unique();

    //count from (select from select)
    auto count = dao::_count<SqliteTest1>()
        .from(select1)
        .throwable()
        .filter(sf1.number != 0)
        .count();
}
catch (DaoException& e) {
}

//select from join
auto join1 = dao::_join<SqliteTest1, SqliteTest2, SqliteTest3>()
    .column(sf2.name, sf1.name, sf3.name, sf1.number, sf2.number)
    .from<SqliteTest1>()
    .innerJoin<SqliteTest2>().on(sf2.id == sf3.tbi2)
    .innerJoin<SqliteTest3>().on(sf3.tbi1 == sf1.id)
    .build();

try {
    auto count1 = dao::_count<SqliteTest1>()
        .from(join1)
        .filter(sf1.number > 10)
        .count();
    QCOMPARE(count1, 3);
}
catch (DaoException& e) {
}

//join from select
auto select = dao::_select<SqliteTest1>().filter(sf1.name == "client").build();
//from
auto join = dao::_join<SqliteTest1, SqliteTest2, SqliteTest3>()
    .column(sf2.name, sf1.name, sf3.name, sf1.number, sf2.number)
    .from(select)
    .innerJoin<SqliteTest2>().on(sf2.id == sf3.tbi2)
    .innerJoin<SqliteTest3>().on(sf3.tbi1 == sf1.id)
    .build().list();
//on
auto join = dao::_join<SqliteTest1, SqliteTest2, SqliteTest3>()
    .column(sf2.name, sf1.name, sf3.name, sf1.number, sf2.number)
    .from<SqliteTest1>()
    .innerJoin(select).on(sf2.id == sf3.tbi2)
    .innerJoin<SqliteTest3>().on(sf3.tbi1 == sf1.id)
    .build().list();
```

### 6.union
```c++
SqliteTest1::Fields sf1;
SqliteTest2::Fields sf2;

//select union select
auto select1 = dao::_select<SqliteTest1>()
    .column(sf1.name, sf1.number)
    .filter(sf1.number >= 12)
    .with(_orderBy(sf1.number))
    .build();

auto select2 = dao::_select<SqliteTest2>()
    .column(sf2.name, sf2.number)
    .filter(sf2.number >= 12)
    .unionSelect(select1)
    .build().list();

//select union join
auto join = dao::_join<SqliteTest1, SqliteTest2, SqliteTest3>()
    .column(sf1.name, sf2.number)
    .from<SqliteTest1>()
    .innerJoin<SqliteTest2>().on(sf2.id == sf3.tbi2)
    .innerJoin<SqliteTest3>().on(sf3.tbi1 == sf1.id)
    .build();

auto select = dao::_select<SqliteTest1>()
    .column(sf1.name, sf1.number)
    .filter(sf1.name == "client")
    .unionSelect(join, true)
    .build().list();

//join union select
auto join = dao::_join<SqliteTest1, SqliteTest2, SqliteTest3>()
    .column(sf1.name, sf2.number)
    .from<SqliteTest1>()
    .innerJoin<SqliteTest2>().on(sf2.id == sf3.tbi2)
    .innerJoin<SqliteTest3>().on(sf3.tbi1 == sf1.id)
    .unionSelect(select)
    .build().list();

//join union join
auto join2 = dao::_join<SqliteTest1, SqliteTest2, SqliteTest3>()
    .column(sf1.name, sf2.number)
    .from<SqliteTest1>()
    .filter(sf1.number >= 12)
    .innerJoin<SqliteTest2>().on(sf2.id == sf3.tbi2)
    .innerJoin<SqliteTest3>().on(sf3.tbi1 == sf1.id)
    .unionSelect(join1
    .build().list();
```

### 7.recursive
```c++
class TmpTest2 : public dao::self<SqliteTest2> {};

SqliteTest1::Fields sf1;
SqliteTest2::Fields sf2;
SqliteTest3::Fields sf3;
TmpTest2::Fields sfs2;

auto recursive = dao::_recursive()
    .tmp<TmpTest2>()
    .initialSelect(
        dao::_select<SqliteTest2>().filter(sf2.number == 50).build()
    )
    .recursiveSelect(
        dao::_join<SqliteTest2, TmpTest2>()
            .columnAll<SqliteTest2>()
            .from<SqliteTest2>()
            .innerJoin<TmpTest2>().on(sfs2.number2 == sf2.number)
            .build()
    );

//select from recursive
auto select = dao::_select<TmpTest2>().from(recursive)
    .column(sfs2.number).with(_orderBy(sfs2.number)).build().list();

//join from recursive
auto join2 = dao::_join<SqliteTest1, TmpTest2, SqliteTest3>()
    .column(sfs2.name, sf1.name, sf3.name, sf1.number, sfs2.number)
    .from<SqliteTest1>()
    .innerJoin(recursive).on(sfs2.id == sf3.tbi2)
    .innerJoin<SqliteTest3>().on(sf3.tbi1 == sf1.id, sf3.name.like("client%"))
    .build().list();
```

### 8.raw query
```c++
SqliteTest1::Fields sf1;
dao::_select<SqliteTest1>()
    .filter(sf1.number == 14)
    .build().raw([&](QSqlQuery& query) {
    while (query.next()) {
        //...
    }
});
```

### 9.custom query
```c++
BaseQuery::queryPrimitive("select 10 + 20", [&](QSqlQuery& query) {
    if (query.next()) {
        int result = query.value(0).toInt();
    }
}, [&](QString err) {
    //...
});
```