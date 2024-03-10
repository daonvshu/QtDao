#pragma once

#include <qobject.h>

class ForeignKeyTest : public QObject {
    Q_OBJECT

public:
    using QObject::QObject;

private slots:
    void initTestCase();

    void testInsert();

    void testUpdate();

    void testDelete();

    void testTransaction();

    void testVersionUpgrade();

    void cleanup();

    void cleanupTestCase();
};
