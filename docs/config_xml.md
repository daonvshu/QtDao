---
sidebar_position: 3
---

# 数据表配置

数据库表与字段等信息的配置通过xml文档编写，再通过代码生成器插件生成c++对应的模型和工具类。编写表配置文件时，需要严格遵循相应数据库的xsd文档约束。本篇详细说明了各个xml标记的作用与限制条件，按照文档约束编写创建符合业务的数据库模型。配置文件模板可以在项目根文件夹`entity`里面查看。

## `<dao>`标签

根节点`<dao>`用于选择数据库类型，示例：  

```xml
<?xml version="1.0" encoding="utf-8"?>
<dao prefix="ts_" db="sqlite" namespace="MySpc" alias="dbconfig1">
</dao>
```

|属性|说明|是否可选|
|:--:|:--|:--|
|`prefix`|表示数据表前缀，每个表名称前都将添加`prefix`前缀用于区分其他表（如系统表），在使用QtDao查询时不必关心前缀，在生成sql语句时将自动使用带有前缀的表名。|
|`db`|表示使用的数据库类型，目前只能为4种：`sqlite`,`mysql`,`sqlserver`,`psql`。|
|`namespace`|对每个生成的实体类添加一个命名空间。|可选|
|`alias`|为当前配置绑定一个别名，可在多数据连接初始化时进行配置的选择。|可选|


## `<tb>`标签

`<tb>`标签用于配置表属性，示例：
```xml
<?xml version="1.0" encoding="utf-8"?>
<dao prefix="ts_" db="sqlite">
    <tb name="TestTb" declaremetatype="true" engine="InnoDB" createmanual="false">
    </tb>
</dao>
```

|属性|说明|是否可选|
|:--:|:--|:--|
|`name`|声明数据库表表名，建议使用大写驼峰命名法，生成的c++模型类类名也使用该名称。|
|`declaremetatype`|将该c++类注册到qt元对象系统，在使用信号槽等功能时传递该自定义类型。同时，该类各个字段注册到qml类型系统，在传递到qml中使用时可以直接修改和访问字段（成员变量）值。|可选|
|`engine`|`mysql`特有属性，显式声明mysql数据库的数据表引擎。|可选|
|`createmanual`|手动创建数据表。如果为true，数据库初始化时不会自动创建该表。用于手动编写代码创建一些不支持的表类型，例如虚拟表。|可选|


## `<item>`标签

`<item>`标签用于配置数据表字段信息，示例：

```xml
<?xml version="1.0" encoding="utf-8"?>
<dao prefix="ts_" db="mysql">
    <tb name="TestTb">
        <item name="id" type="bigint" default="-1" autoincrement="true" constraints="primary key" note="主键"/>
        <item name="name" type="varchar" bitsize="100" constraints="not null" jsonkey="json_name" jsontimeformat="yyyy-MM"/>
        <item name="nametmp" type="text" transient="true"/>
        <item name="typedecimal" type="decimal" bitsize="8" decimal-d="4" default="0.3" />
        <item name="myStructType" type="custom" customtype="MyStruct">
    </tb>
</dao>
```

|属性|说明|是否可选|
|:--:|:--|:--|
|`name`|声明数据表字段名称，建议使用小写驼峰命名法，生成的c++模型成员变量也使用该名称。|
|`type`|声明字段类型，该类型必须是当前数据库支持的类型，不同数据库有不同的可选类型，见下面[类型与默认值](/QtDao/docs/config_xml#数据库类型与默认值说明)说明表描述。|
|`customtype`|当`type`值为`custom`时有效，表示使用自定义类型，该字段的使用见[自定义操作篇](/QtDao/docs/custom#自定义类型)说明。|可选|
|`default`|设置字段和c++模型成员变量默认值，不同数据库有不同程度的默认值支持。同时，当查询单个实例结果时，设置字段默认值是个重要的判断条件，见下面类型与默认值说明表描述。|可选|
|`autoincrement`|声明自增长字段。如果类型具有自增属性（例如psql中的serial类型），不需要设置该属性。|可选|
|`constraints`|为字段添加限制，只能为以下3种：`'not null'`限制字段不能存储null类型值，`'unique'`限制字段不可重复，`'primary key'`声明字段为主键。|可选|
|`bitsize`|设置类型宽度，对应字符串类似于`varchar(100)`，浮点数类似于`decimal(5,3)`。|可选|
|`decimal-d`|配合`bitsize`设置浮点数小数位宽度，类似于`decimal(5,3)`，其中bitsize=5, decimal-d=3。|可选|
|`note`|设置字段备注，在c++模型成员变量上添加相应注释，对于`mysql`还将作为comment写入到表结构中。|可选|
|`jsonkey`|将c++模型类与json相互转换时显示指定转换的key，默认使用字段名。|可选|
|`jsontimeformat`|与json相互转换时，时间日期相关类型显示使用格式化字符串。|可选|
|`transient`|c++模型临时成员变量，将不参与数据库数据转换，用于保存用户临时数据。|可选|
|`reftb`|当前字段作为外键链接到的主表表名称。|可选|
|`refitem`|当前字段作为外键链接的目标字段。|可选|
|`refonupdate`|当前字段作为外键，其链接的主表字段进行更新时的表现行为。可选的有：`'no_action'`,`'restrict'`,`'set_null'`,`'set_default'`,`'cascade'`。|可选|
|`refondelete`|当前字段作为外键，其链接的主表字段进行删除时的表现行为。|可选|
|`deferrable`|当前字段作为外键，事务操作时是否延迟检查外键约束，目前仅sqlite支持。|可选|
|`refvirtual`|虚拟外键引用。|可选|

:::note
注意：当使用外键约束时，其链接目标字段需要添加primary key或unique约束以保证唯一性。
:::

## `<index>`标签

`<index>`标签用于配置数据表索引，示例：

```xml
<?xml version="1.0" encoding="utf-8"?>
<dao prefix="ts_" db="sqlserver">
    <tb name="TestTb">
        <item name="id" type="bigint" default="-1" constraints="primary key" autoincrement="true" note="自增长主键"/>
        <item name="name" type="varchar" constraints="not null"/>
        <item name="number" type="int" default="0"/>
        <item name="number2" type="int"/>
        <index type="unique nonclustered">
            <field seq="asc">name</field>
            <field seq="desc">number</field>
        </index>
        <index>
            <field>number2</field>
        </index>
        <index allow_row_locks="on" allow_page_locks="off" data_compression="none">
            <field>number</field>
            <field>number2</field>
        </index>
    </tb>
</dao>
```

`<index>`可指定`type`属性，对于sqlite/mysql/psql，可以选择使用`'index'`（默认）或`'unique index'`，用于指定索引唯一限制，对于`sqlserver`，可选择使用的有：`'clustered'`、`'unique clustered'`、`'nonclustered'`（默认）、`'unique nonclustered'`同时显式指定唯一与聚集性限制。`allow_row_locks`、`allow_page_locks`、`data_compression`属性为`sqlserver`特有的属性。使用`<field>`标签指定创建索引用到的字段。


## `<foreignkey>`标签

`<foreignkey>`标签用于配置组合外键约束，示例：

```xml
<?xml version="1.0" encoding="utf-8"?>
<dao prefix="ts_" db="sqlserver">
    <tb name="Artist" declaremetatype="true">
        <item name="id" type="long" constraints="primary key" />
        <item name="name" type="text" />
        <item name="size" type="int" />
        <index type="unique index">
            <field>id</field>
            <field>size</field>
        </index>
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

与字段定义为外键时一样，`<foreignkey>`需要指定链接的主表表名和更新删除时的行为。使用`<field>`标签定义本表字段（`<name>`）与主表字段（`<refitem>`）的链接关系。目前仅支持单表的组合外键约束的定义。与单字段外键约束一样，被引用的外键字段需要保证唯一性，需要创建一个唯一索引。


## `<constructor>`标签

`<constructor>`标签用于创建c++模型类构造函数，示例：

```xml
<?xml version="1.0" encoding="utf-8"?>
<dao prefix="ts_" db="sqlserver">
    <tb name="TestTb">
        <item name="id" type="bigint" default="-1" constraints="primary key" autoincrement="true" note="自增长主键"/>
        <item name="name" type="varchar" constraints="not null"/>
        <item name="number" type="int" default="0"/>
        <item name="number2" type="int"/>
        <constructor>
            <field>name</field>
            <field>number</field>
        </constructor>
    </tb>
</dao>
```

通常情况下会产生3种默认构造函数：无参构造函数、全参数构造函数，忽略自增主键、无默认值参数构造函数，当默认生成的构造函数无法满足需求时可以显示声明构造函数，此时应该注意无默认值的成员会变成随机值的可能。生成的模型类构造函数如下：
```cpp
class TestTb {
private:
    //
    qint64 id;
    //
    QString name;
    //
    int number;
    //
    int number2;

public:
    //无参构造函数
    TestTb() {
        id = -1;
        number = 0;
    }

    //全参数构造函数，忽略自增主键
    TestTb(
        const QString& name,
        const int& number,
        const int& number2
    ) : name(name)
    , number(number)
    , number2(number2)
    {
        id = -1;
    }

    //无默认值参数构造函数
    TestTb(
        const QString& name,
        const int& number2
    ) : name(name)
    , number2(number2)
    {
        id = -1;
        number = 0;
    }

    //显示声明构造函数
    TestTb(
        const QString& name,
        const int& number
    ) : name(name)
    , number(number)
    {
        id = -1;
    }
}
```

## 数据库类型与默认值说明

#### Sqlite

|字段类型|c++类型|
|:--|:--|
|`int`|int|
|`long`|qint64|
|`real`|qreal|
|`text`|QString|
|`blob`|QByteArray|
|`variant`|QVariant|

默认值示例：

> 数值类型

```xml
<!-- sql：id integer default -1 -->
<!-- c++：int id = -1; -->
<item name="id" type="int" default="-1">

<!-- sql: id integer default null -->
<!-- c++: qint64 id = qint64(); -->
<item name="id" type="long" default="null">

<!-- sql: num real default 0.1 -->
<!-- c++: qreal num = 0.1; -->
<item name="num" type="real" default="0.1">

<!-- sql: num real default null -->
<!-- c++: qreal num = qreal(); -->
<item name="num" type="real" default="null">
```

> 字符串/字节数组类型

```xml
<!-- sql: str text default 'abc' -->
<!-- c++: QString str = "abc"; -->
<item name="str" type="text" default="abc">

<!-- sql: str text default null -->
<!-- c++: QString str = QString(); -->
<item name="str" type="text" default="null">

<!-- sql: str text default 'null' -->
<!-- c++: QString str = QString("null"); -->
<item name="str" type="text" default='"null"'>

<!-- sql: str text default '' -->
<!-- c++: QString str = QString(); -->
<item name="str" type="text" default="empty">

<!-- sql: str text default 'string' -->
<!-- c++: QString str = QString("string"); -->
<item name="str" type="text" default='QString("string")'>

<!-- blob 默认值等同于 text, c++类型 QString->QByteArray -->
```

> variant类型

variant类型使用blob存储QVariant类型的数据，其声明的字段支持QVariant可存储的大部分类型，读取与写入通过QVariant隐式转换，因此通过variant字段可以存储不同的类型的数据，但仅限于QVariant支持的类型，使用自定义类型时可参考[自定义操作篇](/QtDao/docs/custom#自定义类型)说明。
```xml
<!-- 默认值仅初始化QVariant，sql默认值null -->
<item name="num" type="variant" default="100"/>
<item name="str" type="variant" default='"string"'/>
<item name="t" type="variant" default="QTime::currentTime()"/>
<item name="dat" type="variant" default="QDateTime::currentDateTime()"/>
```

#### Mysql

|字段类型|c++类型|
|:--|:--|
|`tinyint`|char|
|`smallint`|short|
|`mediumint/int`|int|
|`bigint`|qint64|
|`float/double/decimal`|qreal|
|`time`|QString|
|`date`|QDate|
|`datetime/timestamp`|QDateTime|
|`char`|QChar|
|`varchar`|QString|
|`tinytext/text/`<br/>`mediumtext/longtext`|QString|
|`tinyblob/blob/`<br/>`mediumblob/longblob`|QByteArray|

默认值示例：

> 数值/字符串/字节数组类型

同`sqlite`默认值示例

> 时间/日期类型

```xml
<!-- time -->

<!-- sql: typeTime time default '120:59:59' -->
<!-- c++: QString typeTime = "120:59:59"; -->
<item name="typeTime" type="time" default='120:59:59'>

<!-- sql: typeTime time default null -->
<!-- c++: QString typeTime = QTime::currentTime().toString("HH:mm:ss"); -->
<item name="typeTime" type="time" default='QTime::currentTime().toString("HH:mm:ss")'>

<!-- sql: typeTime time default null -->
<!-- c++: QString typeTime = QTime::currentTime().toString("HH:mm:ss"); -->
<item name="typeTime" type="time" default='now'>

<!-- sql: typeTime time default null -->
<!-- c++: QString typeTime = QString(); -->
<item name="typeTime" type="time" default='null'>


<!-- date -->

<!-- sql: typeDate date default '2020-01-01' -->
<!-- c++: QDate typeDate = QDate::fromString("2020-01-01"); -->
<item name="typeDate" type="date" default='2020-01-01'>

<!-- sql: typeDate date default null -->
<!-- c++: QDate typeDate = QDate::currentDate(); -->
<item name="typeDate" type="date" default='QDate::currentDate()'>

<!-- sql: typeDate date default null -->
<!-- c++: QDate typeDate = QDate::currentDate(); -->
<item name="typeDate" type="date" default='now'>

<!-- sql: typeDate date default null -->
<!-- c++: QDate typeDate = QDate(); -->
<item name="typeDate" type="date" default='null'>


<!-- datetime/timestamp -->

<!-- sql: typeDatetime datetime default '2020-01-01 12:59:58.233' -->
<!-- c++: QDateTime typeDatetime = QDateTime::fromString("2020-01-01 12:59:58.233"); -->
<item name="typeDatetime" type="datetime" default='2020-01-01 12:59:58.233'>

<!-- sql: typeDatetime datetime default null -->
<!-- c++: QDateTime typeDatetime = QDateTime::currentDataTime(); -->
<item name="typeDatetime" type="datetime" default='QDateTime::currentDataTime()'>

<!-- sql: typeDatetime datetime default null -->
<!-- c++: QDateTime typeDatetime = QDateTime(); -->
<item name="typeDatetime" type="datetime" default='null'>

<!-- sql: typeDatetime datetime default CURRENT_TIMESTAMP -->
<!-- c++: QDateTime typeDatetime = QDateTime::currentDataTime(); -->
<item name="typeDatetime" type="timestamp" default='now'>

<!-- sql: typeDatetime datetime default CURRENT_TIMESTAMP(3) -->
<!-- c++: QDateTime typeDatetime = QDateTime::currentDataTime(); -->
<item name="typeDatetime" type="timestamp" default='CURRENT_TIMESTAMP(3)'>

<!-- sql: typeDatetime datetime default CURRENT_TIMESTAMP(3) ON UPDATE CURRENT_TIMESTAMP(3) -->
<!-- c++: QDateTime typeDatetime = QDateTime::currentDataTime(); -->
<item name="typeDatetime" type="timestamp" default='CURRENT_TIMESTAMP(3) ON UPDATE CURRENT_TIMESTAMP(3)'>

<!-- sql: typeDatetime datetime default null ON UPDATE CURRENT_TIMESTAMP(3) -->
<!-- c++: QDateTime typeDatetime = QDateTime(); -->
<item name="typeDatetime" type="timestamp" default='null ON UPDATE CURRENT_TIMESTAMP(3)'>
```

#### SqlServer

|字段类型|c++类型|
|:--|:--|
|`tinyint`|uchar|
|`smallint`|short|
|`int`|int|
|`bigint`|qint64|
|`float/double/decimal/` <br/> `numeric/real`|qreal|
|`time`|QTime|
|`date`|QDate|
|`datetime/datetime2/`<br/>`datetimeoffset`|QDateTime|
|`timestamp`|QByteArray|
|`char/varchar/`<br/>`varchar(max)/nchar/nvarchar/`<br/>`nvarchar(max)/text/ntext`|QString|
|`bit`|bool|
|`binary/varbinary/`<br/>`varbinary(max)`|QByteArray|
|`sql_variant`|QVariant|
|`uniqueidentifier/xml`|QByteArray|

默认值示例：

> 数值/字符串/字节数组类型

同`sqlite`默认值示例

> 时间/日期类型

```xml
<!-- time -->

<!-- sql: typeTime time default '12:59:59.6789' -->
<!-- c++: QTime typeTime = QTime::fromString("12:59:59.6789"); -->
<item name="typeTime" type="time" default='12:59:59.6789'>

<!-- sql: typeTime time default null -->
<!-- c++: QTime typeTime = QTime::currentTime(); -->
<item name="typeTime" type="time" default='QTime::currentTime()'>

<!-- sql: typeTime time default getdate() -->
<!-- c++: QTime typeTime = QTime::currentTime(); -->
<item name="typeTime" type="time" default='now'>

<!-- sql: typeTime time default null -->
<!-- c++: QTime typeTime = QTime(); -->
<item name="typeTime" type="time" default='null'>


<!-- date -->

<!-- sql: typeDate date default '2020-01-01' -->
<!-- c++: QDate typeDate = QDate::fromString("2020-01-01"); -->
<item name="typeDate" type="date" default='2020-01-01'>

<!-- sql: typeDate date default null -->
<!-- c++: QDate typeDate = QDate::currentDate(); -->
<item name="typeDate" type="date" default='QDate::currentDate()'>

<!-- sql: typeDate date default getdate() -->
<!-- c++: QDate typeDate = QDate::currentDate(); -->
<item name="typeDate" type="date" default='now'>

<!-- sql: typeDate date default null -->
<!-- c++: QDate typeDate = QDate(); -->
<item name="typeDate" type="date" default='null'>


<!-- datetime/datetime2/datetimeoffset -->

<!-- sql: typeDatetime datetime default '2020-01-01 12:59:58.233' -->
<!-- c++: QDateTime typeDatetime = QDateTime::fromString("2020-01-01 12:59:58.233"); -->
<item name="typeDatetime" type="datetime" default='2020-01-01 12:59:58.233'>

<!-- sql: typeDatetime datetime default null -->
<!-- c++: QDateTime typeDatetime = QDateTime::currentDataTime(); -->
<item name="typeDatetime" type="datetime" default='QDateTime::currentDataTime()'>

<!-- sql: typeDatetime datetime default null -->
<!-- c++: QDateTime typeDatetime = QDateTime(); -->
<item name="typeDatetime" type="datetime" default='null'>

<!-- sql: typeDatetime datetime default getdate() -->
<!-- c++: QDateTime typeDatetime = QDateTime::currentDataTime(); -->
<item name="typeDatetime" type="timestamp" default='now'>
```

#### PostgreSQL

|字段类型|c++类型|
|:--|:--|
|`smallint/smallserial`|short|
|`integer/serial`|int|
|`bigint/bigserial`|qint64|
|`real/numeric/decimal/` <br/> `double precision`|qreal|
|`time`|QTime|
|`date`|QDate|
|`timestamp`|QDateTime|
|`interval`|QString|
|`varchar/text`|QString|
|`bit`|char|
|`char`|QChar|
|`boolean`|bool|
|`bytea`|QByteArray|

默认值示例：

> 数值/字符串/字节数组类型

同`sqlite`默认值示例

> 时间/日期类型

```xml
<!-- time -->

<!-- sql: typeTime time default '12:59:59.6789' -->
<!-- c++: QTime typeTime = QTime::fromString("12:59:59.6789"); -->
<item name="typeTime" type="time" default='12:59:59.6789'>

<!-- sql: typeTime time default null -->
<!-- c++: QTime typeTime = QTime::currentTime(); -->
<item name="typeTime" type="time" default='QTime::currentTime()'>

<!-- sql: typeTime time default CURRENT_TIME -->
<!-- c++: QTime typeTime = QTime::currentTime(); -->
<item name="typeTime" type="time" default='now'>

<!-- sql: typeTime time default CURRENT_TIME -->
<!-- c++: QTime typeTime = QTime::currentTime(); -->
<item name="typeTime" type="time" default='CURRENT_TIME'>

<!-- sql: typeTime time default null -->
<!-- c++: QTime typeTime = QTime(); -->
<item name="typeTime" type="time" default='null'>


<!-- date -->

<!-- sql: typeDate date default '2020-01-01' -->
<!-- c++: QDate typeDate = QDate::fromString("2020-01-01"); -->
<item name="typeDate" type="date" default='2020-01-01'>

<!-- sql: typeDate date default null -->
<!-- c++: QDate typeDate = QDate::currentDate(); -->
<item name="typeDate" type="date" default='QDate::currentDate()'>

<!-- sql: typeDate date default CURRENT_DATE -->
<!-- c++: QDate typeDate = QDate::currentDate(); -->
<item name="typeDate" type="date" default='now'>

<!-- sql: typeDate date default CURRENT_DATE -->
<!-- c++: QDate typeDate = QDate::currentDate(); -->
<item name="typeDate" type="date" default='CURRENT_DATE'>

<!-- sql: typeDate date default null -->
<!-- c++: QDate typeDate = QDate(); -->
<item name="typeDate" type="date" default='null'>


<!-- timestamp -->

<!-- sql: typeDatetime timestamp default '2020-01-01 12:59:58.233' -->
<!-- c++: QDateTime typeDatetime = QDateTime::fromString("2020-01-01 12:59:58.233"); -->
<item name="typeDatetime" type="timestamp" default='2020-01-01 12:59:58.233'>

<!-- sql: typeDatetime timestamp default CURRENT_TIMESTAMP -->
<!-- c++: QDateTime typeDatetime = QDateTime::currentDataTime(); -->
<item name="typeDatetime" type="timestamp" default='QDateTime::currentDataTime()'>

<!-- sql: typeDatetime timestamp default CURRENT_TIMESTAMP -->
<!-- c++: QDateTime typeDatetime = QDateTime(); -->
<item name="typeDatetime" type="timestamp" default='null'>

<!-- sql: typeDatetime timestamp default getdate() -->
<!-- c++: QDateTime typeDatetime = QDateTime::currentDataTime(); -->
<item name="typeDatetime" type="timestamp" default='now'>
```

## 如何使用配置

使用`VSCode`打开配置文件，在插件中搜索`QtDao`，安装后在xml文档中右键菜单点击`create entity for qtdao`，将生成一些C++类文件，对应与各个数据表。例如使用如下配置：

```xml
<?xml version="1.0" encoding="utf-8"?>
<dao prefix="ts_" db="sqlite" namespace="CT">
  <tb name="TestTb1" declaremetatype="true">
    <item name="field1" type="long" constraints="primary key" autoincrement="true" default="1" />
    <item name="field2" type="text" constraints="not null"/>
    <item name="field3" type="int" constraints="unique"/>
    <item name="field4" type="int" default="10" />
    <item name="field5" type="variant" />
    <item name="field6" type="text" />
  </tb>
  <tb name="TestTb2" declaremetatype="true">
    <item name="field1" type="long" constraints="primary key" autoincrement="true" default="1" />
    <item name="field3" type="int" constraints="unique"/>
    <item name="field4" type="int" />
  </tb>
</dao>
```

将生成以下文件：

|文件|说明|
|:--|:--|
|entify.cmake|用于CMake工程，在CMakeLists.txt脚本中添加`include(${CMAKE_CURRENT_SOURCE_DIR}/xxx/entity.cmake)`，就可以使用`ENTITY_FILE_LIST`变量列出所有源文件。|
|entity.pri|用于QtCreator的pro工程，使用`include(xxx/entity.pri)`即可将源文件加入。|
|sqliteentityinclude.h/<br/>sqliteentityinclude.cpp|用于数据表自动创建的工具类，一般情况下不需要关心该文件，加入编译即可。|
|testtb1.h/testtb1.cpp|对应`TestTb1`数据表的类。|
|testtb2.h/testtb2.cpp|对应`TestTb2`数据表的类。|

> 在静态库/动态库中使用配置文件生成的类文件

如果在静态库/动态库中使用，数据库ID注册的代码可能会被优化导致无法自动创建数据表，这时需要手动复制`sqliteentityinclude.cpp`文件中`static int id = ...`这段代码到主程序中进行注册。