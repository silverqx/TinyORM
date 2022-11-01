#include "orm/schema/foreignidcolumndefinitionreference.hpp"

#include "orm/schema/blueprint.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

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
    const auto &foreignName = m_columnDefinition.get().name;

    const auto guessTable = QStringLiteral("%1s")
                            .arg(foreignName.left(
                                     foreignName.lastIndexOf(
                                         QStringLiteral("_%1").arg(column))));

    return references(QVector<QString> {column})
            .on(table.isEmpty() ? guessTable : table);
}

ForeignKeyDefinitionReference
ForeignIdColumnDefinitionReference::references(const QVector<QString> &columns)
{
    m_foreignKeyDefinitionReference =
            m_blueprint.get().foreign(m_columnDefinition.get().name);

    return m_foreignKeyDefinitionReference->references(columns);
}

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE
