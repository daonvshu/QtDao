#pragma once

#include <qobject.h>

class JoinTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();

    void cleanup();

    void cleanupTestCase();
};