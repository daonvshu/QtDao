# 使用
#### 例如在sqlite中有如下3个表：
#### `User`表、`Position`表、`Address`表：

#### 1.配置dao_entity.xml:

```xml
<?xml version="1.0" encoding="utf-8"?>
<dao prefix="ts_">
  <tb name="User">
    <item name="id" qtype="qint64">integer primary key auto_increment</item>
    <item name="name" qtype="QString" index="true">varchar(10) not null</item>
    <item name="age" qtype="int">int default 0</item>
    <item name="sex" qtype="bool">int(1)</item>
    <item name="duty" qtype="QString">varchar(30)</item>
  </tb>
  <tb name="Position">
    <item name="id" qtype="qint64">integer primary key auto_increment</item>
    <item name="uid" qtype="int">varchar(233)</item>
    <item name="star" qtype="QString">varchar(233)</item>
    <item name="x" qtype="int">int default 0</item>
    <item name="y" qtype="int">int default 0</item>
  </tb>
  <tb name="Address">
    <item name="id" qtype="qint64">integer primary key auto_increment</item>
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
<dao>
  <version>1</version>
  <sqltype>sqlite</sqltype>
  <dbname>daotest</dbname>
  <dbhost></dbhost>
  <dbuname>root</dbuname>
  <dbpcc>root</dbpcc>
  <dbport></dbport>
</dao>
```
#### 2.使用[DbEntityGenerator](https://github.com/daonvshu/DbEntityGenerator)生成对应的3个头文件`User.h`,`Position.h`,`Address.h`

#### 3.使用DBCreator在main中进行初始化
```c++
ConnectionPool::loadConfigure(":/xxx/sql/dao_cfg.xml");
bool dbInitSucc = DbCreatorHelper::testConnect();
if (dbInitSucc) {
   DbCreator<User, Position, Address>::init(dbInitSucc);
}

if (!dbInitSucc) {
   QMessageBox::warning(nullptr, u8"提示", u8"数据库连接失败！", 0, 0);
   return 0;
}
```

#### 4.CURD
数据库语句的拼接靠DaoEntityField完成，通过重载后的运算符操作，目前重载过的运算符有：==、!=、>、>=、<、<=、%、+、-、*、/、++、--、()等
- ##### 查询
```c++
bool exist;
auto r = dao::_query<User>().wh(fu.name == "Alice" and fu.age >= 10).build().unique(exist);
```
```sql
select *from ts_user where name = 'Alice' and age >= 10
```
```c++
auto b = dao::_query<User>().wh(_B(fu.name.like("Alice%") and fu.age >= 10) or fu.sex == 1).build().list();
```
```sql
select *from ts_user where (name like 'Alice%' and age >= 10) or sex = 1
```
```c++
int count = dao::_count<User>().wh(fu.name.like("Bob%")).build().count();
```
```sql
select count(*) from ts_user where name like 'Bob%'
```
- ##### 插入
```c++
//1.
dao::_insert<User>().build().insert(User(0, u8"Alice", 12, 1, "student"));
//2.
UserList users;
for (int i = 0; i < 20; i++) {
   users << User(0, QString("Bob_") + ('a' + i), i, i % 2, "");
}
dao::_insert<User>().build().insertBatch(users);
```
```sql
insert into ts_user values(0, 'Alice', 12, 1, 'student')
```
- ##### 更新
```c++
//1.
dao::_update<User>().set((fu.age + 10, fu.duty == "teacher")).wh(fu.name == "Alice").build().update();//注意set条件有2个括号
//2.
User user(0, "Alice", 22, 1, "teacher");
dao::_update<User>().wh(fu.name == user.getName()).build().update(user);
//3.
dao::_update<User>().bind(fu.name).build().updateBy(user);
```
```sql
update ts_user set age = age + 10, duty = 'teacher' where name = 'Alice'
```
```c++
UserList users;
for (int i = 0; i < 20; i++) {
   users << User(0, QString("Bob_") + ('a' + i), 22, 0, "teacher");
}
dao::_update<User>().bind(fu.name).build().updateBy(users);
```
- ##### 删除
```c++
dao::_delete<User>().wh(fu.name == "Bob_a").build().deleteBy();
```
```sql
delete from ts_user where name = 'Bob_a'
```
```c++
QVariantList list;
list << 11 << 12 << 19;
dao::_delete<User>().wh(fu.age.in(list)).build().deleteBy();
```
```sql
delete from ts_user where age in (11,12,19)
```
- ##### join
```c++
User::Fields fu;
Position::Fields fp;
Address::Fields fa;
dao::bindJoinOrder(fu, fp, fa);
auto result = dao::_join()
   .bind(fu.name, fu.age, fu.duty).wh(fu.age > 10).jnull<User>()
   .bind(fp.star).wh(fp.uid == fu.id).jinner<Position>()
   .bind(fa.city, fa.street).wh(fa.uid == fu.id).jinner<Address>()
   .build().list();
for (auto& r : result) {
   User user;
   Position position;
   Address address;
   r.bind(user, position, address);//将值绑定
}
```
```sql
select a.name,a.age,a.duty,b.star,c.city from User a inner join Position b on b.uid = a.id inner join Address c on c.uid = a.id where a.age > 10
```