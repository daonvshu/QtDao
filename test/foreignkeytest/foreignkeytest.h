#pragma once

#include <qobject.h>

class KeywordsTest : public QObject {
    Q_OBJECT

public:
    using QObject::QObject;

private slots:
    void initTestCase();

    void testStep();

    void testInsert();

    void testSelect();

    void testUpdate();

    void testJoin();

    void cleanup();

    void cleanupTestCase();
};
