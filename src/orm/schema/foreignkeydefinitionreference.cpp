#include "orm/schema/foreignkeydefinitionreference.hpp"

#include "orm/schema/columndefinition.hpp"
#include "orm/schema/schemaconstants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs
{

/* public */

ForeignKeyDefinitionReference::ForeignKeyDefinitionReference(
        ForeignKeyCommand &foreignKeyCommandDefinition
)
    : m_foreignKeyCommandDefinition(foreignKeyCommandDefinition)
{}

ForeignKeyDefinitionReference &
ForeignKeyDefinitionReference::references(const QList<QString> &columns)
{
    m_foreignKeyCommandDefinition.get().references = columns;

    return *this;
}

ForeignKeyDefinitionReference &
ForeignKeyDefinitionReference::on(const QString &table)
{
    m_foreignKeyCommandDefinition.get().on = table;

    return *this;
}

ForeignKeyDefinitionReference &
ForeignKeyDefinitionReference::onDelete(const QString &action)
{
    m_foreignKeyCommandDefinition.get().onDelete = action;

    return *this;
}

ForeignKeyDefinitionReference &
ForeignKeyDefinitionReference::onUpdate(const QString &action)
{
    m_foreignKeyCommandDefinition.get().onUpdate = action;

    return *this;
}

ForeignKeyDefinitionReference &
ForeignKeyDefinitionReference::deferrable(const bool value)
{
    m_foreignKeyCommandDefinition.get().deferrable = value;

    return *this;
}

ForeignKeyDefinitionReference &
ForeignKeyDefinitionReference::initiallyImmediate(const bool value)
{
    m_foreignKeyCommandDefinition.get().initiallyImmediate = value;

    return *this;
}

ForeignKeyDefinitionReference &ForeignKeyDefinitionReference::notValid(const bool value)
{
    m_foreignKeyCommandDefinition.get().notValid = value;

    return *this;
}

/* Shortcuts */

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
