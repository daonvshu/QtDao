## qt database object library

这是一个数据库查询与对象转换的操作库，支持基本增删改查操作，它能简单的将查询结果转换为定义好的类实例，基本原理是通过代码生成器和模板配合进行转换，下面是一个简单的示例展示了如何查询一个结果
```c++
Test1::Fields sf1;
Test1 d1 = dao::_select<Test1>()
        .filter(sf1.name == "client", _or(sf1.number == 12, sf1.name == "bob"))
        .build().one();
```
等价于下面的sql语句：
```sql
select *from test1 where name='client' and (number=12 or name='bob')
```

下面是当前受支持的数据库
- [x] sqlite
- [x] mysql
- [x] sqlserver

## 如何使用
[https://daonvshu.github.io/QtDao/](https://daonvshu.github.io/QtDao/)