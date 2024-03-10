---
title: 外键约束
category: functions
layout: post
---

外键约束配置
-------------

外键约束通过xml配置文件中定义，例如下面的配置表：

```xml
<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE dao SYSTEM "../../../entity/sqlite_entity.xsd">
<dao prefix="ts_" db="sqlite">
    <tb name="Artist" declaremetatype="true">
        <item name="id" type="long" constraints="primary key" />
        <item name="name" type="text" />
        <item name="size" type="int" />
        <index type="unique index">
            <field>id</field>
            <field>size</field>
        </index>
    </tb>
    <tb name="Track">
        <item name="id" type="long" default="-1" constraints="primary key" />
        <item name="name" type="text" />
        <item name="pkArtist" type="long" reftb="Artist" refitem="id"
              refonupdate="cascade" refondelete="cascade" deferrable="true" />
    </tb>
    <tb name="Customer">
        <item name="id" type="long" default="-1" constraints="primary key" />
        <item name="name" type="text" />
        <item name="pkArtistId" type="long" />
        <item name="pkArtistSize" type="int" />
        <foreignkey reftb="Artist" refonupdate="cascade" refondelete="cascade">
            <field name="pkArtistId" refitem="id"/>
            <field name="pkArtistSize" refitem="size"/>
        </foreignkey>
    </tb>
</dao>
```

可在`<item>`标签中设置`<reftb>`、`<refitem>`等引用其他表字段，也可以使用`<foreignkey>`标签定义组合外键（目前仅支持单表组合外键）。需要注意的是，被引用字段需要保证唯一性。对于单字段外键来说，被引用字段需要定义为`primary key`或`unique`约束，对于组合外键来说，被引用字段需要创建一个组合唯一索引。另外需要注意的是，被引用的主表必须定义在前面，数据表创建是按照xml配置表顺序依次创建，如果主表不存在，创建外键约束时会导致失败。


延迟检查
-------------

使用`deferrable`属性添加延迟检查标志，目前仅`sqlite`支持。对于延迟检查，体现在事务处理时，插入错误的数据是否在commit时进行外键约束检查。例如下面的例子：

```cpp
int step = 0;
try {
    dao::transaction();

    Track track1(1, "Yesterday", 3);
    dao::_insert<Track>().build().insert(track1);

    step = 1;
    Track track2(2, "Stairway to Heaven", 2);
    dao::_insert<Track>().disableFatalMsg().build().insert(track2);

    step = 2;
    Track track3(3, "Another Brick in the Wall", 3);
    dao::_insert<Track>().build().insert(track3);

    step = 3;
    dao::commit();

    step = 4;
} catch (dao::DaoException&) {
    dao::rollback();
}
```

假如track2尝试插入错误的数据，如果未启用延迟检查（默认），在step=1时将触发DaoException导致事务回滚。当启用延迟检查时，在step=3时才会触发DaoException事务进行回滚。


附加查询函数
-------------

如上面例子中，在两个子表中配置了对`Artist`主表的外键引用，`vscode-qtdao`插件将为主表`Artist`创建两个通过被引用的字段读取子表数据的附加函数：
```cpp
class Artist {
public:
    //...

    TrackList getTrackById();

    CustomerList getCustomerByIdSize();

    //...
}
```

当我们查询出一个`Artist`数据时，可直接读取相关联的子表数据：

```cpp
Artist::Field af;
auto artist = dao::_select<Artist>().filter(af.id == 3).build().unique();
//检查数据存在
if (artist.id != -1) {
    //通过id读取track表数据
    auto tracks = artist.getTrackById();
    //通过id、size读取customer表数据
    auto customers = artist.getCustomerByIdSize();
}
```