#include "orm/schema/mysqlschemabuilder.hpp"

#include "orm/databaseconnection.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Schema
{

QStringList MySqlSchemaBuilder::getColumnListing(const QString &table) const
{
    const QString table_ = QStringLiteral("%1%2").arg(m_connection.getTablePrefix(),
                                                      table);

    auto query = m_connection.select(m_grammar.compileColumnListing(), {
        m_connection.getDatabaseName(), table_
    });

    return m_connection.getPostProcessor().processColumnListing(query);
}

} // namespace Orm::Schema

TINYORM_END_COMMON_NAMESPACE
