#include "FunctionDemo.h"

void FunctionDemo::funtion() {
    //运算符重载后也不能实现的复杂功能使用自定义的函数表达式
    EntityFunction fun;
    EntityField fAge = "age";
    EntityField fSize = "size";
    EntityField fTotal = "total";

    //set condition test
    //age=sum(size)+2, age=age+sum(case when size = 3 then 'ee' else 'ff' end), age=age+sum(min(size)+5+max(total))
    auto setConditions = (fAge == fun.fun().expStr("sum(%1)+?").field(fSize).value(2),
                     fAge + fun.fun().expStr("sum(case when %1=? then ? else ? end)").field(fSize).value(3, "ee", "ff"),
                     fAge + fun.fun().expStr("sum(min(%1)+?+max(%2))").field(fSize, fTotal).value(5));
    qDebug() << "expression = " << setConditions.getExpressionStr() << "   values = " << setConditions.getValueList();

    //where condition test
    EntityField::bindTableOrder(0, fAge);
    EntityField::bindTableOrder(1, fSize);
    EntityField::bindTableOrder(2, fTotal);
    //a.age>1 and (a.age!=sum(b.size+c.total) or max(b.size) != case when c.total > b.size when c.total else b.size end and length(c.total)>5 or c.total != 0) and b.size<9
    setConditions = fAge > 1 and
        fun(fAge != fun.fun().expStr("sum(%1+%2)").field(fSize, fTotal) or
            fun.fun().expStr("max(%1)!=case when %2 > %1 when %2 else %1 end").field(fSize, fTotal) and
            fun.fun().expStr("length(%1)>?").field(fTotal).value(5) or
            fTotal != 0)
        and
        fSize < 9;
    qDebug() << "expression = " << setConditions.getExpressionStr() << "   values = " << setConditions.getValueList();

    //group by a.age having sum(b.size+4)>5 and c.total != 0 order by sum(c.total) desc limit 10
    setConditions = (fun.groupBy(fAge), fun.having(fun.fun().expStr("sum(%1+?)>?").field(fSize).value(4, 5) and
                                                   fTotal != 0), fun.orderBy(fun.fun().expStr("sum(%1)").field(fTotal).desc()), fun.limit(10));
    qDebug() << "expression = " << setConditions.getExpressionStr(true) << "   values = " << setConditions.getValueList();
}
