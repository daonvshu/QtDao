#pragma once

#include <qvector.h>

#include "basequery.h"
#include "reader/entityreaderinterface.h"
#include "reader/builderreaderinterface.h"

QTDAO_BEGIN_NAMESPACE

enum class JoinType {
    CrossJoin,
    InnerJoin,
    LeftJoin,
    RightJoin,
    FullJoin,

    Unset,
};

struct JoinData {
    JoinType joinType = JoinType::Unset;
    FilterGroupConnector filter;
    //select from select
    FromBuildData fromBuildData;
};

class JoinImpl
        : protected BaseQuery
        , protected virtual BuilderReaderInterface
{
public:
    JoinImpl(QString mainTable, const QHash<QString, JoinData>& subJoinData)
        : mainTable(std::move(mainTable)), subJoinData(subJoinData) {}

protected:
    void buildJoinSqlStatement();

    QString readExplainStatement();

    virtual void setTableOrder() = 0;

    virtual QString getAllEntityField() = 0;

    virtual QString getJoinTypeName(JoinType type) = 0;

    virtual void initSequenceTbName() = 0;

protected:
    QString mainTable;
    QHash<QString, JoinData> subJoinData;
    QList<FieldInfo> usedColumns;

    QHash<QString, QString> tableOrder;
    QList<QPair<QString, QString>> sequenceTableNames;

    bool insideRecursiveQuery = false;

    friend class FromBuilder;
    friend class UnionBuilderImpl;
};

template<typename... T> struct JoinEUnpackHelper;
template<typename E, typename... T>
struct JoinEUnpackHelper<E, T...> : JoinEUnpackHelper<T...> {

    static void setOrder(QHash<QString, QString>& orderMap, int i) {
        orderMap.insert(E::Info::getTableName(), (char)('a' + i) + QString("."));
        JoinEUnpackHelper<T...>::setOrder(orderMap, i + 1);
    }

    static QString readEntityFields(const QHash<QString, QString>& orderMap, QList<FieldInfo>& fieldInfo) {
        typename E::Info info;
        QStringList fields = info.getFields();
        auto tbName = info.getTableName();
        QString str;
        for (const auto& f : fields) {
            str.append(orderMap.value(tbName)).append(f).append(",");
            fieldInfo.append(FieldInfo{ f,tbName });
        }
        str.append(JoinEUnpackHelper<T...>::readEntityFields(orderMap, fieldInfo));
        return str;
    }

    static QList<QPair<QString, QString>> getTbName() {
        typename E::Info info;
        QList<QPair<QString, QString>> names;
        names << qMakePair(info.getTableName(), info.getSourceName());
        names.append(JoinEUnpackHelper<T...>::getTbName());
        return names;
    }

    template<int I, typename... K>
    static void bindTupleValue(std::tuple<K...>& result, const QVector<QList<QPair<QString, QVariant>>>& values) {
        typename E::Tool tool;
        const auto& value = values.at(I);
        for (const auto& v : value) {
            tool.bindValue(std::get<I>(result), v.first, v.second);
        }
        JoinEUnpackHelper<T...>::template bindTupleValue<I + 1, K...>(result, values);
    }
};

template<> struct JoinEUnpackHelper<> {
    static void setOrder(QHash<QString, QString>& orderMap, int i) {Q_UNUSED(orderMap);Q_UNUSED(i);}
    static QString readEntityFields(const QHash<QString, QString>& orderMap, QList<FieldInfo>& fieldInfo) { Q_UNUSED(orderMap); Q_UNUSED(fieldInfo); return {}; }
    static QList<QPair<QString, QString>> getTbName() { return {}; }

    template<int I, typename... K>
    static void bindTupleValue(std::tuple<K...>& result, const QVector<QList<QPair<QString, QVariant>>>& values) {Q_UNUSED(result);Q_UNUSED(values);}
};

QTDAO_END_NAMESPACE
