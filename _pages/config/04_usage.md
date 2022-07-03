---
title: 基本使用
category: Jekyll
layout: post
---

连接数据库
-------------

`QtDao`不需要手动建立连接，在编写好[连接配置文件]({{site.baseurl}}/pages/config/03_init/#创建连接配置文件)之后，进行数据库查询时动态创建连接，并且每个连接与当前线程相关的。根据Qt文档数据库模块与线程相关所述，不同线程的数据库连接是不能够共用的，因此，在QtDao建立查询时，当前线程与数据库的连接名是绑定在一起的，同一个线程共用一个连接，否则创建新的连接。尽管使用了连接池，但还是带来了新的问题，解除线程的绑定需要手动进行，目前还没有办法检测到工作线程的结束，手动释放连接如下所示：
```cpp
class MyWorkThread : public QThread {
protected:
    void run() override {
        //...
        User::Field uf;
        auto users = dao::_select<User>().filter(uf.age >= 18).build().list();
        //...
        //close connection
        ConnectionPool::release();
    }
}
```
当然，如果使用了线程池，可以考虑不关闭当前工作线程连接。

同时连接多个数据库
-------------

目前部分类使用了单例模式，暂不支持多个数据库同时连接，在未来会实现该功能。

日志输出
-------------

目前使用的数据库执行过程日志通过统一的回调注册，并交给程序员自己处理。未来将加入更加完善的日志管理系统。
```cpp
#include "dao.h"

void SqlLogPrinter(const QString& sql, const QVariantList& values) {
#ifdef QT_DEBUG
    qDebug() << "sql:" << sql << " values:" << values;
#endif
}

int main(int argc, char *argv[])
{
    //...
    daoSetQueryLogPrinter(SqlLogPrinter);
    //do something ...
}
```