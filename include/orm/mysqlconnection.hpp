#ifndef MYSQLCONNECTION_HPP
#define MYSQLCONNECTION_HPP

#include "orm/databaseconnection.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

    class SHAREDLIB_EXPORT MySqlConnection final : public DatabaseConnection
    {
    public:
        MySqlConnection(
                const std::function<Connectors::ConnectionName()> &connection,
                const QString &database = "", const QString tablePrefix = "",
                const QVariantHash &config = {});
        inline virtual ~MySqlConnection() = default;
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // MYSQLCONNECTION_HPP
