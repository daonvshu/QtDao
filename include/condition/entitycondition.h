#pragma once

#include "../global.h"

#include <qobject.h>
#include <qvariant.h>

#include "connector/connectable.h"

#include "utils/serializing.h"

QTDAO_BEGIN_NAMESPACE

class EntityConnector : public Connectable {
public:
    QList<FieldInfo> getUsedFields() override;

    QVariantList getValueList() override;

    QString getConditionSegment() override;

    virtual void addField(const FieldInfo& field);

    virtual void addValue(const QVariant& value);

    template<typename T>
    void addCustomValue(const T& value) {
        addValue(serializeCustomTypeToBinary(value));
    }

protected:
    QString getField(int index) const;

protected:
    QList<FieldInfo> fields;
    QVariantList values;
    QString connectedStr;
};

class OperatorEntityConnector : public EntityConnector {
public:
    void setOperator(const QString& op) {
        this->connectorOp = op;
    }

    void combine() override;

protected:
    QString connectorOp;
};

class SelfOperatorEntityConnector : public OperatorEntityConnector {
public:
    void combine() override;
};

template<typename T, typename Connector>
class BatchOperatorEntityConnectorImpl : public Connector {
public:
    void addValues(const QList<T>& values) {
        QVariantList variantList;
        for (const auto& v : values) {
            variantList << v;
        }
        Connector::addValue(QVariant(variantList));
    }

    void addCustomValues(const QList<T>& values) {
        QVariantList variantList;
        for (const auto& v : values) {
            variantList << serializeCustomTypeToBinary(v);
        }
        Connector::addValue(QVariant(variantList));
    }
};

template<typename T>
class BatchOperatorEntityConnector : public BatchOperatorEntityConnectorImpl<T, OperatorEntityConnector> {};

template<typename T>
class BatchSelfOperatorEntityConnector : public BatchOperatorEntityConnectorImpl<T, SelfOperatorEntityConnector> {};

class FieldOperatorEntityConnector : public EntityConnector {
public:
    void addFields(const FieldInfo& to, const FieldInfo& from) {
        addField(to);
        addField(from);
    }

    void setOperator(const QString& op) {
        this->connectorOp = op;
    }

    void combine() override;

protected:
    QString connectorOp;
};

class SelfFieldOperatorEntityConnector : public FieldOperatorEntityConnector {
public:
    void combine() override;
};

class InEntityConnector : public EntityConnector {
public:
    template<typename T>
    void addValues(const QList<T>& values) {
        for (const auto& v : values) {
            addValue(v);
        }
    }

    template<typename T>
    void addCustomValues(const QList<T>& values) {
        for (const auto& v : values) {
            addValue(serializeCustomTypeToBinary(v));
        }
    }

    void combine() override;
};

class BetweenConnector : public EntityConnector {
public:
    template<typename T>
    void addValues(const T& from, const T& to) {
        addValue(from);
        addValue(to);
    }

    void combine() override;
};

class IsNullConnector : public EntityConnector {
public:
    explicit IsNullConnector(bool isNull) : checkIsNull(isNull) {}

    void combine() override;

private:
    bool checkIsNull;
};

QTDAO_END_NAMESPACE