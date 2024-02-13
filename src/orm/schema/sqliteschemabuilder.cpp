#include "orm/schema/sqliteschemabuilder.hpp"

#include <filesystem>
#include <fstream>

#include "orm/databaseconnection.hpp"
#include "orm/schema/grammars/sqliteschemagrammar.hpp"
#include "orm/utils/type.hpp"

#ifdef TINYORM_USING_TINYDRIVERS
#  include "orm/exceptions/runtimeerror.hpp"
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

namespace fs = std::filesystem;

using fspath = std::filesystem::path;

namespace Orm::SchemaNs
{

/* public */

std::optional<SqlQuery> SQLiteSchemaBuilder::createDatabase(const QString &name) const
{
    const fspath databaseName(name.toUtf8().constData());

    if (fs::exists(databaseName))
        throw Exceptions::RuntimeError(
                    QStringLiteral("SQLite database '%1' already exists in %2().")
                    .arg(QString::fromStdString(databaseName.string()), __tiny_func__));

    const std::ofstream fs(databaseName,
                           std::ios::out | std::ios::binary | std::ios::trunc);

    if (!fs.fail())
        return std::nullopt;

    throw Exceptions::RuntimeError(
                QStringLiteral("Creation of SQLite database '%1' failed in %2().")
                .arg(QString::fromStdString(databaseName.string()), __tiny_func__));
}

std::optional<SqlQuery>
SQLiteSchemaBuilder::dropDatabaseIfExists(const QString &name) const
{
    const fspath databasePath(name.toUtf8().constData());

    if (fs::exists(databasePath))
        fs::remove(databasePath);

    return std::nullopt;
}

void SQLiteSchemaBuilder::dropAllTables() const
{
    if (m_connection->getDatabaseName() != in_memory)
        return refreshDatabaseFile(); // NOLINT(readability-avoid-return-with-void-value) clazy:exclude=returning-void-expression

    using Grammars::SQLiteSchemaGrammar;

    m_connection->selectFromWriteConnection(SQLiteSchemaGrammar::
                                            compileEnableWriteableSchema());

    m_connection->selectFromWriteConnection(m_grammar->compileDropAllTables({}));

    m_connection->selectFromWriteConnection(SQLiteSchemaGrammar::
                                            compileDisableWriteableSchema());

    m_connection->selectFromWriteConnection(SQLiteSchemaGrammar::compileRebuild());
}

void SQLiteSchemaBuilder::dropAllViews() const
{
    using Grammars::SQLiteSchemaGrammar;

    m_connection->selectFromWriteConnection(SQLiteSchemaGrammar::
                                            compileEnableWriteableSchema());

    m_connection->selectFromWriteConnection(m_grammar->compileDropAllViews({}));

    m_connection->selectFromWriteConnection(SQLiteSchemaGrammar::
                                            compileDisableWriteableSchema());

    m_connection->selectFromWriteConnection(SQLiteSchemaGrammar::compileRebuild());
}

SqlQuery SQLiteSchemaBuilder::getAllTables() const
{
    // TODO schema, use postprocessor processColumnListing() silverqx
    return m_connection->selectFromWriteConnection(m_grammar->compileGetAllTables());
}

SqlQuery SQLiteSchemaBuilder::getAllViews() const
{
    return m_connection->selectFromWriteConnection(m_grammar->compileGetAllViews());
}

void SQLiteSchemaBuilder::refreshDatabaseFile() const
{
    const auto &databaseName = m_connection->getDatabaseName();

    const std::ofstream fs(databaseName.toUtf8().constData(),
                           std::ios::out | std::ios::binary | std::ios::trunc);

    if (!fs.fail())
        return;

    throw Exceptions::RuntimeError(
                QStringLiteral("Truncation of SQLite database '%1' failed in %2().")
                .arg(databaseName, __tiny_func__));
}

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE
