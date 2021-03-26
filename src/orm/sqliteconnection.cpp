#include "orm/sqliteconnection.hpp"

#include "orm/query/grammars/sqlitegrammar.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

SQLiteConnection::SQLiteConnection(
        const std::function<Connectors::ConnectionName()> &connection,
        const QString &database, const QString tablePrefix,
        const QVariantHash &config
)
    : DatabaseConnection(connection, database, tablePrefix, config)
{
    /* We need to initialize a query grammar that is a very important part
       of the database abstraction, so we initialize it to the default value
       while starting. */
    useDefaultQueryGrammar();
}

std::unique_ptr<QueryGrammar> SQLiteConnection::getDefaultQueryGrammar() const
{
    return std::make_unique<Query::Grammars::SQLiteGrammar>();
}

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
