#pragma once

#include <qobject.h>

namespace DaoSqlite {
    class SqliteEntityDelegate : public QObject {
        Q_OBJECT

    public:
        Q_INVOKABLE explicit SqliteEntityDelegate() {}

        Q_INVOKABLE void createEntityTables();

        Q_INVOKABLE void entityTablesUpgrade();
    };
}
Q_DECLARE_METATYPE(DaoSqlite::SqliteEntityDelegate*);