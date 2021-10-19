#include "orm/schema/postgresschemabuilder.hpp"

#include "orm/databaseconnection.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Schema
{

QStringList PostgresSchemaBuilder::getColumnListing(const QString &table) const
{
    auto [schema, table_] = parseSchemaAndTable(table);

    table_ = QStringLiteral("%1%2").arg(m_connection.getTablePrefix(), table);

    auto query = m_connection.select(m_grammar.compileColumnListing(), {
        schema, table_
    });

    return m_connection.getPostProcessor().processColumnListing(query);
}

std::pair<QString, QString>
PostgresSchemaBuilder::parseSchemaAndTable(const QString &table) const
{
    QString schema;

    if (m_connection.getConfig().contains("schema")) {
        auto table_ = table.split(DOT);
        auto schemaConfig = m_connection.getConfig("schema").value<QStringList>();

        // table was specified with the schema, like schema.table, so use this schema
        if (schemaConfig.contains(table_.at(0)))
            return {table_.takeFirst(), table_.join(DOT)};

        // Instead, get a schema from the configuration
        if (!schemaConfig.isEmpty())
            schema = std::move(schemaConfig[0]);
    }

    // Default schema
    if (schema.isEmpty())
        schema = "public";

    return {std::move(schema), table};
}

} // namespace Orm::Schema

TINYORM_END_COMMON_NAMESPACE
