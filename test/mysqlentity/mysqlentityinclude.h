#pragma once

#include <qobject.h>

namespace DaoMysql {
    class MysqlEntityDelegate : public QObject {
        Q_OBJECT

    public:
        Q_INVOKABLE explicit MysqlEntityDelegate() {}

        Q_INVOKABLE void createEntityTables();

        Q_INVOKABLE void entityTablesUpgrade();
    };
}
Q_DECLARE_METATYPE(DaoMysql::MysqlEntityDelegate*);