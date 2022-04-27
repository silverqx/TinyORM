#pragma once
#ifndef TOM_MIGRATOR_HPP
#define TOM_MIGRATOR_HPP

#include <set>
#include <typeindex>

#include <orm/connectionresolverinterface.hpp>
#include <orm/types/log.hpp>

#include "tom/concerns/interactswithio.hpp"
#include "tom/tomtypes.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
{

    class MigrationRepository;

    /*! Migration service class. */
    class Migrator : public Concerns::InteractsWithIO
    {
        Q_DISABLE_COPY(Migrator)

        /*! Alias for the ConnectionResolverInterface. */
        using ConnectionResolverInterface = Orm::ConnectionResolverInterface;
        /*! Alias for the DatabaseConnection. */
        using DatabaseConnection = Orm::DatabaseConnection;
        /*! Alias for the pretend Log. */
        using Log = Orm::Types::Log;

    public:
        /*! Constructor. */
        Migrator(std::shared_ptr<MigrationRepository> &&repository,
                 std::shared_ptr<ConnectionResolverInterface> &&resolver,
                 const std::vector<std::shared_ptr<Migration>> &migrations,
                 const QCommandLineParser &parser);
        /*! Virtual destructor. */
        inline ~Migrator() override = default;

        /* Main migrate operations */
        /*! Migrate options. */
        struct MigrateOptions
        {
            /*! Dump the SQL queries that would be run. */
            bool pretend  = false;
            /*! Force the migrations to be run so they can be rolled back individually. */
            bool step     = false;
            /*! The number of migrations to be reverted. */
            int stepValue = 0;
        };

        /*! Run the pending migrations. */
        std::vector<std::shared_ptr<Migration>> run(MigrateOptions options) const;
        /*! Rollback the last migration operation. */
        std::vector<RollbackItem> rollback(MigrateOptions options) const;
        /*! Rolls all of the currently applied migrations back. */
        std::vector<RollbackItem> reset(bool pretend = false) const;

        /* Database connection related */
        /*! Execute the given callback using the given connection as the default
            connection. */
        int usingConnection(QString &&name, bool debugSql,
                            std::function<int()> &&callback);

        /* Proxies to MigrationRepository */
        /*! Determine if the migration repository exists. */
        bool repositoryExists() const;
        /*! Determine if any migrations have been run. */
        bool hasRunAnyMigrations() const;

        /* Getters / Setters */
        /*! Get the default connection name. */
        inline const QString &getConnection() const noexcept;
        /*! Set the default connection name. */
        void setConnection(QString &&name, std::optional<bool> &&debugSql);
        /*! Get the migration repository instance. */
        inline MigrationRepository &repository() const noexcept;
        /*! Get migration names list. */
        inline const std::set<QString> &migrationNames() const noexcept;

    protected:
        /* Database connection related */
        /*! Resolve the database connection instance. */
        DatabaseConnection &resolveConnection(const QString &name = "") const;
        /*! Get the debug sql by the connection name. */
        std::optional<bool> getConnectionDebugSql(const QString &name) const;

        /* Migration instances lists and hashes */
        /*! Create a map that maps migration names by migrations type-id (type_index). */
        void createMigrationNamesMap();
        /*! Get a migration name by a migration type-id. */
        QString getMigrationName(const Migration &migration) const;

        /* Migrate */
        /*! Get the migration instances that have not yet run. */
        std::vector<std::shared_ptr<Migration>>
        pendingMigrations(const QVector<QVariant> &ran) const;
        /*! Run "up" a migration instance. */
        void runUp(const Migration &migration, int batch, bool pretend) const;

        /* Rollback */
        /*! Get the migrations for a rollback operation (used by rollback). */
        std::vector<RollbackItem>
        getMigrationsForRollback(MigrateOptions options) const;
        /*! Get the migrations for a rollback operation (used by reset). */
        std::vector<RollbackItem>
        getMigrationsForRollback(std::vector<MigrationItem> &&ran) const;

        /*! Rollback the given migrations. */
        std::vector<RollbackItem>
        rollbackMigrations(std::vector<RollbackItem> &&migrations, bool pretend) const;
        /*! Run "down" a migration instance. */
        void runDown(const RollbackItem &migrationToRollback, bool pretend) const;

        /* Pretend */
        /*! Migrate type (up/down). */
        enum struct MigrateMethod { Up, Down };

        /*! Pretend to run the migrations. */
        void pretendToRun(const Migration &migration, MigrateMethod method) const;
        /*! Get all of the queries that would be run for a migration. */
        QVector<Log> getQueries(const Migration &migration, MigrateMethod method) const;

        /* Migrate up/down common */
        /*! Run a migration inside a transaction if the database supports it. */
        void runMigration(const Migration &migration, MigrateMethod method) const;
        /*! Migrate by the given method (up/down). */
        void migrateByMethod(const Migration &migration, MigrateMethod method) const;

        /*! Throw if migrations passed to the TomApplication are not sorted
            alphabetically. */
        void throwIfMigrationsNotSorted(const QString &previousMigrationName,
                                        const QString &migrationName) const;

        /*! The migration repository instance. */
        std::shared_ptr<MigrationRepository> m_repository;
        /*! The database connection resolver instance. */
        std::shared_ptr<ConnectionResolverInterface> m_resolver;
        /*! The name of the database connection to use. */
        QString m_connection {};

        /*! Reference to the migrations vector to process. */
        const std::vector<std::shared_ptr<Migration>> &m_migrations;
        /*! Map a migration names by migrations type-id (type_index)
            (used migrate, rollback, pretend). */
        std::unordered_map<std::type_index, QString> m_migrationNamesMap {};
        /*! Migration names list (used by status). */
        std::set<QString> m_migrationNames {};
        /*! Map a migration instances by migration names (used by reset). */
        std::unordered_map<QString,
                           std::shared_ptr<Migration>> m_migrationInstancesMap {};
    };

    /* public */

    const QString &Migrator::getConnection() const noexcept
    {
        return m_connection;
    }

    MigrationRepository &Migrator::repository() const noexcept
    {
        return *m_repository;
    }

    const std::set<QString> &Migrator::migrationNames() const noexcept
    {
        return m_migrationNames;
    }

} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_MIGRATOR_HPP

