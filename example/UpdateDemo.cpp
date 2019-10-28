#include "UpdateDemo.h"

void UpdateDemo::update() {
    //创建一个操作User表字段的实例
    User::Fields uf;

    //更新指定字段，由于"="的运算符优先级较高，set和where条件中统一使用"=="
    dao::_update<User>().set(uf.age == 1, uf.duty == "12345").wh(uf.name == "Bob_0" or uf(uf.age <= 1525363201 and uf.age >= 1525276800)).build().update();

    //批量更新指定字段，值必须是QVariantList类型
    QVariantList ageList, dutyList, names;
    ageList << 15 << 16 << 17;
    dutyList << "schoolmate" << "teacher" << "father";
    names << "Bob_5" << "Bob_6" << "Bob_7";
    dao::_update<User>().set(uf.age == ageList, uf.duty == dutyList).wh(uf.name == names).build().updateBatch();

    //更新一个对象，通过id主键更新，没有id主键的表考虑使用insertOrReplace更新
    bool exist;
    User schoolmate = dao::_query<User>().wh(uf.duty == "schoolmate").build().unique(exist);
    schoolmate.setAge(666);
    dao::_update<User>().build().update(schoolmate);

    //更新一堆对象
    UserList bobs = dao::_query<User>().wh(uf.age < 20 and uf.age > 15).build().list();
    for (int i = 0; i < bobs.size(); i++) {
        bobs[i].setAge(20);
    }
    dao::_update<User>().build().updateBatch(bobs);
}
