#pragma once

#include "ConnectionPool.h"
#include "DbLoader.h"

#include "builder/InsertBuilder.h"
#include "builder/SelectBuilder.h"
#include "builder/CountBuilder.h"
#include "builder/UpdateBuilder.h"
#include "builder/DeleteBuilder.h"
#include "builder/JoinBuilder.h"
#include "builder/RecursiveQueryBuilder.h"

#include "condition/ConditionOperator.h"

class dao {
public:
    template<typename T>
    static InsertBuilder<T> _insert() {
        return InsertBuilder<T>();
    }

    template<typename T>
    static SelectBuilder<T> _select() {
        return SelectBuilder<T>();
    }

    template<typename T>
    static QList<T> _selectAll() {
        return SelectBuilder<T>().build().list();
    }

    template<typename T>
    static CountBuilder<T> _count() {
        return CountBuilder<T>();
    }

    template<typename T>
    static UpdateBuilder<T> _update() {
        return UpdateBuilder<T>();
    }

    template<typename T>
    static DeleteBuilder<T> _delete() {
        return DeleteBuilder<T>();
    }

    template<typename... T>
    static JoinBuilder<T...> _join() {
        return JoinBuilder<T...>();
    }

    static RecursiveQueryBuilder _recursive(bool unionAll = false) {
        return RecursiveQueryBuilder(unionAll);
    }

    template<typename E>
    class self : public E {
    public:
        struct Info : E::Info {
            enum {
                Attach = E::Info::Attach + 1
            };

            static QString getSourceName() {
                return E::Info::getSourceName();
            }

            static QString getTableName() {
                return getSourceName() + "_" + QString::number(Attach);
            }
        };

        class Fields : public E::Fields {
        public:
            Fields() {
                reset(Info::getTableName());
            }
        };
    };
};
