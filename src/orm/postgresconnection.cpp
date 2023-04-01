#include "orm/postgresconnection.hpp"

#include <range/v3/view/move.hpp>

#include "orm/query/grammars/postgresgrammar.hpp"
#include "orm/query/processors/postgresprocessor.hpp"
#include "orm/schema/grammars/postgresschemagrammar.hpp"
#include "orm/schema/postgresschemabuilder.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

/* private */

PostgresConnection::PostgresConnection(
        std::function<Connectors::ConnectionName()> &&connection,
        QString &&database, QString &&tablePrefix, QtTimeZoneConfig &&qtTimeZone,
        QVariantHash &&config
)
    : DatabaseConnection(
          std::move(connection), std::move(database), std::move(tablePrefix),
          std::move(qtTimeZone), std::move(config))
{
    /* We need to initialize a query grammar that is a very important part
       of the database abstraction, so we initialize it to the default value
       while starting. */
    useDefaultQueryGrammar();

    useDefaultPostProcessor();
}

/* public */

/* Getters */

QStringList PostgresConnection::searchPath(const bool flushCache)
{
    auto searchPathRaw = this->searchPathRaw(flushCache);

    // Don't user the searchPathRaw.contains(username) here, any performance gain from it

    const auto username = std::as_const(m_config)[username_].value<QString>();
    Q_ASSERT(!username.isEmpty());

    // Resolve the $user variable
    return ranges::views::move(searchPathRaw)
            | ranges::views::transform([&username](QString &&schema)
    {
        // Don't use the ternary operator here so the std::move() can apply
        if (schema == QStringLiteral("$user"))
            return username; // NOLINT(performance-no-automatic-move)

        return std::move(schema);
    })
            | ranges::to<QStringList>();
}

QStringList PostgresConnection::searchPathRaw(const bool flushCache)
{
    /* The pretending mode doesn't use the cached value and the search_path from
       the real database; it always uses the 'search_path' configuration option or
       it returns the public schema if the 'search_path' is not defined. */
    if (m_pretending)
        return searchPathRawForPretending();

    // Return the cached value
    if (!flushCache && m_searchPath)
        return *m_searchPath;

    /* Obtain and cache the 'search_path' for the current connection. Caching avoids
       slow database querying. */
    return *(m_searchPath = searchPathRawDb());
}

/* protected */

std::unique_ptr<QueryGrammar> PostgresConnection::getDefaultQueryGrammar() const
{
    // Ownership of a unique_ptr()
    auto grammar = std::make_unique<Query::Grammars::PostgresGrammar>();

    withTablePrefix(*grammar);

    return grammar;
}

std::unique_ptr<SchemaGrammar> PostgresConnection::getDefaultSchemaGrammar()
{
    // Ownership of a unique_ptr()
    auto grammar = std::make_unique<SchemaNs::Grammars::PostgresSchemaGrammar>();

    withTablePrefix(*grammar);

    return grammar;
}

std::unique_ptr<SchemaBuilder> PostgresConnection::getDefaultSchemaBuilder()
{
    return std::make_unique<SchemaNs::PostgresSchemaBuilder>(shared_from_this());
}

std::unique_ptr<QueryProcessor> PostgresConnection::getDefaultPostProcessor() const
{
    return std::make_unique<Query::Processors::PostgresProcessor>();
}

/* private */

QStringList PostgresConnection::searchPathRawForPretending() const
{
    /* The default value is the 'public' schema if pretending and the connection
       configuration doesn't contain the search_path. Using the public schema in this
       case is totally ok. */
    if (!hasConfig(search_path))
        return {PUBLIC};

    /* Obtain the search_path from the connection configuration, the type is already
       validated in the PostgreSQL's configuration parser (QString or QStringList). */
    return parseSearchPath(getConfig(search_path));
}

QStringList PostgresConnection::searchPathRawDb()
{
    auto query = unprepared(QStringLiteral("show search_path"));

    [[maybe_unused]]
    const auto ok = query.first();
    Q_ASSERT(ok);
    Q_ASSERT(query.isValid());

    const auto searchPath = query.value(search_path).value<QString>();
    Q_ASSERT(!searchPath.isEmpty());

    return parseSearchPath(searchPath);
}

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE
