#include "orm/sqliteconnection.hpp"

#include "orm/query/grammars/sqlitegrammar.hpp"
#include "orm/query/processors/sqliteprocessor.hpp"
#include "orm/schema/grammars/sqliteschemagrammar.hpp"
#include "orm/schema/sqliteschemabuilder.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

/* private */

SQLiteConnection::SQLiteConnection(
        std::function<Connectors::ConnectionName()> &&connection,
        QString &&database, QString &&tablePrefix, QtTimeZoneConfig &&qtTimeZone,
        const std::optional<bool> returnQDateTime, QVariantHash &&config
)
    : DatabaseConnection(
          std::move(connection), std::move(database), std::move(tablePrefix),
          std::move(qtTimeZone), returnQDateTime,     std::move(config))
{
    /* We need to initialize a query grammar that is a very important part
       of the database abstraction, so we initialize it to the default value
       while starting. */
    useDefaultQueryGrammar();

    useDefaultPostProcessor();
}

/* public */

SQLiteConnection &SQLiteConnection::setReturnQDateTime(const bool value)
{
    m_returnQDateTime = value;

#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(push)
#  pragma warning(disable : 26815)
#endif
    return *this;
#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(pop)
#endif
}

/* protected */

std::unique_ptr<QueryGrammar> SQLiteConnection::getDefaultQueryGrammar() const
{
    // Ownership of a unique_ptr()
    auto grammar = std::make_unique<Query::Grammars::SQLiteGrammar>();

    withTablePrefix(*grammar);

    return grammar;
}

std::unique_ptr<SchemaGrammar> SQLiteConnection::getDefaultSchemaGrammar()
{
    // Ownership of a unique_ptr()
    auto grammar = std::make_unique<SchemaNs::Grammars::SQLiteSchemaGrammar>();

    withTablePrefix(*grammar);

    return grammar;
}

std::unique_ptr<SchemaBuilder> SQLiteConnection::getDefaultSchemaBuilder()
{
    return std::make_unique<SchemaNs::SQLiteSchemaBuilder>(shared_from_this());
}

std::unique_ptr<QueryProcessor> SQLiteConnection::getDefaultPostProcessor() const
{
    return std::make_unique<Query::Processors::SQLiteProcessor>();
}

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE
