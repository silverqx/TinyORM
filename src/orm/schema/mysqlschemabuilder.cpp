#include "orm/schema/mysqlschemabuilder.hpp"

#include "orm/databaseconnection.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Schema
{

QStringList MySqlSchemaBuilder::getColumnListing(const QString &table) const
{
    // FEATURE table prefix silverqx
//    const auto table_ = m_connection.getTablePrefix() + table;

    auto [ok, query] = m_connection.select(m_grammar.compileColumnListing(), {
        m_connection.getDatabaseName(), table
    });

    return m_connection.getPostProcessor().processColumnListing(query);
}

} // namespace Orm::Schema
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
