#pragma once

#include <qobject.h>

namespace DaoSqlite {
    class EntityDelegate : public QObject {
        Q_OBJECT

    public:
        Q_INVOKABLE explicit EntityDelegate() {}

        Q_INVOKABLE void createEntityTables();

        Q_INVOKABLE void entityTablesUpgrade();
    };
}
Q_DECLARE_METATYPE(DaoSqlite::EntityDelegate*);