#pragma once

#include <qobject.h>
#include <qsharedpointer.h>

namespace dao {
    class AbstractClient;
}

class ClientSqliteTest : public QObject {
    Q_OBJECT

public:
    using QObject::QObject;

private slots:
    void initTestCase();

    void databaseProcessTest();

    void tableProcessTest();

    void indexProcessTest();

    void fieldProcessTest();

    void dataTransTest();

    void cleanup();

    void cleanupTestCase();

private:
    QSharedPointer<dao::AbstractClient> client;
};
