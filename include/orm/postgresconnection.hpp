#pragma once
#ifndef ORM_POSTGRESCONNECTION_HPP
#define ORM_POSTGRESCONNECTION_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/databaseconnection.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

    /*! PostgreSql database connection. */
    class SHAREDLIB_EXPORT PostgresConnection final : public DatabaseConnection
    {
        Q_DISABLE_COPY(PostgresConnection)

    public:
        /*! Constructor. */
        explicit PostgresConnection(
                std::function<Connectors::ConnectionName()> &&connection,
                const QString &database = "", const QString &tablePrefix = "",
                const QVariantHash &config = {});
        /*! Virtual destructor. */
        inline ~PostgresConnection() final = default;

        /*! Get a schema builder instance for the connection. */
        std::unique_ptr<SchemaBuilder> getSchemaBuilder() final;

    protected:
        /*! Get the default query grammar instance. */
        std::unique_ptr<QueryGrammar> getDefaultQueryGrammar() const final;
        /*! Get the default schema grammar instance. */
        std::unique_ptr<SchemaGrammar> getDefaultSchemaGrammar() const final;
        /*! Get the default post processor instance. */
        std::unique_ptr<QueryProcessor> getDefaultPostProcessor() const final;
    };

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_POSTGRESCONNECTION_HPP
