#include "tom/migrator.hpp"

#include <typeinfo>

#include <orm/databaseconnection.hpp>
#include <orm/utils/query.hpp>
#include <orm/utils/type.hpp>

#include <range/v3/algorithm/contains.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/move.hpp>
#include <range/v3/view/reverse.hpp>

#include "tom/exceptions/invalidtemplateargumenterror.hpp"
#include "tom/exceptions/runtimeerror.hpp"
#include "tom/migration.hpp"
#include "tom/migrationrepository.hpp"
#include "tom/tomutils.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::DatabaseConnection;

using Orm::Constants::DESC;
using Orm::Constants::UNDERSCORE;

using QueryUtils = Orm::Utils::Query;
using TypeUtils = Orm::Utils::Type;

using TomUtils = Tom::Utils;

namespace Tom
{

/* public */

Migrator::Migrator(
        std::shared_ptr<MigrationRepository> &&repository,
        std::shared_ptr<ConnectionResolverInterface> &&connectionResolver,
        const std::vector<std::shared_ptr<Migration>> &migrations,
        const std::unordered_map<std::type_index,
                                 MigrationProperties> &migrationsProperties,
        const QCommandLineParser &parser
)
    : Concerns::InteractsWithIO(parser)
    , m_repository(std::move(repository))
    , m_connectionResolver(std::move(connectionResolver))
    , m_migrations(migrations)
    , m_migrationsProperties(migrationsProperties)
{
    /* Initialize these containers in the constructor as every command that uses
       the Migrator also needs a migrations map or names list. */
    createMigrationNamesMap();
}

/* Main migrate operations */

std::vector<std::shared_ptr<Migration>> Migrator::run(const MigrateOptions options) const
{
    auto migrations = pendingMigrations(m_repository->getRanSimple());

    /* First we will just make sure that there are any migrations to run. If there
       aren't, we will just make a note of it to the developer so they're aware
       that all of the migrations have been run against this database system. */
    if (migrations.empty()) {
        info(QStringLiteral("Nothing to migrate."));

        return migrations;
    }

    /* Next, we will get the next batch number for the migrations so we can insert
       correct batch number in the database migrations repository when we store
       each migration's execution. We will also extract a few of the options. */
    auto batch = m_repository->getNextBatchNumber();

    const auto &[pretend, step, unused1, unused2] = options;

    /* Once we have the vector of migrations, we will spin through them and run the
       migrations "up" so the changes are made to the databases. We'll then log
       that the migration was run so we don't repeat it next time we execute. */
    for (const auto &migration : migrations) {
        runUp(*migration, batch, pretend);

        if (step)
            ++batch;
    }

    return migrations;
}

std::vector<RollbackItem>
Migrator::rollback(const MigrateOptions options) const
{
    /* We want to pull in the last batch of migrations that ran on the previous
       migration operation. We'll then reverse those migrations and run each
       of them "down" to reverse the last migration "operation" which ran. */
    return rollbackMigrations(getMigrationsForRollback(options), options.pretend);
}

std::vector<RollbackItem> Migrator::reset(const bool pretend) const
{
    /* Reverse all the ran migrations list to reset this database. This will allow us
       to get the database back into its "empty" state, ready for the migrations. */
    return rollbackMigrations(getMigrationsForRollback(m_repository->getRan(DESC)),
                              pretend);
}

/* Proxies to MigrationRepository */

bool Migrator::repositoryExists() const
{
    return m_repository->repositoryExists();
}

//bool Migrator::hasRunAnyMigrations() const
//{
//    return repositoryExists() && !m_repository->getRanSimple().isEmpty();
//}

/* protected */

/* Database connection related */

DatabaseConnection &Migrator::resolveConnection(const QString &name) const
{
    return m_connectionResolver->connection(name);
}

/* Migration instances lists and hashes */

void Migrator::createMigrationNamesMap()
{
    QString previousMigrationName;

    for (const auto &migration : m_migrations.get()) {
        // To avoid expression with side effects in the typeid ()
        const auto &migrationRef = *migration;

        auto migrationName = getMigrationName(migrationRef);

        // Verify duplicate Migration, the same Migration can't be passed more times
        throwIfContainMigration(migrationName);

        // Verify alphabetical sorting
        throwIfMigrationsNotSorted(previousMigrationName, migrationName);
        previousMigrationName = migrationName;

        m_migrationNamesMap.try_emplace(std::type_index(typeid (migrationRef)),
                                        migrationName);

        m_migrationInstancesMap.try_emplace(migrationName, migration);

        m_migrationNames.emplace(std::move(migrationName));
    }
}

QString Migrator::getMigrationName(const Migration &migration) const
{
    /* Migration name from the T_MIGRATION macro */
    {
        const auto &migrationName = m_migrationsProperties.get()
                                    .at(typeid (migration)).name;

        throwIfMigrationFileNameNotValid(migrationName);

        if (!migrationName.isEmpty())
            return migrationName;
    }

    /* Migration name from the migration type-id */
    {
        const auto migrationNameOriginal = TypeUtils::classPureBasename(migration, false);

        /* sliced(1) or mid(1) to remove the '_' at the beginning, it doesn't matter if
           it starts with the '_' char, it will be validated by the throw method below. */

        auto migrationName = migrationNameOriginal.startsWith(UNDERSCORE)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                             ? migrationNameOriginal.sliced(1)
#else
                             ? migrationNameOriginal.mid(1)
#endif
                             : migrationNameOriginal;

        throwIfMigrationClassNameNotValid(migrationNameOriginal, migrationName);

        return migrationName;
    }
}

QString Migrator::cachedMigrationName(const Migration &migration) const
{
    const auto &migrationTypeId = typeid (migration);

    Q_ASSERT(m_migrationNamesMap.contains(migrationTypeId));

    return m_migrationNamesMap.at(migrationTypeId);
}

/* Migrate */

std::vector<std::shared_ptr<Migration>>
Migrator::pendingMigrations(const QVector<QVariant> &ran) const
{
    return m_migrations.get()
            | ranges::views::remove_if([this, &ran](const auto &migration)
    {
        return ran.contains(cachedMigrationName(*migration));
    })
            | ranges::to<std::vector<std::shared_ptr<Migration>>>();
}

void Migrator::runUp(const Migration &migration, const int batch,
                     const bool pretend) const
{
    if (pretend)
        return pretendToRun(migration, MigrateMethod::Up); // clazy:exclude=returning-void-expression

    const auto migrationName = cachedMigrationName(migration);

    comment(QStringLiteral("Migrating: "), false).note(migrationName);

    QElapsedTimer timer;
    timer.start();

    runMigration(migration, MigrateMethod::Up);

    const auto elapsedTime = timer.elapsed();

    /* Once we have run a migrations class, we will log that it was run in this
       repository so that we don't try to run it next time we do a migration
       in the application. A migration repository keeps the migrate order. */
    m_repository->log(migrationName, batch);

    info(QStringLiteral("Migrated:"), false);
    note(QStringLiteral("  %1 (%2ms)").arg(migrationName).arg(elapsedTime));
}

/* Rollback */

std::vector<RollbackItem>
Migrator::getMigrationsForRollback(const MigrateOptions options) const
{
    auto migrationsDb = getMigrationsForRollbackByOptions(options);

    return m_migrations.get()
            | ranges::views::reverse
            | ranges::views::filter([this, &migrationsDb](const auto &migration)
    {
        return ranges::contains(migrationsDb, cachedMigrationName(*migration),
                                [](const auto &m) { return m.migration; });
    })
            | ranges::views::transform([this, &migrationsDb](const auto &migration)
                                       -> RollbackItem
    {
        // Can not happen that it doesn't find, checked in previous lambda by 'contains'
        auto &&[id, migrationName, _] =
                *std::ranges::find(migrationsDb, cachedMigrationName(*migration),
                                   [](const auto &m) { return m.migration; });

        return {std::move(id), std::move(migrationName), migration};
    })
            | ranges::to<std::vector<RollbackItem>>();
}

std::vector<MigrationItem>
Migrator::getMigrationsForRollbackByOptions(const MigrateOptions options) const
{
    if (options.stepValue > 0)
        return m_repository->getMigrations(options.stepValue);

    if (options.batch > 0)
        return m_repository->getMigrationsByBatch(options.batch);

    return m_repository->getLast();
}

std::vector<RollbackItem>
Migrator::getMigrationsForRollback(std::vector<MigrationItem> &&ran) const
{
    return ranges::views::move(ran)
            | ranges::views::transform([this](auto &&migrationItem) -> RollbackItem
    {
        auto &&[id, migrationName, _] = migrationItem;

        auto migration = m_migrationInstancesMap.at(migrationName);

        return {std::move(id), std::move(migrationName), std::move(migration)};
    })
            | ranges::to<std::vector<RollbackItem>>();
}

std::vector<RollbackItem>
Migrator::rollbackMigrations(std::vector<RollbackItem> &&migrations,
                             const bool pretend) const
{
    if (migrations.empty()) {
        info(QStringLiteral("Nothing to rollback"));

        return std::move(migrations);
    }

    for (const auto &migration : migrations)
        runDown(migration, pretend);

    return std::move(migrations);
}

void Migrator::runDown(const RollbackItem &migrationToRollback, const bool pretend) const
{
    const auto &[id, migrationName, migration] = migrationToRollback;

    if (pretend)
        return pretendToRun(*migration, MigrateMethod::Down); // clazy:exclude=returning-void-expression

    comment(QStringLiteral("Rolling back: "), false).note(migrationName);

    QElapsedTimer timer;
    timer.start();

    runMigration(*migration, MigrateMethod::Down);

    const auto elapsedTime = timer.elapsed();

    /* Once we have successfully run the migration "down" we will remove it from
       the migration repository so it will be considered to have not been run
       by the application then will be able to fire by any later operation. */
    m_repository->deleteMigration(id);

    info(QStringLiteral("Rolled back:"), false);
    note(QStringLiteral("  %1 (%2ms)").arg(migrationName).arg(elapsedTime));
}

/* Pretend */

void Migrator::pretendToRun(const Migration &migration, const MigrateMethod method) const
{
    for (auto &&query : getQueries(migration, method)) {
        info(QStringLiteral("%1: ").arg(cachedMigrationName(migration)), false);

        note(QueryUtils::parseExecutedQueryForPretend(query.query,
                                                      query.boundValues));
    }
}

QVector<Migrator::Log>
Migrator::getQueries(const Migration &migration, const MigrateMethod method) const
{
    const auto &migrationTypeId = typeid (migration);

    Q_ASSERT(m_migrationsProperties.get().contains(migrationTypeId));

    /* Now that we have the connections we can resolve it and pretend to run the
       queries against the database returning the array of raw SQL statements
       that would get fired against the database system for this migration. */
    return resolveConnection(m_migrationsProperties.get().at(migrationTypeId).connection)
            .pretend([&migration, method]()
    {
        migrateByMethod(migration, method);
    });
}

/* Migrate up/down common */

void Migrator::runMigration(const Migration &migration, const MigrateMethod method) const
{
    const auto &migrationTypeId = typeid (migration);

    Q_ASSERT(m_migrationsProperties.get().contains(migrationTypeId));

    const auto &migartionProperties = m_migrationsProperties.get().at(migrationTypeId);

    auto &connection = resolveConnection(migartionProperties.connection);

    // Invoke migration in the transaction if a database driver supports it
    const auto withinTransaction =
            connection.getSchemaGrammar().supportsSchemaTransactions() &&
            migartionProperties.withinTransaction;

    // Without transaction
    if (!withinTransaction)
        return migrateByMethod(migration, method); // clazy:exclude=returning-void-expression

    // Transactional migration
    connection.beginTransaction();

    try {
        migrateByMethod(migration, method);

    }  catch (...) {

        connection.rollBack();
        // Re-throw
        throw;
    }

    connection.commit();
}

void Migrator::migrateByMethod(const Migration &migration, const MigrateMethod method)
{
    switch (method) {
    case MigrateMethod::Up:
        return migration.up(); // clazy:exclude=returning-void-expression

    case MigrateMethod::Down:
        return migration.down(); // clazy:exclude=returning-void-expression
    }

    Q_UNREACHABLE();
}

/* Validate migrations */

void Migrator::throwIfMigrationsNotSorted(const QString &previousMigrationName,
                                          const QString &migrationName)
{
    if (previousMigrationName < migrationName)
        return;

    throw Exceptions::InvalidTemplateArgumentError(
                QStringLiteral(
                    "The template arguments passed to the TomApplication::migrations() "
                    "must always be sorted alphabetically (%1 < %2).")
                .arg(previousMigrationName, migrationName));
}

void Migrator::throwIfContainMigration(const QString &migrationName) const
{
    if (!m_migrationNames.contains(migrationName))
        return;

    throw Exceptions::InvalidTemplateArgumentError(
                QStringLiteral(
                    "The '%1' migration has already been added, the template arguments "
                    "passed to the TomApplication::migrations() can't contain the same "
                    "migration more times.")
                .arg(migrationName));
}

void Migrator::throwIfMigrationFileNameNotValid(const QString &migrationName)
{
    if (migrationName.isEmpty() || TomUtils::startsWithDatetimePrefix(migrationName))
        return;

    throw Exceptions::RuntimeError(
                QStringLiteral(
                    "Migration filename '%1' has to start with the datetime prefix.")
                .arg(migrationName));

}

void Migrator::throwIfMigrationClassNameNotValid(const QString &migrationNameOriginal,
                                                 const QString &migrationName)
{
    Q_ASSERT(!migrationName.isEmpty());

    if (TomUtils::startsWithDatetimePrefix(migrationName))
        return;

    throw Exceptions::RuntimeError(
                QStringLiteral(
                    "Migration class name '%1' has to start with the datetime prefix, "
                    "eg. _2014_10_12_000000_create_xyz_table, another accepted format "
                    "is StudlyCase eg. CreateXyzTable with the T_MIGRATION macro (it "
                    "extracts this datetime prefix from a file name).")
                .arg(migrationNameOriginal));
}

} // namespace Tom

TINYORM_END_COMMON_NAMESPACE
