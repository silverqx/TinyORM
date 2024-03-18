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
                 std::shared_ptr<ConnectionResolverInterface> &&connectionResolver,
                 const std::vector<std::shared_ptr<Migration>> &migrations,
                 const std::unordered_map<std::type_index,
                                          MigrationProperties> &migrationsProperties,
                 const QCommandLineParser &parser);
        /*! Virtual destructor. */
        ~Migrator() override = default;

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
            /*! The batch of migrations (identified by batch number) to be reverted. */
            int batch     = 0;
        };

        /*! Run the pending migrations. */
        std::vector<std::shared_ptr<Migration>> run(MigrateOptions options) const;
        /*! Rollback the last migration operation. */
        std::vector<RollbackItem> rollback(MigrateOptions options) const;
        /*! Rolls all of the currently applied migrations back. */
        std::vector<RollbackItem> reset(bool pretend = false) const;

        /* Proxies to MigrationRepository */
        /*! Determine if the migration repository exists. */
        bool repositoryExists() const;
        /*! Determine if any migrations have been run. */
//        bool hasRunAnyMigrations() const;

        /* Getters / Setters */
        /*! Get the migration repository instance. */
        inline MigrationRepository &repository() const noexcept;
        /*! Get migration names list. */
        inline const std::set<QString> &migrationNames() const noexcept;

    protected:
        /* Database connection related */
        /*! Resolve the database connection instance. */
        DatabaseConnection &resolveConnection(const QString &name = "") const;

        /* Migration instances lists and hashes */
        /*! Create a map that maps migration names by migrations type-id (type_index). */
        void createMigrationNamesMap();
        /*! Get a migration name by a migration type-id (from the migration instance). */
        QString getMigrationName(const Migration &migration) const;
        /*! Get a cached migration name by a migration type-id. */
        QString cachedMigrationName(const Migration &migration) const;

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
        /*! Get the migrations for a rollback operation (used by rollback). */
        std::vector<MigrationItem>
        getMigrationsForRollbackByOptions(MigrateOptions options) const;
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
        enum struct MigrateMethod : quint8
        {
            /*! Migrate Up. */
            Up,
            /*! Migrate Down. */
            Down,
        };

        /*! Pretend to run the migrations. */
        void pretendToRun(const Migration &migration, MigrateMethod method) const;
        /*! Get all of the queries that would be run for a migration. */
        QVector<Log> getQueries(const Migration &migration, MigrateMethod method) const;

        /* Migrate up/down common */
        /*! Run a migration inside a transaction if the database supports it. */
        void runMigration(const Migration &migration, MigrateMethod method) const;
        /*! Migrate by the given method (up/down). */
        static void migrateByMethod(const Migration &migration, MigrateMethod method);

        /* Validate migrations */
        /*! Throw if migrations passed to the TomApplication are not sorted
            alphabetically. */
        static void throwIfMigrationsNotSorted(const QString &previousMigrationName,
                                               const QString &migrationName);
        /*! Throw if migrations passed to the TomApplication contain the same Migration
            more times. */
        void throwIfContainMigration(const QString &migrationName) const;
        /*! Throw if migration filename is not valid. */
        static void throwIfMigrationFileNameNotValid(const QString &migrationName);
        /*! Throw if migration class name is not valid. */
        static void
        throwIfMigrationClassNameNotValid(const QString &migrationNameOriginal,
                                          const QString &migrationName);

        /*! The migration repository instance. */
        std::shared_ptr<MigrationRepository> m_repository;
        /*! The database connection resolver instance. */
        std::shared_ptr<ConnectionResolverInterface> m_connectionResolver;

        /*! Reference to the migrations vector to process. */
        std::reference_wrapper<
                const std::vector<std::shared_ptr<Migration>>> m_migrations;
        /*! Reference to the cached migration properties. */
        std::reference_wrapper<
                const std::unordered_map<std::type_index,
                                         MigrationProperties>> m_migrationsProperties;
        /*! Map a migration names by migrations type-id (type_index)
            (used by migrate, rollback, pretend). */
        std::unordered_map<std::type_index, QString> m_migrationNamesMap;
        /*! Migration names list (used by status). */
        std::set<QString> m_migrationNames;
        /*! Map a migration instances by migration names (used by reset). */
        std::unordered_map<QString,
                           std::shared_ptr<Migration>> m_migrationInstancesMap;
    };

    /* public */

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

