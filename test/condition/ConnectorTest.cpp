#include "ConnectorTest.h"

#include "../sqliteentity/SqliteTest2.h"

#include "../../src/condition/ConditionOperator.h"

#include <QtTest>

void ConnectorTest::initTestCase() {

}

void ConnectorTest::cleanup() {

}

void ConnectorTest::conditionConnectortest() {
    SqliteTest2::Fields sf;
    auto condition = _and(
        sf.id == 1,
        _or(sf.name == "alice", sf.varianttype == "qwe", sf.number2++),
        sf.name.in(QStringList() << "r" << "t" << "u"),
        sf.number.between(10, 20),
        sf.varianttype.notNull(),
        sf.varianttype.lsNull()
    );
    condition.connect([&](const QString&) { return "a."; });
    QCOMPARE(condition.getConditionStr(),
        QString("a.id=? and (a.name=? or a.varianttype=? or a.number2=a.number2+?) and a.name in (?,?,?) and a.number between ? and ? and a.varianttype is not null and a.varianttype is null")
    );
    QCOMPARE(condition.getValues(),
        QVariantList() << 1 << "alice" << "qwe" << 1 << "r" << "t" << "u" << 10 << 20
    );
}

void ConnectorTest::constraintConditionTest() {
    SqliteTest2::Fields sf;
    //test limit
    auto limit = _constraint(
        _limit(10),
        _limit(20, 30)
    );
    limit.connect();
    QCOMPARE(limit.getConditionStr(),
        QString("limit ? limit ?,?")
    );
    QCOMPARE(limit.getValues(),
        QVariantList() << 10 << 20 << 30
    );

    //test order by / group by
    auto groupOrderBy = _constraint(
        _groupBy(sf.id),
        _groupBy(sf.name, sf.number),
        _orderBy(sf.id),
        _orderBy(sf.id.desc()),
        _orderBy(sf.id, sf.name, sf.number2),
        _orderBy(sf.id, sf.name.desc(), sf.number)
    );
    groupOrderBy.connect([&](const QString&) { return "a."; });
    QCOMPARE(groupOrderBy.getConditionStr(),
        QString("group by a.id group by a.name,a.number order by a.id order by a.id desc order by a.id,a.name,a.number2 order by a.id,a.name desc,a.number")
    );
    QCOMPARE(groupOrderBy.getValues(),
        QVariantList()
    );
}

void ConnectorTest::functionConnectorTest() {
    SqliteTest2::Fields sf;
    auto connector = _and(
        sf.number2 == 1,
        _fun("sum(case when %1=? then ? else ? end)").field(sf.number).value(3, "ee", "ff")
    );
    connector.connect([&](const QString&) { return "a."; });
    QCOMPARE(connector.getConditionStr(), "a.number2=? and sum(case when a.number=? then ? else ? end)");
    QCOMPARE(connector.getValues(),
        QVariantList() << 1 << 3 << "ee" << "ff"
    );
}

void ConnectorTest::havingStatementTest() {
    SqliteTest2::Fields sf;
    auto having = _constraint(
        _having(sf.number > 10),
        _having(_fun("count(%1) > ?").field(sf.number).value(10))
    );
    having.connect([&](const QString&) {return "a."; });
    QCOMPARE(having.getConditionStr(),
        QString("having a.number>? having count(a.number) > ?")
    );
    QCOMPARE(having.getValues(),
        QVariantList() << 10 << 10
    );
}

void ConnectorTest::cleanupTestCase() {

}
