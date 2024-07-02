#include "orm/schema/foreignidcolumndefinitionreference.hpp"

#include "orm/schema/blueprint.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

#ifdef TINYORM_DISABLE_ORM
using Orm::Constants::TMPL_PLURAL;
#endif

namespace Orm::SchemaNs
{

ForeignIdColumnDefinitionReference::ForeignIdColumnDefinitionReference(
        Blueprint &blueprint,
        const ColumnDefinitionReference<> columnDefinitionReference
)
    : ColumnDefinitionReference(columnDefinitionReference.m_columnDefinition)
    , m_blueprint(blueprint)
{}

ForeignKeyDefinitionReference
ForeignIdColumnDefinitionReference::constrained(const QString &table,
                                                const QString &column)
{
    // Guess table name, cut off _column from the foreign index name and make it plural
    const auto guessTableName = [this, &column]
    {
        const auto &foreignName = m_columnDefinition.get().name;

        // Nothing to guess, the column is empty
        if (column.isEmpty())
            return TMPL_PLURAL.arg(foreignName);

        // Don's use the Qt6's QString::first() here (the result can be -1)
        return TMPL_PLURAL.arg(foreignName.left(
                                   foreignName.lastIndexOf(
                                       QStringLiteral("_%1").arg(column))));
    };

    return references(QList<QString> {column})
            .on(table.isEmpty() ? guessTableName() : table);
}

ForeignKeyDefinitionReference
ForeignIdColumnDefinitionReference::references(const QList<QString> &columns)
{
    m_foreignKeyDefinitionReference =
            m_blueprint.get().foreign(m_columnDefinition.get().name);

    return m_foreignKeyDefinitionReference->references(columns);
}

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE
