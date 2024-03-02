#pragma once

#include <qobject.h>

struct ForeignKey {
    enum Action {
        FK_NO_ACTION,  //no_action
        FK_RESTRICT,   //restrict
        FK_SET_NULL,   //set_null
        FK_SET_DEFAULT,//set_default
        FK_CASCADE,    //cascade

        FK_NotSet,
    };

    ForeignKey(QString referenceTb, Action onUpdate, Action onDelete, bool deferrable = false)
        : referenceTb(std::move(referenceTb))
        , onUpdate(onUpdate)
        , onDelete(onDelete)
        , deferrable(deferrable)
    {}

    template<typename... Args>
    ForeignKey& field(const QString& name, const QString& referTo, Args... args) {
        fieldKeys << name;
        referenceKeys << referTo;
        return field(args...);
    }

    ForeignKey& field() {
        return *this;
    }

    QString referenceTb;
    Action onUpdate;
    Action onDelete;
    bool deferrable; //sqlite

    QStringList fieldKeys;
    QStringList referenceKeys;
};