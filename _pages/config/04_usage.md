---
title: 基本使用
category: config
layout: post
---

连接数据库
-------------

`QtDao`不需要手动建立连接，在初始化之后，进行数据库查询时动态创建连接，并且每个连接与当前线程相关的。根据Qt文档数据库模块与线程相关所述，不同线程的数据库连接是不能够共用的，因此，在QtDao建立查询时，当前线程与数据库的连接名是绑定在一起的，同一个线程共用一个连接，否则创建新的连接。如下所示：
```cpp
class MyWorkThread : public QThread {
protected:
    void run() override {
        //...
        User::Field uf;
        auto users = dao::_select<User>().filter(uf.age >= 18).build().list();
    }
}
```

同时连接多个数据库
-------------

目前部分类使用了单例模式，暂不支持多个数据库同时连接，在未来会实现该功能。

日志输出
-------------

`QtDao`内部使用`QLoggingCategory`打印日志，如果要查看某一个查询执行的sql语句，在执行`build`之前调用`logging`函数传入`category`即可：

```cpp

Q_LOGGING_CATEGORY(userCategory, "query.user")

UserList listUsers() {
    User::Fields field;
    return dao::_select<User>()
        .filter(field.score > 100)
        .logging(userCategory)
        .build().list();
}
```

在不需要打印指定查询时，关闭其日志即可：

```cpp
int main(int argc, char *argv[]) {
    //...
    QLoggingCategory::setFilterRules("query.user.debug=false");
    //...
}
```

使用函数`dao::loggingUseDefault()`启用默认日志输出，此时未使用logging的查询将使用默认的`category`：
```cpp
dao::loggingUseDefault();
//关闭默认 category
QLoggingCategory::setFilterRules("*qtdao.query.debug=false");
```