#pragma once

#include <qdatastream.h>
#include <qdatetime.h>

struct MyStruct {
    QString name;
    int id;
    QDateTime dateTime;

    bool operator==(const MyStruct& other) const {
        return other.name == this->name;
    }
};

inline QDataStream& operator<<(QDataStream& out, const MyStruct& data) {
    out << data.name << data.id << data.dateTime;
    return out;
}

inline QDataStream& operator>>(QDataStream& in, MyStruct& data) {
    in >> data.name >> data.id >> data.dateTime;
    return in;
}