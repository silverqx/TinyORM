#include "orm/schema/foreignkeydefinitionreference.hpp"

#include "orm/schema/foreignidcolumndefinitionreference.hpp"
#include "orm/schema/schemaconstants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs
{

ForeignKeyDefinitionReference::ForeignKeyDefinitionReference(
            const ColumnDefinitionReference<> columnDefinitionReference
)
    : m_columnDefinition(columnDefinitionReference.m_columnDefinition)
{}

ForeignKeyDefinitionReference::ForeignKeyDefinitionReference(
            ForeignIdColumnDefinitionReference foreignIdColumnReference
)
    : m_columnDefinition(foreignIdColumnReference.m_columnDefinition)
{}

ForeignKeyDefinitionReference &
ForeignKeyDefinitionReference::references(const QVector<QString> &columns)
{
    m_columnDefinition.get().references = columns;

    return *this;
}

ForeignKeyDefinitionReference &
ForeignKeyDefinitionReference::on(const QString &table)
{
    m_columnDefinition.get().on = table;

    return *this;
}

ForeignKeyDefinitionReference &
ForeignKeyDefinitionReference::onDelete(const QString &action)
{
    m_columnDefinition.get().onDelete = action;

    return *this;
}

ForeignKeyDefinitionReference &
ForeignKeyDefinitionReference::onUpdate(const QString &action)
{
    m_columnDefinition.get().onUpdate = action;

    return *this;
}

ForeignKeyDefinitionReference &ForeignKeyDefinitionReference::cascadeOnUpdate()
{
    return onUpdate(Cascade);
}

ForeignKeyDefinitionReference &ForeignKeyDefinitionReference::restrictOnUpdate()
{
    return onUpdate(Restrict);
}

ForeignKeyDefinitionReference &ForeignKeyDefinitionReference::cascadeOnDelete()
{
    return onDelete(Cascade);
}

ForeignKeyDefinitionReference &ForeignKeyDefinitionReference::restrictOnDelete()
{
    return onDelete(Restrict);
}

ForeignKeyDefinitionReference &ForeignKeyDefinitionReference::nullOnDelete()
{
    return onDelete(SetNull);
}

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE
