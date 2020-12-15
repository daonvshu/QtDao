### 1.condition delete
```c++
SqliteTest1::Fields sf1;
bool success = dao::_delete<SqliteTest1>().filter(sf1.name.like("a%")).build().deleteBy();

QStringList target;
target << "bob" << "client";
success = dao::_delete<SqliteTest1>().filter(sf1.name == target).build().deleteBatch();
```

### 2.object(s) delete
```c++
SqliteTest2::Fields sf2;
auto result1 = dao::_select<SqliteTest2>().filter(sf2.name == "joker").build().unique();
bool success = dao::_delete<SqliteTest2>().build().deleteBy(result1);

auto result2 = dao::_selectAll<SqliteTest2>();
success = dao::_delete<SqliteTest2>().build().deleteBatch(result2);
```

### 3.truncate
```c++
dao::_truncate<SqliteTest2>();
```