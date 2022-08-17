#include "orm/schema/postgresschemabuilder.hpp"

#include <QSet>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlRecord>

#include "orm/databaseconnection.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs
{

/* public */

std::optional<QSqlQuery> PostgresSchemaBuilder::createDatabase(const QString &name) const
{
    // DUP schema silverqx
    return m_connection.unprepared(
                m_grammar.compileCreateDatabase(name, m_connection));
}

std::optional<QSqlQuery>
PostgresSchemaBuilder::dropDatabaseIfExists(const QString &name) const
{
    // DUP schema silverqx
    return m_connection.unprepared(
                m_grammar.compileDropDatabaseIfExists(name));
}

// TEST schema, test in functional tests silverqx
void PostgresSchemaBuilder::dropAllTables() const
{
    auto query = getAllTables();

    // No fields in the record
    if (query.record().isEmpty())
        return;

    /* ConnectionFactory provides a default value 'spatial_ref_sys' for this option
       for the QPSQL driver. */
    const auto excludedTables = m_connection.getConfig(dont_drop).value<QStringList>();

    QVector<QString> tables;
    if (const auto size = query.size(); size > 0)
        tables.reserve(size);

    while (query.next())
        if (auto table = query.value(0).value<QString>();
            !excludedTables.contains(table)
        )
            tables << std::move(table);

    if (tables.isEmpty())
        return;

    m_connection.unprepared(m_grammar.compileDropAllTables(tables));
}

// TEST schema, test in functional tests silverqx
void PostgresSchemaBuilder::dropAllViews() const
{
    auto query = getAllViews();

    // No fields in the record
    if (query.record().isEmpty())
        return;

    /* For these it throws that needed by the postgis extension and proposes to delete
       extension instead, so exclude them. */
    const QSet<QString> excludedViews {QStringLiteral("geography_columns"),
                                       QStringLiteral("geometry_columns")};

    QVector<QString> views;
    if (const auto size = query.size(); size > 0)
        views.reserve(size);

    while (query.next())
        if (auto view = query.value(0).value<QString>();
            !excludedViews.contains(view)
        )
            views << std::move(view);

    if (views.isEmpty())
        return;

    m_connection.unprepared(m_grammar.compileDropAllViews(views));
}

QSqlQuery PostgresSchemaBuilder::getAllTables() const
{
    auto schemaList = m_connection.getConfig(schema_).value<QStringList>();

    QVector<QString> schema;
    std::ranges::move(schemaList, std::back_inserter(schema));

    // TODO schema, use postprocessor processColumnListing() silverqx
    return m_connection.selectFromWriteConnection(
                m_grammar.compileGetAllTables(std::move(schema)));
}

QSqlQuery PostgresSchemaBuilder::getAllViews() const
{
    auto schemaList = m_connection.getConfig(schema_).value<QStringList>();

    QVector<QString> schema;
    std::ranges::move(schemaList, std::back_inserter(schema));

    return m_connection.selectFromWriteConnection(
                m_grammar.compileGetAllViews(std::move(schema)));
}

QStringList PostgresSchemaBuilder::getColumnListing(const QString &table) const
{
    auto [schema, table_] = parseSchemaAndTable(table);

    table_ = NOSPACE.arg(m_connection.getTablePrefix(), table);

    auto query = m_connection.selectFromWriteConnection(
            m_grammar.compileColumnListing(), {schema, table_});

    return m_connection.getPostProcessor().processColumnListing(query);
}

bool PostgresSchemaBuilder::hasTable(const QString &table) const
{
    auto [schema, table_] = parseSchemaAndTable(table);

    table_ = NOSPACE.arg(m_connection.getTablePrefix(), table);

    Q_ASSERT(m_connection.driver()->hasFeature(QSqlDriver::QuerySize));

    return m_connection.selectFromWriteConnection(
                m_grammar.compileTableExists(), {schema, table_}).size() > 0;
}

/* protected */

std::tuple<QString, QString>
PostgresSchemaBuilder::parseSchemaAndTable(const QString &table) const
{
    QString schema;

    if (m_connection.getConfig().contains(schema_)) {
        auto table_ = table.split(DOT);
        auto schemaConfig = m_connection.getConfig(schema_).value<QStringList>();

        // table was specified with the schema, like schema.table, so use this schema
        if (schemaConfig.contains(table_.constFirst()))
            return {table_.takeFirst(), table_.join(DOT)};

        // Instead, get a schema from the configuration
        if (!schemaConfig.isEmpty())
            schema = std::move(schemaConfig[0]);
    }

    // Default schema
    if (schema.isEmpty())
        schema = PUBLIC;

    return {std::move(schema), table};
}

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE
