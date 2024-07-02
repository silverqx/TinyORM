#include "orm/schema/mysqlschemabuilder.hpp"

#include "orm/databaseconnection.hpp"
#include "orm/utils/query.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using QueryUtils = Orm::Utils::Query;

namespace Orm::SchemaNs
{

/* public */

std::optional<SqlQuery> MySqlSchemaBuilder::createDatabase(const QString &name) const
{
    return m_connection->statement(
                m_grammar->compileCreateDatabase(name, *m_connection));
}

std::optional<SqlQuery>
MySqlSchemaBuilder::dropDatabaseIfExists(const QString &name) const
{
    return m_connection->statement(
                m_grammar->compileDropDatabaseIfExists(name));
}

void MySqlSchemaBuilder::dropAllTables() const
{
    auto query = getAllTables();
    const auto querySize = QueryUtils::queryResultSize(query);

    // Nothing to do, empty result
    if (querySize <= 0)
        return;

    QList<QString> tables;
    tables.reserve(querySize);

    while (query.next())
        tables << query.value(0).value<QString>();

    Q_ASSERT(!tables.isEmpty());

    disableForeignKeyConstraints();

    m_connection->statement(m_grammar->compileDropAllTables(tables));

    enableForeignKeyConstraints();
}

void MySqlSchemaBuilder::dropAllViews() const
{
    auto query = getAllViews();
    const auto querySize = QueryUtils::queryResultSize(query);

    // Nothing to do, empty result
    if (querySize <= 0)
        return;

    QList<QString> views;
    views.reserve(querySize);

    while (query.next())
        views << query.value(0).value<QString>();

    Q_ASSERT(!views.isEmpty());

    m_connection->statement(m_grammar->compileDropAllViews(views));
}

SqlQuery MySqlSchemaBuilder::getAllTables() const
{
    // TODO schema, use postprocessor processColumnListing() silverqx
    return m_connection->selectFromWriteConnection(m_grammar->compileGetAllTables());
}

SqlQuery MySqlSchemaBuilder::getAllViews() const
{
    return m_connection->selectFromWriteConnection(m_grammar->compileGetAllViews());
}

QStringList MySqlSchemaBuilder::getColumnListing(const QString &table) const
{
    const auto tablePrefixed = NOSPACE.arg(m_connection->getTablePrefix(), table);

    auto query = m_connection->selectFromWriteConnection(
                     m_grammar->compileColumnListing(),
                     {m_connection->getDatabaseName(), tablePrefixed});

    return m_connection->getPostProcessor().processColumnListing(query);
}

bool MySqlSchemaBuilder::hasTable(const QString &table) const
{
    const auto tablePrefixed = NOSPACE.arg(m_connection->getTablePrefix(), table);

    auto query = m_connection->selectFromWriteConnection(
                     m_grammar->compileTableExists(),
                     {m_connection->getDatabaseName(), tablePrefixed});

    return QueryUtils::queryResultSize(query) > 0;
}

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE
