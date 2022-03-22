#include "orm/schema.hpp"

#include "orm/macros/likely.hpp"
#include "orm/schema/schemabuilder.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

std::unordered_map<Connectors::ConnectionName,
                   std::unique_ptr<SchemaBuilder>> Schema::m_schemaBuildersCache;
std::shared_ptr<DatabaseManager> Schema::m_manager;

/* public */

/* Proxy methods to the SchemaBuilder */

QSqlQuery Schema::createDatabase(const QString &name, const QString &connection)
{
    return schemaBuilder(connection).createDatabase(name);
}

QSqlQuery Schema::dropDatabaseIfExists(const QString &name, const QString &connection)
{
    return schemaBuilder(connection).dropDatabaseIfExists(name);
}

void Schema::create(
        const QString &table, const std::function<void(Blueprint &)> &callback,
        const QString &connection)
{
    schemaBuilder(connection).create(table, callback);
}

void Schema::table(
        const QString &table, const std::function<void(Blueprint &)> &callback,
        const QString &connection)
{
    schemaBuilder(connection).table(table, callback);
}

void Schema::drop(const QString &table, const QString &connection)
{
    schemaBuilder(connection).drop(table);
}

void Schema::dropIfExists(const QString &table, const QString &connection)
{
    schemaBuilder(connection).dropIfExists(table);
}

void Schema::rename(const QString &from, const QString &to, const QString &connection)
{
    schemaBuilder(connection).rename(from, to);
}

void Schema::dropColumns(const QString &table, const QVector<QString> &columns,
                         const QString &connection)
{
    schemaBuilder(connection).dropColumns(table, columns);
}

void Schema::dropColumn(const QString &table, const QString &column,
                        const QString &connection)
{
    schemaBuilder(connection).dropColumns(table, {column});
}

void Schema::renameColumn(const QString &table, const QString &from,
                          const QString &to, const QString &connection)
{
    schemaBuilder(connection).renameColumn(table, from, to);
}

void Schema::dropAllTables(const QString &connection)
{
    schemaBuilder(connection).dropAllTables();
}

void Schema::dropAllViews(const QString &connection)
{
    schemaBuilder(connection).dropAllViews();
}

void Schema::dropAllTypes(const QString &connection)
{
    schemaBuilder(connection).dropAllTypes();
}

QSqlQuery Schema::getAllTables(const QString &connection)
{
    return schemaBuilder(connection).getAllTables();
}

QSqlQuery Schema::getAllViews(const QString &connection)
{
    return schemaBuilder(connection).getAllViews();
}

QSqlQuery Schema::enableForeignKeyConstraints(const QString &connection)
{
    return schemaBuilder(connection).enableForeignKeyConstraints();
}

QSqlQuery Schema::disableForeignKeyConstraints(const QString &connection)
{
    return schemaBuilder(connection).disableForeignKeyConstraints();
}

QStringList Schema::getColumnListing(const QString &table, const QString &connection)
{
    return schemaBuilder(connection).getColumnListing(table);
}

bool Schema::hasTable(const QString &table, const QString &connection)
{
    return schemaBuilder(connection).hasTable(table);
}

bool Schema::hasColumn(const QString &table, const QString &column,
                       const QString &connection)
{
    return schemaBuilder(connection).hasColumn(table, column);
}

bool Schema::hasColumns(const QString &table, const QVector<QString> &columns,
                        const QString &connection)
{
    return schemaBuilder(connection).hasColumns(table, columns);
}

/* Schema */

SchemaBuilder &Schema::connection(const QString &name)
{
    return schemaBuilder(name);
}

SchemaBuilder &Schema::on(const QString &name)
{
    return schemaBuilder(name);
}

/* Others */

void Schema::defaultStringLength(const int length)
{
    Blueprint::defaultStringLength(length);
}

/* private */

SchemaBuilder &Schema::schemaBuilder(const QString &connection)
{
    /* Cache obtained schema builders, they should always be the same for the given
       connection name, cached value is removed from the cache in
       DatabaseManager::removeConnection() */
    if (m_schemaBuildersCache.contains(connection))
        return *m_schemaBuildersCache[connection];

    // Ownership of a unique_ptr()
    return *(m_schemaBuildersCache[connection] = manager().connection(connection)
                                                 .getSchemaBuilder());
}

DatabaseManager &Schema::manager()
{
    if (m_manager) T_LIKELY
        return *m_manager;

    else T_UNLIKELY
        return *(m_manager = DatabaseManager::instance());
}

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE
