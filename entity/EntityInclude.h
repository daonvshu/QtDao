#pragma once

#include <qobject.h>

class EntityDelegate : public QObject {
    Q_OBJECT

public:
    Q_INVOKABLE explicit EntityDelegate() {}

    Q_INVOKABLE void createEntityTables();
};
Q_DECLARE_METATYPE(EntityDelegate*);