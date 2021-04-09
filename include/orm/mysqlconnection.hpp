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

        /*! Get a schema builder instance for the connection. */
        std::unique_ptr<SchemaBuilder> getSchemaBuilder() override;

        /*! Determine if the connected database is a MariaDB database. */
        inline bool isMaria();

        /*! Check database connection and show warnings when the state changed. */
        bool pingDatabase() override;

    protected:
        /*! Get the default query grammar instance. */
        std::unique_ptr<QueryGrammar> getDefaultQueryGrammar() const override;
        /*! Get the default schema grammar instance. */
        std::unique_ptr<SchemaGrammar> getDefaultSchemaGrammar() const override;
        /*! Get the default post processor instance. */
        std::unique_ptr<QueryProcessor> getDefaultPostProcessor() const override;

        /*! If the connected database is a MariaDB database. */
        std::optional<bool> m_isMaria;
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // MYSQLCONNECTION_HPP
