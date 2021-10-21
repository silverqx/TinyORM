#pragma once
#ifndef ORM_MYSQLCONNECTION_HPP
#define ORM_MYSQLCONNECTION_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/databaseconnection.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

    /*! MySql database connection. */
    class SHAREDLIB_EXPORT MySqlConnection final : public DatabaseConnection
    {
        Q_DISABLE_COPY(MySqlConnection)

    public:
        /*! Constructor. */
        MySqlConnection(
                std::function<Connectors::ConnectionName()> &&connection,
                const QString &database = "", const QString &tablePrefix = "",
                const QVariantHash &config = {});
        /*! Virtual destructor. */
        inline virtual ~MySqlConnection() = default;

        /*! Get a schema builder instance for the connection. */
        std::unique_ptr<SchemaBuilder> getSchemaBuilder() override;

        /*! Determine if the connected database is a MariaDB database. */
        bool isMaria();

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

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_MYSQLCONNECTION_HPP
