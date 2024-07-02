#include "orm/schema/postgresschemabuilder.hpp"

#include <QSet>

#include "orm/exceptions/searchpathemptyerror.hpp"
#include "orm/postgresconnection.hpp"
#include "orm/schema/grammars/postgresschemagrammar.hpp"
#include "orm/utils/query.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using QueryUtils = Orm::Utils::Query;

namespace Orm::SchemaNs
{

/* public */

std::optional<SqlQuery> PostgresSchemaBuilder::createDatabase(const QString &name) const
{
    return m_connection->unprepared(
                m_grammar->compileCreateDatabase(name, *m_connection));
}

std::optional<SqlQuery>
PostgresSchemaBuilder::dropDatabaseIfExists(const QString &name) const
{
    return m_connection->unprepared(
                m_grammar->compileDropDatabaseIfExists(name));
}

void PostgresSchemaBuilder::dropAllTables() const
{
    auto query = getAllTables();
    const auto querySize = QueryUtils::queryResultSize(query);

    // Nothing to do, empty result
    if (querySize <= 0)
        return;

    QList<QString> tables;
    tables.reserve(querySize);

    while (query.next())
        if (auto [tableUnqualified, tableQualified] = columnValuesForDrop(query);
            !excludedTables().intersects(
                grammar().escapeNames(QSet<QString> {tableUnqualified, tableQualified}))
        )
            tables << (tableQualified.isEmpty() ? std::move(tableUnqualified)
                                                : std::move(tableQualified));

    // This can happen if the getAllTables() returns only excluded tables
    if (tables.isEmpty())
        return;

    m_connection->unprepared(m_grammar->compileDropAllTables(tables));
}

void PostgresSchemaBuilder::dropAllViews() const
{
    auto query = getAllViews();
    const auto querySize = QueryUtils::queryResultSize(query);

    // Nothing to do, empty result
    if (querySize <= 0)
        return;

    QList<QString> views;
    views.reserve(querySize);

    while (query.next())
        if (auto [viewUnqualified, viewQualified] = columnValuesForDrop(query);
            !excludedViews().intersects(
                grammar().escapeNames(QSet<QString> {viewUnqualified, viewQualified}))
        )
            views << (viewQualified.isEmpty() ? std::move(viewUnqualified)
                                              : std::move(viewQualified));

    // This can happen if the getAllViews() returns only excluded views
    if (views.isEmpty())
        return;

    m_connection->unprepared(m_grammar->compileDropAllViews(views));
}

SqlQuery PostgresSchemaBuilder::getAllTables() const
{
    auto searchPathList = searchPath();

    // Move to the vector (toList() uses copy)
    QList<QString> searchPath;
    searchPath.reserve(searchPathList.size());
    std::ranges::move(searchPathList, std::back_inserter(searchPath));

    // TODO schema, use postprocessor processColumnListing() silverqx
    return m_connection->selectFromWriteConnection(
                m_grammar->compileGetAllTables(searchPath));
}

SqlQuery PostgresSchemaBuilder::getAllViews() const
{
    auto searchPathList = searchPath();

    // Move to the vector (toList() uses copy)
    QList<QString> searchPath;
    searchPath.reserve(searchPathList.size());
    std::ranges::move(searchPathList, std::back_inserter(searchPath));

    return m_connection->selectFromWriteConnection(
                m_grammar->compileGetAllViews(searchPath));
}

QStringList PostgresSchemaBuilder::getColumnListing(const QString &table) const
{
    const auto [database, schema, table_] = parseSchemaAndTable(table);

    const auto tablePrefixed = NOSPACE.arg(m_connection->getTablePrefix(), table_);

    auto query = m_connection->selectFromWriteConnection(
                     m_grammar->compileColumnListing(),
                     {database, schema, tablePrefixed});

    return m_connection->getPostProcessor().processColumnListing(query);
}

bool PostgresSchemaBuilder::hasTable(const QString &table) const
{
    const auto [database, schema, table_] = parseSchemaAndTable(table);

    const auto tablePrefixed = NOSPACE.arg(m_connection->getTablePrefix(), table_);

    auto query = m_connection->selectFromWriteConnection(
                     m_grammar->compileTableExists(),
                     {database, schema, tablePrefixed});

    return QueryUtils::queryResultSize(query) > 0;
}

/* protected */

std::tuple<QString, QString, QString>
PostgresSchemaBuilder::parseSchemaAndTable(const QString &reference) const
{
    auto parts = reference.split(DOT, Qt::KeepEmptyParts);
    Q_ASSERT(!parts.isEmpty() && parts.size() <= 3);

    auto database = m_connection->getConfig(database_).value<QString>();
    const auto &connection = m_connection->getName();

    /* Drop the database name as it can't be different than the database for the current
       connection anyway. Also throw if the database name differs from a database
       defined in the configuration. */
    dropDatabaseForParse(database, parts, connection);

    /* We will use the default schema unless the schema has been specified in the
       query. If the schema has been specified in the query then we can use it
       instead of a default PostgeSQL's database search_path. */
    auto schema = getSchemaForParse(parts, connection);

    Q_ASSERT(parts.size() == 1);

    return {std::move(database), std::move(schema), std::move(parts.first())};
}

/* private */

void PostgresSchemaBuilder::dropDatabaseForParse(
        const QString &databaseConfig, QStringList &parts, const QString &connection)
{
    // Nothing to drop, a database name was not passed in the query
    if (parts.size() != 3)
        return;

    throwIfDatabaseDiffers(databaseConfig, parts, connection);
}

void PostgresSchemaBuilder::throwIfDatabaseDiffers(
        const QString &databaseConfig, QStringList &parts, const QString &connection)
{
    // Drop the database name from the 'parts' vector
    auto database = parts.takeFirst();

    if (databaseConfig == database)
        return;

    throw Exceptions::InvalidArgumentError(
                QStringLiteral(
                    "The database '%1' name in the fully qualified table name differs "
                    "from the database name '%2' defined in the PostgreSQL "
                    "configuration '%3' in %4().")
                .arg(std::move(database), databaseConfig, connection, __tiny_func__));
}

QString PostgresSchemaBuilder::getSchemaForParse(QStringList &parts,
                                                 const QString &connection) const
{
    // The schema was passed as part of the table name (qualified table name)
    if (parts.size() == 2)
        return parts.takeFirst();

    // Get the PostgreSQL server search_path for the current connection
    auto searchPathList = searchPath();

    // Throw if the database search_path is empty
    throwIfEmptySearchPath(searchPathList, connection);

    // Get the first/default schema name
    return std::move(searchPathList.first());
}

void PostgresSchemaBuilder::throwIfEmptySearchPath(const QStringList &searchPath,
                                                   const QString &connection)
{
    if (!isSearchPathEmpty(searchPath))
        return;

    throw Exceptions::SearchPathEmptyError(
                QStringLiteral(
                    "The PostgreSQL 'search_path' connection configuration option is "
                    "empty, please provide the fully qualified table name during "
                    "the PostgresSchemaBuilder's getColumnListing() or hasTable() "
                    "method calls or set the 'search_path' option in the PostgreSQL "
                    "configuration '%1', in %2().")
                .arg(connection, __tiny_func__));
}

QSet<QString> PostgresSchemaBuilder::excludedTables() const
{
    /* ConnectionFactory provides the default 'spatial_ref_sys' value for the 'dont_drop'
       configuration option for the QPSQL driver. */
    const auto excludedTablesList = grammar().escapeNames(
                                        parseSearchPath(
                                            m_connection->getConfig(dont_drop)));

    return {excludedTablesList.constBegin(), excludedTablesList.constEnd()};
}

const QSet<QString> &PostgresSchemaBuilder::excludedViews() const
{
    /* For these, the PostgreSQL server throws that they are needed by the postgis
       extension and proposes to delete the postgis extension instead, so exclude them.
       This is happening during dropping of all views. */
    static const auto cached(grammar().escapeNames(
                                 QSet<QString> {QStringLiteral("geography_columns"),
                                                QStringLiteral("geometry_columns")}));
    return cached;
}

std::tuple<QString, QString>
PostgresSchemaBuilder::columnValuesForDrop(TSqlQuery &query)
{
    return {query.value(0).value<QString>(),
            query.value(QStringLiteral("qualifiedname"))
                 .value<QString>()};
}

QStringList PostgresSchemaBuilder::searchPath() const
{
    return dynamic_cast<PostgresConnection &>(*m_connection).searchPath();
}

const Grammars::PostgresSchemaGrammar &PostgresSchemaBuilder::grammar() const
{
    return dynamic_cast<const Grammars::PostgresSchemaGrammar &>(*m_grammar);
}

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE
