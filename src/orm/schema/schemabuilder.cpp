#include "orm/schema/schemabuilder.hpp"

#include <range/v3/action/transform.hpp>
#include <range/v3/algorithm/contains.hpp>

#include "orm/databaseconnection.hpp"
#include "orm/exceptions/logicerror.hpp"
#include "orm/utils/helpers.hpp"
#include "orm/utils/query.hpp"

#ifdef TINYORM_USING_TINYDRIVERS
#  include "orm/exceptions/runtimeerror.hpp"
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Utils::Helpers;

using QueryUtils = Orm::Utils::Query;

namespace Orm::SchemaNs
{

SchemaBuilder::SchemaBuilder(std::shared_ptr<DatabaseConnection> connection)
    : m_connection(std::move(connection))
    , m_grammar(m_connection->getSchemaGrammarShared())
{}

std::optional<SqlQuery> SchemaBuilder::createDatabase(const QString &/*unused*/) const
{
    throw Exceptions::LogicError(
                QStringLiteral("%1 database driver does not support creating databases.")
                .arg(m_connection->driverName()));
}

std::optional<SqlQuery>
SchemaBuilder::dropDatabaseIfExists(const QString &/*unused*/) const
{
    throw Exceptions::LogicError(
                QStringLiteral("%1 database driver does not support dropping databases.")
                .arg(m_connection->driverName()));
}

void SchemaBuilder::create(const QString &table,
                           const std::function<void(Blueprint &)> &callback) const
{
    build(Helpers::tap<Blueprint>(createBlueprint(table),
                                  [&callback](auto &blueprint)
    {
        blueprint.create();

        std::invoke(callback, blueprint);
    }));
}

void SchemaBuilder::table(const QString &table,
                          const std::function<void(Blueprint &)> &callback) const
{
    build(createBlueprint(table, callback));
}

void SchemaBuilder::drop(const QString &table) const
{
    build(Helpers::tap<Blueprint>(createBlueprint(table), [](auto &blueprint)
    {
        blueprint.drop();
    }));
}

void SchemaBuilder::dropIfExists(const QString &table) const
{
    build(Helpers::tap<Blueprint>(createBlueprint(table), [](auto &blueprint)
    {
        blueprint.dropIfExists();
    }));
}

void SchemaBuilder::rename(const QString &from, const QString &to) const
{
    build(Helpers::tap<Blueprint>(createBlueprint(from), [&to](auto &blueprint)
    {
        blueprint.rename(to);
    }));
}

void SchemaBuilder::dropColumns(const QString &table,
                                const QList<QString> &columns) const
{
    auto blueprint = createBlueprint(table);

    blueprint.dropColumns(columns);

    build(std::move(blueprint));
}

void SchemaBuilder::dropColumn(const QString &table, const QString &column) const
{
    auto blueprint = createBlueprint(table);

    blueprint.dropColumns({column});

    build(std::move(blueprint));
}

void SchemaBuilder::renameColumn(const QString &table, const QString &from,
                                 const QString &to)
{
    auto blueprint = createBlueprint(table);

    blueprint.renameColumn(from, to);

    build(std::move(blueprint));
}

void SchemaBuilder::dropAllTables() const
{
    // CUR schema, solve this logic vs runtime exception silverqx
    throw Exceptions::LogicError(
                QStringLiteral("%1 database driver does not support dropping all tables.")
                .arg(m_connection->driverName()));
}

void SchemaBuilder::dropAllViews() const
{
    throw Exceptions::LogicError(
                QStringLiteral("%1 database driver does not support dropping all views.")
                .arg(m_connection->driverName()));
}

void SchemaBuilder::dropAllTypes() const
{
    throw Exceptions::LogicError(
                QStringLiteral("%1 database driver does not support dropping all types.")
                .arg(m_connection->driverName()));
}

SqlQuery SchemaBuilder::getAllTables() const
{
    throw Exceptions::LogicError(
                QStringLiteral("%1 database driver does not support getting all tables.")
                .arg(m_connection->driverName()));
}

SqlQuery SchemaBuilder::getAllViews() const
{
    throw Exceptions::RuntimeError(NotImplemented);
}

SqlQuery SchemaBuilder::enableForeignKeyConstraints() const
{
    return m_connection->statement(m_grammar->compileEnableForeignKeyConstraints());
}

SqlQuery SchemaBuilder::disableForeignKeyConstraints() const
{
    return m_connection->statement(m_grammar->compileDisableForeignKeyConstraints());
}

void
SchemaBuilder::withoutForeignKeyConstraints(const std::function<void()> &callback) const
{
    disableForeignKeyConstraints();

    try {
        std::invoke(callback);

    } catch (...) {

        enableForeignKeyConstraints();
        // Re-throw
        throw;
    }
}

QStringList SchemaBuilder::getColumnListing(const QString &table) const
{
    auto query = m_connection->selectFromWriteConnection(
                     m_grammar->compileColumnListing(
                         NOSPACE.arg(m_connection->getTablePrefix(), table)));

    return m_connection->getPostProcessor().processColumnListing(query);
}

bool SchemaBuilder::hasTable(const QString &table) const
{
    const auto table_ = NOSPACE.arg(m_connection->getTablePrefix(), table);

    auto query = m_connection->selectFromWriteConnection(
                     m_grammar->compileTableExists(), {table_});

    return QueryUtils::queryResultSize(query) > 0;
}

// TEST schema, test in functional tests silverqx
bool SchemaBuilder::hasColumn(const QString &table, const QString &column) const
{
    return ranges::contains(getColumnListing(table), column.toLower(),
                            [](auto &&columnFromListing)
    {
        return columnFromListing.toLower();
    });
}

bool SchemaBuilder::hasColumns(const QString &table,
                               const QList<QString> &columns) const
{
    auto columnsFromListing = getColumnListing(table);

    columnsFromListing |= ranges::actions::transform([](const auto &column)
    {
        return column.toLower();
    });

    return std::ranges::all_of(columns, [&columnsFromListing](const auto &column)
    {
        return columnsFromListing.contains(column.toLower());
    });
}

Blueprint SchemaBuilder::createBlueprint(
            const QString &table, const std::function<void(Blueprint &)> &callback) const
{
    auto prefix = m_connection->getConfig(prefix_indexes).value<bool>()
                  ? m_connection->getConfig(prefix_).value<QString>()
                  : QString("");

    return Blueprint(table, callback, std::move(prefix));
}

void SchemaBuilder::build(Blueprint &&blueprint) const // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
{
    blueprint.build(*m_connection, *m_grammar);
}

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE
