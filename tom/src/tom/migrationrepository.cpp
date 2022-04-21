#include "tom/migrationrepository.hpp"

#include <orm/databaseconnection.hpp>
#include <orm/query/querybuilder.hpp>

using Orm::Constants::ASC;
using Orm::Constants::DESC;
using Orm::Constants::GE;

using Orm::DatabaseConnection;
using Orm::SchemaNs::Blueprint;

using QueryBuilder = Orm::Query::Builder;

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
            ->orderBy("batch", ASC)
            .orderBy("migration", ASC)
            .pluck("migration");
}

std::vector<MigrationItem> MigrationRepository::getRan(const QString &order) const
{
    // Ownership of the QSharedPointer<QueryBuilder>
    auto query = table()
                 ->orderBy("batch", order)
                 .orderBy("migration", order)
                 .get();

    return hydrateMigrations(query);
}

std::vector<MigrationItem> MigrationRepository::getMigrations(const int steps) const
{
    // Ownership of the QSharedPointer<QueryBuilder>
    auto query = table()->where("batch", GE, 1)
                 .orderBy("batch", DESC)
                 .orderBy("migration", DESC)
                 .take(steps)
                 .get();

    return hydrateMigrations(query);
}

std::vector<MigrationItem> MigrationRepository::getLast() const
{
    // Ownership of the QSharedPointer<QueryBuilder>
    auto query = table()->whereEq("batch", getLastBatchNumber())
                 .orderBy("migration", DESC)
                 .get();

    return hydrateMigrations(query);
}

std::map<QString, QVariant> MigrationRepository::getMigrationBatches() const
{
    // Ownership of the QSharedPointer<QueryBuilder>
    return table()
            ->orderBy("batch", ASC)
            .orderBy("migration", ASC)
            .pluck<QString>("batch", "migration");
}

void MigrationRepository::log(const QString &file, const int batch) const
{
    // Ownership of the QSharedPointer<QueryBuilder>
    table()->insert({{"migration", file}, {"batch", batch}});
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
    return table()->max("batch").value<int>();
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

        table.string("migration").unique();
        table.integer("batch");
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
                    MigrationItem {query.value("id").value<quint64>(),
                                   query.value("migration").value<QString>(),
                                   query.value("batch").value<int>()});
#else
        migration.emplace_back(query.value("id").value<quint64>(),
                               query.value("migration").value<QString>(),
                               query.value("batch").value<int>());
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
