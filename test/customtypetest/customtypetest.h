#pragma once

#include <qobject.h>

class CustomTypeTest : public QObject {
    Q_OBJECT

public:
    using QObject::QObject;

private slots:
    void initTestCase();

    void testInsert();

    void testSelect();

    void testConditionIn();

    void cleanup();

    void cleanupTestCase();
};
