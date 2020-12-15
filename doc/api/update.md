### 1.condition update
```c++
SqliteTest1::Fields sf1;
bool success = dao::_update<SqliteTest1>()
    .set(sf1.number = 18, sf1.hex = "test update")
    .filter(sf1.name == "bob")
    .build().update();

QList<qreal> numbersSet;
numbersSet << 21 << 22;
QList<QByteArray> hexSet;
hexSet << "test1" << "test2";
QStringList targets;
targets << "bob" << "client";

success = dao::_update<SqliteTest1>()
    .set(sf1.number = numbersSet, sf1.hex = hexSet)
    .filter(sf1.name == targets)
    .build().updateBatch();
```

### 2.object(s) update
```c++
SqliteTest2::Fields sf2;
auto t2test = dao::_select<SqliteTest2>().filter(sf2.name == "bob").build().unique();
t2test.setNumber(50);
bool success = dao::_update<SqliteTest2>().build().update(t2test);

auto t2stest = dao::_select<SqliteTest2>().filter(sf2.number < 100).with(_orderBy(sf2.id)).build().list();
for (auto& t2 : t2stest) {
    t2.setVarianttype("testt2s");
}
success = dao::_update<SqliteTest2>().build().updateBatch(t2stest);
```