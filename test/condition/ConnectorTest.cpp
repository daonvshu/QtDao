#include "ConnectorTest.h"

#include "../sqliteentity/SqliteTest2.h"

#include <qtest.h>

void ConnectorTest::initTestCase() {

}

void ConnectorTest::cleanup() {

}

void ConnectorTest::conditionConnectortest() {
    SqliteTest2::Fields sf;
    auto condition = _and(sf.id == 1, _or(sf.name == "alice", sf.varianttype == "qwe", sf.number2++));
    condition.connect("");
    QString conditionStr = condition.getConditionStr();
    QCOMPARE(conditionStr, QString("(id=? and (name=? or varianttype=? or number2=number2+?))"));
    QVariantList values = condition.getValues();
    QCOMPARE(values, QVariantList() << 1 << "alice" << "qwe" << 1);
}

void ConnectorTest::cleanupTestCase() {

}
