## 使用步骤
### 1.编译生成器
下载[DbEntityGenerator](https://github.com/daonvshu/DbEntityGenerator)代码生成器，使用你本地的Qt库编译出生成器程序

### 2.编辑数据库结构文件
在你的工程目录下新建一个文件夹用于存放数据库实体类比如entity，将库中对应的[数据库结构实例文件](https://github.com/daonvshu/QtDao/tree/master/entity)拷贝到entity文件夹中，参照实例文件xml编辑数据库结构并遵循xsd文件约束避免出错。接下来使用生成器生成对应实体类，将预先编译好的DbEntityGenerator拷贝到当前entity文件夹中并在当前目录下使用命名行执行以下命令：
```
DbEntityGenerator.exe sqlite_entity.xml ../qtdao/src
```
其中第一个参数为当前使用的数据库结构配置文件，第二个参数为库目录src的相对路径。生成成功后将生成的文件包含至项目中。

### 3.初始化
新建一个数据库连接配置类如下：
```c++
class SqliteConfig : public QObject {
    Q_OBJECT

    Q_PROPERTY(int version READ getVersion())
    Q_PROPERTY(QString type READ getSqlType())
    Q_PROPERTY(QString dbName READ getDatabaseName())

public:
    int getVersion() {
        return ver;
    }

    QString getSqlType() {
        return "sqlite";
    }

    QString getDatabaseName() {
        return "sqlitetest";
    }

    int ver = 1;
};
```
其中`version`,`type`,`dbName`属性必不可少，类名无关紧要，qtdao将通过property读取配置参数（各个数据库配置参考[configdemo](https://github.com/daonvshu/QtDao/tree/master/doc/configdemo)）。接下来在main函数中（或适当的时机）执行以下的初始化
```c++
class CustomDbExceptionHandler : public DbExceptionHandler {
public:
    using DbExceptionHandler::DbExceptionHandler;

    void initDbFail(DbErrCode errcode, const QString& reason) override {};

    void databaseOpenFail(DbErrCode errcode, const QString& failReason) override {};

    void execFail(DbErrCode errcode, const QString& lastErr) {};

    void execWarning(const QString& info) override {};
};

void SqlLogPrinter(const QString& sql, const QVariantList& values) {
    
}

int main(int argc, char *argv[])
{
    ...
    daoSetQueryLogPrinter(SqlLogPrinter);
    DbLoader::init(SqliteConfig(), new CustomDbExceptionHandler(parent));
    ...
}
```
这里`CustomDbExceptionHandler`类不是必要的，它用于处理数据库操作时的错误或警告。`SqlLogPrinter`也不是必要的，它用于打印sql执行时的sql语句和值列表，执行init后将自动执行以下操作：
- 检查数据库连接
- 创建数据库（可选）
- 创建数据表（可选）
- 检查数据库版本并升级（可选）

### 4.注意
到这里你可以开始使用qtdao进行数据库操作了，使用时只需包含src/dao.h头文件即可，在使用过程中需要注意以下问题：

- qtdao查询是支持在不同线程中进行的，当在工作线程中进行查询时注意在线程结束前关闭当前线程占用的数据库连接，调用下面的函数关闭当前线程的连接（如果使用线程池管理线程，可不用关闭当前线程的连接）
```
ConnectionPool::closeConnection();
```
或者使用本地线程监听类
```
dao::LocalQuery local;
```
当local实例离开当前作用域被释放后，会自动调用ConnectionPool::closeConnection函数

- 程序结束时需手动释放所有数据库连接
```
ConnectionPool::release();
```

- 由于数据库创建表时使用的是反射机制，因此，如果EntityDelegate类（由DBEntityGenerator生成）用于编译成静态/动态库，在数据库初始化之前需要手动实例化一个EntityDelegate以加入到qt元对象系统，如下：
```c++
int main(int argc, char *argv[])
{
    ...
    DaoSqlite::SqliteEntityDelegate del;
    DbLoader::init(SqliteConfig(), new CustomDbExceptionHandler(parent));
    ...
}
```

- 创建数据库和创建表是可选的，调用以下函数关闭功能：
```c++
DbLoader::disableCreateDatabase();//不创建数据库
DbLoader::disableCreateTable();//不创建数据表
```