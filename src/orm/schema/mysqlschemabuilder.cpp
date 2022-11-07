#include "orm/schema/mysqlschemabuilder.hpp"

#include <QtSql/QSqlDriver>
#include <QtSql/QSqlRecord>

#include "orm/databaseconnection.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs
{

/* public */

std::optional<SqlQuery> MySqlSchemaBuilder::createDatabase(const QString &name) const
{
    return m_connection.statement(
                m_grammar.compileCreateDatabase(name, m_connection));
}

std::optional<SqlQuery>
MySqlSchemaBuilder::dropDatabaseIfExists(const QString &name) const
{
    return m_connection.statement(
                m_grammar.compileDropDatabaseIfExists(name));
}

// TEST schema, test in functional tests silverqx
void MySqlSchemaBuilder::dropAllTables() const
{
    auto query = getAllTables();

    // No fields in the record
    if (query.record().isEmpty())
        return;

    QVector<QString> tables;
    if (const auto size = query.size(); size > 0)
        tables.reserve(size);

    while (query.next())
        tables << query.value(0).value<QString>();

    if (tables.isEmpty())
        return;

    disableForeignKeyConstraints();

    m_connection.statement(m_grammar.compileDropAllTables(tables));

    enableForeignKeyConstraints();
}

// TEST schema, test in functional tests silverqx
void MySqlSchemaBuilder::dropAllViews() const
{
    auto query = getAllViews();

    // No fields in the record
    if (query.record().isEmpty())
        return;

    QVector<QString> views;
    if (const auto size = query.size(); size > 0)
        views.reserve(size);

    while (query.next())
        views << query.value(0).value<QString>();

    if (views.isEmpty())
        return;

    m_connection.statement(m_grammar.compileDropAllViews(views));
}

SqlQuery MySqlSchemaBuilder::getAllTables() const
{
    // TODO schema, use postprocessor processColumnListing() silverqx
    return m_connection.selectFromWriteConnection(m_grammar.compileGetAllTables());
}

SqlQuery MySqlSchemaBuilder::getAllViews() const
{
    return m_connection.selectFromWriteConnection(m_grammar.compileGetAllViews());
}

QStringList MySqlSchemaBuilder::getColumnListing(const QString &table) const
{
    const auto table_ = NOSPACE.arg(m_connection.getTablePrefix(), table);

    auto query = m_connection.selectFromWriteConnection(
                     m_grammar.compileColumnListing(),
                     {m_connection.getDatabaseName(), table_});

    return m_connection.getPostProcessor().processColumnListing(query);
}

bool MySqlSchemaBuilder::hasTable(const QString &table) const
{
    const auto table_ = NOSPACE.arg(m_connection.getTablePrefix(), table);

    Q_ASSERT(m_connection.driver()->hasFeature(QSqlDriver::QuerySize));

    return m_connection.selectFromWriteConnection(
                m_grammar.compileTableExists(),
                {m_connection.getDatabaseName(), table_}).size() > 0;
}

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE
