#pragma once

#include "basequery.h"
#include "reader/entityreaderinterface.h"
#include "reader/builderreaderinterface.h"

QTDAO_BEGIN_NAMESPACE

class FromBuilder;
class SelectImpl
        : protected BaseQuery
        , protected virtual EntityReaderInterface
        , protected virtual BuilderReaderInterface
{
public:
    using BaseQuery::BaseQuery;

protected:
    void buildFilterSqlStatement();

    QString getBindColumns(QVariantList& values);

    QString readExplainStatement();

    //----- executors -----
    typedef std::function<void(const QString&, const QVariant&)> EntityBinder;

    typedef std::function<void(const QSqlRecord&)> RecordBinder;

    static void recordBind(const QSqlRecord& record, const EntityBinder& entityBinder);

    void uniqueExec(const EntityBinder& entityBinder, const RecordBinder& recordHandler = nullptr);

    void oneExec(const EntityBinder& entityBinder, const RecordBinder& recordHandler = nullptr);

    void listExec(const RecordBinder& recordHandler);

    //----- caster -----
    template<typename T>
    static T readFromRecord(const QSqlRecord& record, const EntityField<T>& field) {
        return record.value(field()).template value<T>();
    }

    template<typename T, typename K>
    static QPair<T, K> readFromRecord(const QSqlRecord& record,
                                      const EntityField<T>& field1,
                                      const EntityField<K>& field2) {
        return qMakePair(readFromRecord(record, field1), readFromRecord(record, field2));
    }

    template<typename T, typename... Args>
    static std::tuple<T, typename Args::Type...> readFromRecords(const QSqlRecord& record,
                               const EntityField<T>& field,
                               const Args&... args) {
        auto d = readFromRecord(record, field);
        return std::tuple_cat(std::tie(d), readFromRecords(record, args...));
    }

    template<typename T>
    static std::tuple<T> readFromRecords(const QSqlRecord& record, const EntityField<T>& field) {
        return std::make_tuple(readFromRecord(record, field));
    }

protected:
    int topSize = 0;
    bool topPercent = false;

    friend class FromBuilder;
    friend class UnionBuilderImpl;
};

QTDAO_END_NAMESPACE
