#pragma once

#include <qobject.h>

namespace DaoMysql {
    class EntityDelegate : public QObject {
        Q_OBJECT

    public:
        Q_INVOKABLE explicit EntityDelegate() {}

        Q_INVOKABLE void createEntityTables();

        Q_INVOKABLE void entityTablesUpgrade();
    };
}
Q_DECLARE_METATYPE(DaoMysql::EntityDelegate*);