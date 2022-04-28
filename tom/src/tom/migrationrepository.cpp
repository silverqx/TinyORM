#include "tom/migrationrepository.hpp"

#include <orm/databaseconnection.hpp>
#include <orm/query/querybuilder.hpp>

#include "tom/tomconstants.hpp"

using Orm::Constants::ASC;
using Orm::Constants::DESC;
using Orm::Constants::GE;
using Orm::Constants::ID;

using Orm::DatabaseConnection;
using Orm::SchemaNs::Blueprint;

using QueryBuilder = Orm::Query::Builder;

using Tom::Constants::batch_;
using Tom::Constants::migration_;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
{

/* public */

MigrationRepository::MigrationRepository(
        std::shared_ptr<ConnectionResolverInterface> &&resolver, QString table
)
    : m_resolver(std::move(resolver))
    , m_table(std::move(table))
{}

QVector<QVariant> MigrationRepository::getRanSimple() const
{
    // Ownership of the QSharedPointer<QueryBuilder>
    return table()
            ->orderBy(batch_, ASC)
            .orderBy(migration_, ASC)
            .pluck(migration_);
}

std::vector<MigrationItem> MigrationRepository::getRan(const QString &order) const
{
    // Ownership of the QSharedPointer<QueryBuilder>
    auto query = table()
                 ->orderBy(batch_, order)
                 .orderBy(migration_, order)
                 .get();

    return hydrateMigrations(query);
}

std::vector<MigrationItem> MigrationRepository::getMigrations(const int steps) const
{
    // Ownership of the QSharedPointer<QueryBuilder>
    auto query = table()->where(batch_, GE, 1)
                 .orderBy(batch_, DESC)
                 .orderBy(migration_, DESC)
                 .take(steps)
                 .get();

    return hydrateMigrations(query);
}

std::vector<MigrationItem> MigrationRepository::getLast() const
{
    // Ownership of the QSharedPointer<QueryBuilder>
    auto query = table()->whereEq(batch_, getLastBatchNumber())
                 .orderBy(migration_, DESC)
                 .get();

    return hydrateMigrations(query);
}

std::map<QString, QVariant> MigrationRepository::getMigrationBatches() const
{
    // Ownership of the QSharedPointer<QueryBuilder>
    return table()
            ->orderBy(batch_, ASC)
            .orderBy(migration_, ASC)
            .pluck<QString>(batch_, migration_);
}

void MigrationRepository::log(const QString &file, const int batch) const
{
    // Ownership of the QSharedPointer<QueryBuilder>
    table()->insert({{migration_, file}, {batch_, batch}});
}

void MigrationRepository::deleteMigration(const quint64 id) const
{
    // Ownership of the QSharedPointer<QueryBuilder>
    table()->deleteRow(id);
}

int MigrationRepository::getNextBatchNumber() const
{
    return getLastBatchNumber() + 1;
}

int MigrationRepository::getLastBatchNumber() const
{
    // Ownership of the QSharedPointer<QueryBuilder>
    // Will be 0 on empty migrations table
    return table()->max(batch_).value<int>();
}

void MigrationRepository::createRepository() const
{
    // Ownership of a unique_ptr()
    const auto schema = getConnection().getSchemaBuilder();

    /* The migrations table is responsible for keeping track of which migrations have
       actually run for the application. We'll create the table to hold the migration
      file paths as well as the batch ID. */
    schema->create(m_table, [](Blueprint &table)
    {
        table.id();

        table.string(migration_).unique();
        table.integer(batch_);
    });
}

bool MigrationRepository::repositoryExists() const
{
    // Ownership of a unique_ptr()
    const auto schema = getConnection().getSchemaBuilder();

    return schema->hasTable(m_table);
}

void MigrationRepository::deleteRepository() const
{
    getConnection().getSchemaBuilder()->drop(m_table);
}

DatabaseConnection &MigrationRepository::getConnection() const
{
    return m_resolver->connection(m_connection);
}

void MigrationRepository::setConnection(const QString &name,
                                        std::optional<bool> &&debugSql)
{
    m_connection = name;

    if (!debugSql)
        return;

    // Enable/disable showing of sql queries in the console
    setConnectionDebugSql(std::move(debugSql));
}

/* protected */

QSharedPointer<QueryBuilder> MigrationRepository::table() const
{
    return getConnection().table(m_table);
}

std::vector<MigrationItem>
MigrationRepository::hydrateMigrations(QSqlQuery &query) const
{
    std::vector<MigrationItem> migration;

    while (query.next())
#ifdef __clang__
        migration.emplace_back(
                    MigrationItem {query.value(ID).value<quint64>(),
                                   query.value(migration_).value<QString>(),
                                   query.value(batch_).value<int>()});
#else
        migration.emplace_back(query.value(ID).value<quint64>(),
                               query.value(migration_).value<QString>(),
                               query.value(batch_).value<int>());
#endif

    return migration;
}

void MigrationRepository::setConnectionDebugSql(std::optional<bool> &&debugSql) const
{
    auto &connection = getConnection();

    if (*debugSql)
        connection.enableDebugSql();
    else
        connection.disableDebugSql();
}

} // namespace Tom

TINYORM_END_COMMON_NAMESPACE
