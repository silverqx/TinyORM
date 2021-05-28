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
    const QString table_ = m_connection.getTablePrefix() + table;

    auto query = m_connection.select(m_grammar.compileColumnListing(), {
        m_connection.getDatabaseName(), table_
    });

    return m_connection.getPostProcessor().processColumnListing(query);
}

} // namespace Orm::Schema
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
