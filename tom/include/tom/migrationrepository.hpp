#pragma once
#ifndef TOM_MIGRATIONREPOSITORY_HPP
#define TOM_MIGRATIONREPOSITORY_HPP

#include <map>
#include <optional>
#include <vector>

#include <orm/connectionresolverinterface.hpp>

#include "tom/tomtypes.hpp"

class QVariant; // clazy:exclude=qt6-fwd-fixes

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Query
{
    class Builder;
}
namespace Orm::Types
{
    class SqlQuery;
}

namespace Tom
{

    /*! Migrations database repository. */
    class MigrationRepository
    {
        Q_DISABLE_COPY(MigrationRepository)

        /*! Alias for the ConnectionResolverInterface. */
        using ConnectionResolverInterface = Orm::ConnectionResolverInterface;
        /*! Alias for the DatabaseConnection. */
        using DatabaseConnection = Orm::DatabaseConnection;
        /*! Alias for the QueryBuilder. */
        using QueryBuilder = Orm::Query::Builder;
        /*! Alias for the SqlQuery. */
        using SqlQuery = Orm::Types::SqlQuery;

    public:
        /*! Constructor. */
        MigrationRepository(
                std::shared_ptr<ConnectionResolverInterface> &&connectionResolver,
                QString table);
        /*! Default destructor. */
        inline ~MigrationRepository() = default;

        /*! Get the completed migrations (only migration names using pluck). */
        QVector<QVariant> getRanSimple() const;
        /*! Get the completed migrations. */
        std::vector<MigrationItem> getRan(const QString &order) const;
        /*! Get list of migrations. */
        std::vector<MigrationItem> getMigrations(int steps) const;
        /*! Get the last migration batch. */
        std::vector<MigrationItem> getLast() const;
        /*! Get the completed migrations with their batch numbers. */
        std::map<QString, QVariant> getMigrationBatches() const;
        /*! Log that a migration was run. */
        void log(const QString &file, int batch) const;
        /*! Remove a migration from the log. */
        void deleteMigration(quint64 id) const;
        /*! Get the next migration batch number. */
        int getNextBatchNumber() const;
        /*! Get the last migration batch number. */
        int getLastBatchNumber() const;
        /*! Create the migration repository data store. */
        void createRepository() const;
        /*! Drop the migration repository data store if exists. */
        void dropRepositoryIfExists() const;
        /*! Determine if the migration repository exists. */
        bool repositoryExists() const;
        /*! Delete the migration repository data store. */
        void deleteRepository() const;

        /*! Resolve the database connection instance. */
        DatabaseConnection &connection() const;
        /*! Set the connection name to use in the repository. */
        inline void setConnection(const QString &name) noexcept;

    protected:
        /*! Get a query builder for the migration table. */
        std::shared_ptr<QueryBuilder> table() const;

        /*! Hydrate a vector of migration items from a raw SqlQuery. */
        std::vector<MigrationItem> hydrateMigrations(SqlQuery &query) const;

        /*! The database connection resolver instance. */
        std::shared_ptr<ConnectionResolverInterface> m_connectionResolver;
        /*! The name of the migration table. */
        QString m_table;
        /*! The name of the database connection to use. */
        QString m_connection {};
    };

    /* public */

    void MigrationRepository::setConnection(const QString &name) noexcept
    {
        m_connection = name;
    }

} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_MIGRATIONREPOSITORY_HPP
