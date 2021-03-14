## qt database object library

这是一个数据库查询与对象转换的操作库，支持基本增删改查操作，它能简单的将查询结果转换为定义好的类实例，基本原理是通过代码生成器[DbEntityGenerator](https://github.com/daonvshu/DbEntityGenerator)和模板配合进行转换，下面是一个简单的示例展示了如何查询一个结果
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
下载[master分支](https://github.com/daonvshu/QtDao)，打开工程（QtDao.sln/qtdao.pro）使用当前qt5库编译生成静态库（qtdao.lib），使用步骤[看这](https://github.com/daonvshu/QtDao/blob/master/doc/setup/setup.md)

## 支持的功能

- **数据库初始化**
  - 检查连接
  - 自动创建数据库/数据表
  - 检查版本并升级
- **日志**
  - 打印执行的sql语句
  - 打印执行的sql值列表
- **连接池**
  - 多线程查询
  - 连接名复用
- **查询**
  - insert([demo](https://github.com/daonvshu/QtDao/blob/master/doc/api/insert.md))
    - 对象（/批量）插入
    - 部分值插入
    - 插入或替换(insert or replace)
    - 使用查询结果插入(insert into select)
  - select([demo](https://github.com/daonvshu/QtDao/blob/master/doc/api/select.md))
    - 对象查询
    - 联合(union)
    - join
    - 递归查询(recursive)
    - 子查询（嵌套查询）(~~mysql~~)
    - 自定义语句查询
    - 计数(count)
  - update([demo](https://github.com/daonvshu/QtDao/blob/master/doc/api/update.md))
    - 条件更新
    - 对象（/批量）更新
  - delete([demo](https://github.com/daonvshu/QtDao/blob/master/doc/api/delete.md))
    - 条件删除
    - 对象（/批量）删除
  - 其他
    - 事务
    - 解释
    - sqlite写同步