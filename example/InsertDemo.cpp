#include "InsertDemo.h"

void InsertDemo::insertOrReplace() {
    //插入单个对象，id的值无关紧要，插入值时不会读取id值，插入后会将新增的id赋值回被插入对象
    dao::_insert<User>().build().insert(User(0, "Alice", 12, 1, "student"));

    //插入一个列表
    int st = 1525276800;//2018-5-3
    int et = 1549555199;//2019-2-7
    UserList users;
    int i = 0;
    while (st < et) {
        users << User(0, QString("Bob_") + QString::number(i), st, i % 2, "");
        st += 12 * 3600 + i;
        i++;
    }
    //insertBatch为常规的批量插入，最终调用query.execBatch
    //dao::_insert<User>().build().insertBatch(users);

    //insertMutil为values语句循环拼接作为1条sql语句插入，需要注意的是数据库对sql语句的长度有限制
    dao::_insert<User>().build().insertMutil(users);

    //如何插入任意变量类型到数据库，借助QVariant类型变量
    SettingList slist;
    slist << Setting("s1", 1);
    slist << Setting("s2", "abc");
    slist << Setting("s3", 3.56f);
    slist << Setting("s4", QColor(Qt::red));
    dao::_replace<Setting>().build().insertMutil(slist);
}
