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

        /*! Get the default query grammar instance. */
        std::unique_ptr<QueryGrammar> getDefaultQueryGrammar() const override;

        /*! Determine if the connected database is a MariaDB database. */
        inline bool isMaria();

        /*! Check database connection and show warnings when the state changed. */
        bool pingDatabase() override;

    protected:
        /*! If the connected database is a MariaDB database. */
        std::optional<bool> m_isMaria;
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // MYSQLCONNECTION_HPP
