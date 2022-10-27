#include "orm/postgresconnection.hpp"

#include <QtSql/QSqlDriver>

#include "orm/query/grammars/postgresgrammar.hpp"
#include "orm/query/processors/postgresprocessor.hpp"
#include "orm/schema/grammars/postgresschemagrammar.hpp"
#include "orm/schema/postgresschemabuilder.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

/* public */

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

std::unique_ptr<SchemaBuilder> PostgresConnection::getSchemaBuilder()
{
    if (!m_schemaGrammar)
        useDefaultSchemaGrammar();

    return std::make_unique<SchemaNs::PostgresSchemaBuilder>(*this);
}

/* protected */

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
    auto grammar = std::make_unique<SchemaNs::Grammars::PostgresSchemaGrammar>();

    withTablePrefix(*grammar);

    return grammar;
}

std::unique_ptr<QueryProcessor> PostgresConnection::getDefaultPostProcessor() const
{
    return std::make_unique<Query::Processors::PostgresProcessor>();
}

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE
