# 使用
#### 例如在sqlite中有如下3个表：
#### `User`表、`Position`表、`Address`表：

#### 1.配置dao_entity.xml:

```xml
<?xml version="1.0" encoding="utf-8"?>
<dao prefix="ts_">
  <tb name="User">
    <item name="id" qtype="qint64" key="id">integer primary key auto_increment</item>
    <item name="name" qtype="QString" key="index">varchar(10) not null</item>
    <item name="age" qtype="int">int default 0</item>
    <item name="sex" qtype="bool">int(1)</item>
    <item name="duty" qtype="QString">varchar(30)</item>
  </tb>
  <tb name="Position">
    <item name="id" qtype="qint64" key="id">integer primary key auto_increment</item>
    <item name="uid" qtype="int">varchar(233)</item>
    <item name="star" qtype="QString">varchar(233)</item>
    <item name="x" qtype="int">int default 0</item>
    <item name="y" qtype="int">int default 0</item>
  </tb>
  <tb name="Address">
    <item name="id" qtype="qint64" key="id">integer primary key auto_increment</item>
    <item name="uid" qtype="int">int not null</item>
    <item name="province" qtype="QString">varchar(233)</item>
    <item name="city" qtype="QString">varchar(223)</item>
    <item name="street" qtype="QString">varchar(233)</item>
  </tb>
</dao>
```
配置dao_cfg.xml
```xml
<?xml version="1.0" encoding="utf-8"?>
<dao version="3" type="mysql" dbname="daotest">
  <dbhost>localhost</dbhost>
  <dbuname>root</dbuname>
  <dbpcc>root</dbpcc>
  <dbport>3306</dbport>
  <dboption>MYSQL_OPT_CONNECT_TIMEOUT=3;MYSQL_OPT_READ_TIMEOUT=3;MYSQL_OPT_WRITE_TIMEOUT=3</dboption>
</dao>
```
#### 2.使用[DbEntityGenerator](https://github.com/daonvshu/DbEntityGenerator)生成对应的3个头文件`User.h`,`Position.h`,`Address.h`

#### 3.在main中进行初始化
```c++
ConnectionPool::loadConfigure(":/xxx/sql/dao_cfg.xml");
bool dbInitSucc = DbCreatorHelper::testConnect();
if (dbInitSucc) {
   DbTablesInit(dbInitSucc);
}

if (!dbInitSucc) {
   QMessageBox::warning(nullptr, u8"提示", u8"数据库连接失败！", 0, 0);
   return 0;
}
```

#### 4.如何使用
- 一个查询
```c++
User::Fields fu;
bool exist;
User user = dao::_query<User>().wh(fu.name == "Alice" and fu.age >= 10).build().unique(exist);
```
等价于
```sql
select *from ts_user where name = 'Alice' and age >= 10
```
更多的实例看example文件夹