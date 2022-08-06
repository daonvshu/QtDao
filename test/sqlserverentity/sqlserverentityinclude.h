#pragma once

#include <qobject.h>

namespace DaoSqlServer {
    class SqlServerEntityDelegate : public QObject {
        Q_OBJECT

    public:
        Q_INVOKABLE explicit SqlServerEntityDelegate() {}

        Q_INVOKABLE void createEntityTables();

        Q_INVOKABLE void entityTablesUpgrade();
    };
}
Q_DECLARE_METATYPE(DaoSqlServer::SqlServerEntityDelegate*);