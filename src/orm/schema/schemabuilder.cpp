#include "orm/schema/schemabuilder.hpp"

#include "orm/databaseconnection.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Schema
{

SchemaBuilder::SchemaBuilder(DatabaseConnection &connection)
    : m_connection(connection)
    , m_grammar(connection.getSchemaGrammar())
{}

QStringList SchemaBuilder::getColumnListing(const QString &table) const
{
    // FEATURE table prefix silverqx
    auto [ok, query] = m_connection.selectFromWriteConnection(
            m_grammar.compileColumnListing(table));

    return m_connection.getPostProcessor().processColumnListing(query);
}

} // namespace Orm::Schema
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
