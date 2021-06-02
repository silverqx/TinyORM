#include "orm/postgresconnection.hpp"

#include <QtSql/QSqlDriver>

#include "orm/query/grammars/postgresgrammar.hpp"
#include "orm/query/processors/postgresprocessor.hpp"
#include "orm/schema/grammars/postgresschemagrammar.hpp"
#include "orm/schema/postgresschemabuilder.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

PostgresConnection::PostgresConnection(
        const std::function<Connectors::ConnectionName()> &connection,
        const QString &database, const QString &tablePrefix,
        const QVariantHash &config
)
    : DatabaseConnection(connection, database, tablePrefix, config)
{
    /* We need to initialize a query grammar that is a very important part
       of the database abstraction, so we initialize it to the default value
       while starting. */
    useDefaultQueryGrammar();

    useDefaultPostProcessor();
}

std::unique_ptr<SchemaBuilder> PostgresConnection::getSchemaBuilder()
{
    if (!m_schemaGrammar)
        useDefaultSchemaGrammar();

    return std::make_unique<Schema::PostgresSchemaBuilder>(*this);
}

std::unique_ptr<QueryGrammar> PostgresConnection::getDefaultQueryGrammar() const
{
    // Ownership of a unique_ptr()
    auto grammar = std::make_unique<Query::Grammars::PostgresGrammar>();

    withTablePrefix(*grammar);

    return grammar;
}

std::unique_ptr<SchemaGrammar> PostgresConnection::getDefaultSchemaGrammar() const
{
    // Ownership of a unique_ptr()
    auto grammar = std::make_unique<Schema::Grammars::PostgresSchemaGrammar>();

    withTablePrefix(*grammar);

    return grammar;
}

std::unique_ptr<QueryProcessor> PostgresConnection::getDefaultPostProcessor() const
{
    return std::make_unique<Query::Processors::PostgresProcessor>();
}

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
