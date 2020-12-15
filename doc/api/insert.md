### 1.condition insert
```c++
SqliteTest1::Fields sf1;
bool success = dao::_insert<SqliteTest1>()
        .set(sf1.name = "test", sf1.number = 1)
        .build().insert();

auto ids = QList<qint64>() << 10 << 11;  //auto increment id set value will not work 
auto names = QStringList() << "name1" << "name2";
auto numbers = QList<int>() << 4 << 50;

bool success = dao::_insert<SqliteTest2>()
    .set(sf2.id = ids, sf2.number = numbers, sf2.name = names) //set value with sequence independent
    .build().insert();
```
### 2.object(s) insert
```c++
SqliteTest1 test1(2, "test", 40, "object");
bool success = dao::_insert<SqliteTest1>().build().insert(test1);

SqliteTest2List test;
test << SqliteTest2(0, "name1", 1, 11, 111);
test << SqliteTest2(0, "name2", 2, 22, 22.2);
test << SqliteTest2(0, "name3", 3, 33, "333");
bool success = dao::_insert<SqliteTest2>().build().insert(test);

SqliteTest2List test;
test << SqliteTest2(0, "name4", 4, 44, 444);
test << SqliteTest2(0, "name5", 5, 55, 55.2);
test << SqliteTest2(0, "name6", 6, 66, "666");
bool success = dao::_insert<SqliteTest2>().build().insert2(test);
```

### 3.insert or replace
```c++
auto entity = SqliteTest2(0, "testinsertorreplace", -1, -2, "666");
bool success = dao::_insert<SqliteTest2>().build().insertOrReplace(entity);
```

### 4.transcation
```c++
dao::transcation();
auto entity = SqliteTest2(0, "test transcation", 10000, 10000, "666");
dao::_insert<SqliteTest2>().build().insert(entity);
try {
    dao::_insert<SqliteTest2>().throwable().build().insert(entity);
    dao::commit();
}
catch (DaoException&) {
    dao::rollback();
}
```

### 5.insert into select
```c++
SqliteTest1::Fields sf1;
bool success = dao::_insertIntoSelect<SqliteTest2>()
    .column(sf2.name, sf2.number)
    .from(
        dao::_select<SqliteTest1>().column(sf1.name, sf1.number).filter(sf1.number < 12).build()
    )
    .build().insert();


SqliteTest1::Fields sf1;
SqliteTest2::Fields sf2;

auto join = dao::_join<SqliteTest1, SqliteTest2>()
    .column(sf1.name, _fun("%1 + 10 as number").field(sf1.number))
    .from<SqliteTest1>()
    .innerJoin<SqliteTest2>().on(sf2.name == sf1.name)
    .build();
bool success = dao::_insertIntoSelect<SqliteTest2>()
    .column(sf2.name, sf2.number)
    .from(join)
    .build().insert();
```