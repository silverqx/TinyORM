#include "orm/schema/schemabuilder.hpp"

#include "orm/databaseconnection.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Schema
{

SchemaBuilder::SchemaBuilder(DatabaseConnection &connection)
    : m_connection(connection)
    , m_grammar(connection.getSchemaGrammar())
{}

QStringList SchemaBuilder::getColumnListing(const QString &table) const
{
    auto query = m_connection.selectFromWriteConnection(
                     m_connection.getTablePrefix() +
                     m_grammar.compileColumnListing(table));

    return m_connection.getPostProcessor().processColumnListing(query);
}

} // namespace Orm::Schema

TINYORM_END_COMMON_NAMESPACE
