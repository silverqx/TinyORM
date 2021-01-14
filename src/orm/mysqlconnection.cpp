#include "orm/mysqlconnection.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

MySqlConnection::MySqlConnection(
        const std::function<Connectors::ConnectionName()> &connection,
        const QString &database, const QString tablePrefix,
        const QVariantHash &config
)
    : DatabaseConnection(connection, database, tablePrefix, config)
{}

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
