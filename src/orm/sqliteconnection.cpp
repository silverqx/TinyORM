#include "orm/sqliteconnection.hpp"

#include "orm/query/grammars/sqlitegrammar.hpp"
#include "orm/query/processors/sqliteprocessor.hpp"
#include "orm/schema/grammars/sqliteschemagrammar.hpp"
#include "orm/schema/sqliteschemabuilder.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

/* public */

SQLiteConnection::SQLiteConnection(
        std::function<Connectors::ConnectionName()> &&connection,
        const QString &database, const QString &tablePrefix,
        const QVariantHash &config
)
    : DatabaseConnection(std::move(connection), database, tablePrefix, config)
{
    /* We need to initialize a query grammar that is a very important part
       of the database abstraction, so we initialize it to the default value
       while starting. */
    useDefaultQueryGrammar();

    useDefaultPostProcessor();
}

std::unique_ptr<SchemaBuilder> SQLiteConnection::getSchemaBuilder()
{
    if (!m_schemaGrammar)
        useDefaultSchemaGrammar();

    return std::make_unique<SchemaNs::SQLiteSchemaBuilder>(*this);
}

/* protected */

std::unique_ptr<QueryGrammar> SQLiteConnection::getDefaultQueryGrammar() const
{
    // Ownership of a unique_ptr()
    auto grammar = std::make_unique<Query::Grammars::SQLiteGrammar>();

    withTablePrefix(*grammar);

    return grammar;
}

std::unique_ptr<SchemaGrammar> SQLiteConnection::getDefaultSchemaGrammar() const
{
    // Ownership of a unique_ptr()
    auto grammar = std::make_unique<SchemaNs::Grammars::SQLiteSchemaGrammar>();

    withTablePrefix(*grammar);

    return grammar;
}

std::unique_ptr<QueryProcessor> SQLiteConnection::getDefaultPostProcessor() const
{
    return std::make_unique<Query::Processors::SQLiteProcessor>();
}

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE
