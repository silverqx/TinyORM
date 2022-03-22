#include "orm/schema/schemabuilder.hpp"

#include <range/v3/action/transform.hpp>
#include <range/v3/algorithm/contains.hpp>

#include "orm/databaseconnection.hpp"
#include "orm/exceptions/logicerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs
{

SchemaBuilder::SchemaBuilder(DatabaseConnection &connection)
    : m_connection(connection)
    , m_grammar(connection.getSchemaGrammar())
{}

QSqlQuery SchemaBuilder::createDatabase(const QString &/*unused*/) const
{
    throw Exceptions::LogicError(
                QStringLiteral("%1 database driver does not support creating databases.")
                .arg(m_connection.driverName()));
}

QSqlQuery SchemaBuilder::dropDatabaseIfExists(const QString &/*unused*/) const
{
    throw Exceptions::LogicError(
                "This database driver does not support dropping databases.");
}

void SchemaBuilder::create(const QString &table,
                           const std::function<void(Blueprint &)> &callback) const
{
    auto blueprint = createBlueprint(table);

    blueprint.create();

    std::invoke(callback, blueprint);

    build(std::move(blueprint));
}

void SchemaBuilder::table(const QString &table,
                          const std::function<void(Blueprint &)> &callback) const
{
    build(createBlueprint(table, callback));
}

void SchemaBuilder::drop(const QString &table) const
{
    // CUR schema, create tap helper silverqx
    auto blueprint = createBlueprint(table);

    blueprint.drop();

    build(std::move(blueprint));
}

void SchemaBuilder::dropIfExists(const QString &table) const
{
    auto blueprint = createBlueprint(table);

    blueprint.dropIfExists();

    build(std::move(blueprint));
}

void SchemaBuilder::rename(const QString &from, const QString &to) const
{
    auto blueprint = createBlueprint(from);

    blueprint.rename(to);

    build(std::move(blueprint));
}

void SchemaBuilder::dropColumns(const QString &table,
                                const QVector<QString> &columns) const
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
                .arg(m_connection.driverName()));
}

void SchemaBuilder::dropAllViews() const
{
    throw Exceptions::LogicError(
                QStringLiteral("%1 database driver does not support dropping all views.")
                .arg(m_connection.driverName()));
}

void SchemaBuilder::dropAllTypes() const
{
    throw Exceptions::LogicError(
                QStringLiteral("%1 database driver does not support dropping all types.")
                .arg(m_connection.driverName()));
}

QSqlQuery SchemaBuilder::getAllTables() const
{
    throw Exceptions::LogicError(
                QStringLiteral("%1 database driver does not support getting all tables.")
                .arg(m_connection.driverName()));
}

QSqlQuery SchemaBuilder::getAllViews() const
{
    throw Exceptions::RuntimeError(NotImplemented);
}

QSqlQuery SchemaBuilder::enableForeignKeyConstraints() const
{
    return m_connection.statement(m_grammar.compileEnableForeignKeyConstraints());
}

QSqlQuery SchemaBuilder::disableForeignKeyConstraints() const
{
    return m_connection.statement(m_grammar.compileDisableForeignKeyConstraints());
}

QStringList SchemaBuilder::getColumnListing(const QString &table) const
{
    auto query = m_connection.selectFromWriteConnection(
                     m_connection.getTablePrefix() +
                     m_grammar.compileColumnListing(table));

    return m_connection.getPostProcessor().processColumnListing(query);
}

bool SchemaBuilder::hasTable(const QString &/*unused*/) const
{
    throw Exceptions::RuntimeError(NotImplemented);
}

// CUR schema, test in functional tests silverqx
bool SchemaBuilder::hasColumn(const QString &table, const QString &column) const
{
    return ranges::contains(getColumnListing(table), column.toLower(),
                            [](auto &&columnFromListing)
    {
        return columnFromListing.toLower();
    });
}

bool SchemaBuilder::hasColumns(const QString &table,
                               const QVector<QString> &columns) const
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
    auto prefix = m_connection.getConfig(prefix_indexes).value<bool>()
                  ? m_connection.getConfig(prefix_).value<QString>()
                  : "";

    return Blueprint(table, callback, std::move(prefix));
}

void SchemaBuilder::build(Blueprint &&blueprint) const
{
    blueprint.build(m_connection, m_grammar);
}

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE
