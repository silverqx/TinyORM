#include "orm/schema/grammars/postgresschemagrammar.hpp"

#include <unordered_set>

#include "orm/databaseconnection.hpp"
#include "orm/macros/threadlocal.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs::Grammars
{

/* public */

/* Compile methods for the SchemaBuilder */

QString PostgresSchemaGrammar::compileCreateDatabase(
            const QString &name, DatabaseConnection &connection) const
{
    return QStringLiteral("create database %1 encoding %2")
            .arg(wrapValue(name),
                 wrapValue(connection.getConfig(charset_).value<QString>()));
}

QString PostgresSchemaGrammar::compileDropDatabaseIfExists(const QString &name) const
{
    // DUP schema silverqx
    return QStringLiteral("drop database if exists %1").arg(wrapValue(name));
}

QString PostgresSchemaGrammar::compileDropAllTables(const QVector<QString> &tables) const
{
    return QStringLiteral("drop table %1 cascade").arg(columnize(tables));
}

QString PostgresSchemaGrammar::compileDropAllViews(const QVector<QString> &views) const
{
    return QStringLiteral("drop view %1 cascade").arg(columnize(views));
}

QString
PostgresSchemaGrammar::compileGetAllTables(const QVector<QString> &databases) const
{
    return QStringLiteral("select tablename from pg_catalog.pg_tables "
                          "where schemaname in (%1)")
            .arg(quoteString(databases));
}

QString
PostgresSchemaGrammar::compileGetAllViews(const QVector<QString> &databases) const
{
    return QStringLiteral("select viewname from pg_catalog.pg_views "
                          "where schemaname in (%1)")
            .arg(quoteString(databases));
}

QString PostgresSchemaGrammar::compileEnableForeignKeyConstraints() const
{
    return QStringLiteral("SET CONSTRAINTS ALL IMMEDIATE;");
}

QString PostgresSchemaGrammar::compileDisableForeignKeyConstraints() const
{
    return QStringLiteral("SET CONSTRAINTS ALL DEFERRED;");
}

QString PostgresSchemaGrammar::compileTableExists() const
{
    return QStringLiteral("select * "
                          "from information_schema.tables "
                          "where table_schema = ? and table_name = ? and "
                          "table_type = 'BASE TABLE'");
}

QString PostgresSchemaGrammar::compileColumnListing(const QString &/*unused*/) const
{
    return QStringLiteral("select column_name "
                          "from information_schema.columns "
                          "where table_schema = ? and table_name = ?");
}

/* Compile methods for commands */

QVector<QString>
PostgresSchemaGrammar::compileCreate(const Blueprint &blueprint) const
{
    // Primary SQL query for create table
    auto sqlCreateTable = compileCreateTable(blueprint);

    // Add autoIncrement starting values to the SQL query if have been supplied
    auto autoIncrementStartingValues = compileAutoIncrementStartingValues(blueprint);

    /* Prepare container with all sql queries, autoIncrement for every column uses
       alter table, so separate SQL queries are provided for every column. */
    QVector<QString> sql;
    sql.reserve(2);

    sql << std::move(sqlCreateTable);

    if (!autoIncrementStartingValues.isEmpty())
        sql << std::move(autoIncrementStartingValues);

    return sql;
}

QVector<QString>
PostgresSchemaGrammar::compileDrop(const Blueprint &blueprint,
                                   const BasicCommand &/*unused*/) const
{
    // DUP schema silverqx
    return {QStringLiteral("drop table %1").arg(wrapTable(blueprint))};
}

QVector<QString>
PostgresSchemaGrammar::compileDropIfExists(const Blueprint &blueprint,
                                           const BasicCommand &/*unused*/) const
{
    // DUP schema silverqx
    return {QStringLiteral("drop table if exists %1").arg(wrapTable(blueprint))};
}

QVector<QString>
PostgresSchemaGrammar::compileRename(const Blueprint &blueprint,
                                     const RenameCommand &command) const
{
    return {QStringLiteral("alter table %1 rename to %2")
                .arg(wrapTable(blueprint), BaseGrammar::wrap(command.to))};
}

QVector<QString>
PostgresSchemaGrammar::compileAdd(const Blueprint &blueprint,
                                  const BasicCommand &/*unused*/) const
{
    auto columns = prefixArray("add column", getColumns(blueprint));

    // Add autoIncrement starting values to the SQL query if have been supplied
    auto autoIncrementStartingValues = compileAutoIncrementStartingValues(blueprint);

    auto sqlAlterTable = QStringLiteral("alter table %1 %2")
                         .arg(wrapTable(blueprint), columnizeWithoutWrap(columns));

    /* Prepare container with all sql queries, autoIncrement for every column uses
       alter table, so separate SQL queries are provided for every column. */
    QVector<QString> sql;
    sql.reserve(2);

    sql << std::move(sqlAlterTable);

    if (!autoIncrementStartingValues.isEmpty())
        sql << std::move(autoIncrementStartingValues);

    return sql;
}

QVector<QString>
PostgresSchemaGrammar::compileDropColumn(const Blueprint &blueprint,
                                         const DropColumnsCommand &command) const
{
    return {QStringLiteral("alter table %1 %2")
                .arg(wrapTable(blueprint),
                     columnizeWithoutWrap(prefixArray("drop column",
                                                      wrapArray(command.columns))))};
}

QVector<QString>
PostgresSchemaGrammar::compileRenameColumn(const Blueprint &blueprint,
                                           const RenameCommand &command) const
{
    // DUP schema silverqx
    return {QStringLiteral("alter table %1 rename column %2 to %3")
                .arg(wrapTable(blueprint), BaseGrammar::wrap(command.from),
                     BaseGrammar::wrap(command.to))};
}

QVector<QString>
PostgresSchemaGrammar::compilePrimary(const Blueprint &blueprint,
                                      const IndexCommand &command) const
{
    return {QStringLiteral("alter table %1 add primary key (%2)")
                .arg(wrapTable(blueprint), columnize(command.columns))};
}

QVector<QString>
PostgresSchemaGrammar::compileUnique(const Blueprint &blueprint,
                                     const IndexCommand &command) const
{
    return {QStringLiteral("alter table %1 add constraint %2 unique (%3)")
                .arg(wrapTable(blueprint), BaseGrammar::wrap(command.index),
                     columnize(command.columns))};
}

QVector<QString>
PostgresSchemaGrammar::compileIndex(const Blueprint &blueprint,
                                    const IndexCommand &command) const
{
    auto algorithm = command.algorithm.isEmpty()
                     ? EMPTY
                     : QStringLiteral(" using %1").arg(command.algorithm);

    return {QStringLiteral("create index %1 on %2%3 (%4)")
                .arg(BaseGrammar::wrap(command.index), wrapTable(blueprint),
                     std::move(algorithm), columnize(command.columns))};
}

QVector<QString>
PostgresSchemaGrammar::compileFullText(const Blueprint &blueprint,
                                       const IndexCommand &command) const
{
    static const auto TsVectorTmpl = QStringLiteral("to_tsvector(%1, %2)");

    const auto language = command.language.isEmpty() ? QStringLiteral("english")
                                                     : command.language;

    const auto columns = command.columns
            | ranges::views::transform([this, &language, &TsVectorTmpl = TsVectorTmpl]
                                       (const auto &column)
    {
        return TsVectorTmpl.arg(quoteString(language), BaseGrammar::wrap(column));
    })
            | ranges::to<QVector<QString>>();

    /* Double (()) described here, simply it's a expression not the column name:
       https://www.postgresql.org/docs/10/indexes-expressional.html */
    return {QStringLiteral("create index %1 on %2 using gin ((%3))")
                .arg(BaseGrammar::wrap(command.index),
                     wrapTable(blueprint),
                     ContainerUtils::join(columns, QStringLiteral(" || ")))};
}

QVector<QString>
PostgresSchemaGrammar::compileSpatialIndex(const Blueprint &blueprint,
                                           const IndexCommand &command) const
{
    const_cast<IndexCommand &>(command).algorithm = QStringLiteral("gist");

    return compileIndex(blueprint, command);
}

QVector<QString>
PostgresSchemaGrammar::compileForeign(const Blueprint &blueprint,
                                      const ForeignKeyCommand &command) const
{
    auto sqlCommands = SchemaGrammar::compileForeign(blueprint, command);

    Q_ASSERT(sqlCommands.size() == 1);

    auto &sql = sqlCommands.first();

    const auto isDeferrable = command.deferrable.has_value();

    if (isDeferrable)
        sql += command.deferrable ? QStringLiteral(" deferrable")
                                  : QStringLiteral(" not deferrable");

    if (isDeferrable && *command.deferrable && command.initiallyImmediate)
        sql += command.initiallyImmediate ? QStringLiteral(" initially immediate")
                                          : QStringLiteral(" initially deferred");

    if (command.notValid && *command.notValid)
        sql += QStringLiteral(" not valid");

    return sqlCommands;
}

QVector<QString>
PostgresSchemaGrammar::compileDropPrimary(const Blueprint &blueprint,
                                          const IndexCommand &/*unused*/) const
{
    auto index = BaseGrammar::wrap(QStringLiteral("%1_pkey").arg(blueprint.getTable()));

    return {QStringLiteral("alter table %1 drop constraint %2")
                .arg(wrapTable(blueprint), std::move(index))};
}

QVector<QString>
PostgresSchemaGrammar::compileDropIndex(const Blueprint &/*unused*/,
                                        const IndexCommand &command) const
{
    return {QStringLiteral("drop index %1").arg(BaseGrammar::wrap(command.index))};
}

QVector<QString>
PostgresSchemaGrammar::compileRenameIndex(const Blueprint &/*unused*/,
                                          const RenameCommand &command) const
{
    return {QStringLiteral("alter index %1 rename to %2")
                .arg(BaseGrammar::wrap(command.from), BaseGrammar::wrap(command.to))};
}

QVector<QString>
PostgresSchemaGrammar::compileComment(const Blueprint &blueprint,
                                      const CommentCommand &command) const
{
    return {QStringLiteral("comment on column %1.%2 is %3")
                .arg(wrapTable(blueprint), BaseGrammar::wrap(command.column),
                     quoteString(escapeString(command.comment)))};
}

QVector<QString>
PostgresSchemaGrammar::invokeCompileMethod(const CommandDefinition &command,
                                           const DatabaseConnection &/*unused*/,
                                           const Blueprint &blueprint) const
{
    const auto &basicCommand = reinterpret_cast<const BasicCommand &>(command);
    Q_ASSERT(typeid (QString) == typeid (basicCommand.name));
    const auto &name = basicCommand.name;

    if (name == Create)
        return compileCreate(blueprint);

    /*! Type for the compileXx() methods. */
    using CompileMemFn =
            std::function<QVector<QString>(
                const PostgresSchemaGrammar &, const Blueprint &,
                const CommandDefinition &)>;

    const auto bind = [](auto &&compileMethod)
    {
        return [compileMethod = std::forward<decltype (compileMethod)>(compileMethod)]
               (const PostgresSchemaGrammar &grammar, const Blueprint &blueprint_,
                const CommandDefinition &command_) // clazy:exclude=function-args-by-value
        {
            /* Get type of a second parameter of compile method and cast to that type. */
            const auto &castCommand =
                    reinterpret_cast<decltype (argumentType<1>(compileMethod))>(command_);

            return std::invoke(compileMethod, grammar, blueprint_, castCommand);
        };
    };

    /* Pointers to a command's compile member methods by a command name, yes yes c++ 😂.
       I have to map by QString instead of enum struct because a command.name is used
       to look up, I could use enum struct but I would have to map
       QString(command.name) -> enum. */
    T_THREAD_LOCAL
    static const std::unordered_map<QString, CompileMemFn> cached {
        {Add,              bind(&PostgresSchemaGrammar::compileAdd)},
        {Rename,           bind(&PostgresSchemaGrammar::compileRename)},
        {Drop,             bind(&PostgresSchemaGrammar::compileDrop)},
        {DropIfExists,     bind(&PostgresSchemaGrammar::compileDropIfExists)},

        {DropColumn,       bind(&PostgresSchemaGrammar::compileDropColumn)},
        {RenameColumn,     bind(&PostgresSchemaGrammar::compileRenameColumn)},

        {Primary,          bind(&PostgresSchemaGrammar::compilePrimary)},
        {Unique,           bind(&PostgresSchemaGrammar::compileUnique)},
        {Index,            bind(&PostgresSchemaGrammar::compileIndex)},
        {Fulltext,         bind(&PostgresSchemaGrammar::compileFullText)},
        {SpatialIndex,     bind(&PostgresSchemaGrammar::compileSpatialIndex)},
        {Foreign,          bind(&PostgresSchemaGrammar::compileForeign)},

        {DropPrimary,      bind(&PostgresSchemaGrammar::compileDropPrimary)},
        {DropUnique,       bind(&PostgresSchemaGrammar::compileDropUnique)},
        {DropIndex,        bind(&PostgresSchemaGrammar::compileDropIndex)},
        {DropFullText,     bind(&PostgresSchemaGrammar::compileDropFullText)},
        {DropSpatialIndex, bind(&PostgresSchemaGrammar::compileDropSpatialIndex)},
        {DropForeign,      bind(&PostgresSchemaGrammar::compileDropForeign)},

        {RenameIndex,      bind(&PostgresSchemaGrammar::compileRenameIndex)},
        // PostgreSQL specific
        {Comment,          bind(&PostgresSchemaGrammar::compileComment)},
    };

    Q_ASSERT_X(cached.contains(name),
               "PostgresSchemaGrammar::invokeCompileMethod",
               QStringLiteral("Compile methods map doesn't contain the '%1' key "
                              "(unsupported command).")
               .arg(name)
               .toUtf8().constData());

    return std::invoke(cached.at(name), *this, blueprint, command);
}

/* protected */

QString PostgresSchemaGrammar::compileCreateTable(const Blueprint &blueprint) const
{
    // DUP schema silverqx
    return QStringLiteral("%1 table %2 (%3)")
            .arg(blueprint.isTemporary() ? QStringLiteral("create temporary")
                                         : Create,
                 wrapTable(blueprint),
                 columnizeWithoutWrap(getColumns(blueprint)));
}

QString PostgresSchemaGrammar::addModifiers(QString &&sql,
                                            const ColumnDefinition &column) const
{
    constexpr static std::array modifierMethods {
        &PostgresSchemaGrammar::modifyCollate,   &PostgresSchemaGrammar::modifyIncrement,
        &PostgresSchemaGrammar::modifyNullable,  &PostgresSchemaGrammar::modifyDefault,
        &PostgresSchemaGrammar::modifyVirtualAs, &PostgresSchemaGrammar::modifyStoredAs,
    };

    for (const auto method : modifierMethods)
        sql += std::invoke(method, this, column);

    return std::move(sql);
}

QVector<QString>
PostgresSchemaGrammar::compileAutoIncrementStartingValues(const Blueprint &blueprint)
{
    const auto autoIncrementStartingValues = blueprint.autoIncrementStartingValues();

    // Nothing to compile
    if (autoIncrementStartingValues.isEmpty())
        return {};

    return autoIncrementStartingValues
            | ranges::views::transform([&blueprint](const auto &startingValue)
                                       -> QString
    {
        Q_ASSERT(startingValue.value);

        return QStringLiteral(R"(alter sequence "%1_%2_seq" restart with %3)")
                .arg(blueprint.getTable())
                .arg(startingValue.columnName)
                .arg(*startingValue.value);
    })
            | ranges::to<QVector<QString>>();
}

QVector<QString>
PostgresSchemaGrammar::compileDropConstraint(const Blueprint &blueprint,
                                             const IndexCommand &command) const
{
    return {QStringLiteral("alter table %1 drop constraint %2")
                .arg(wrapTable(blueprint), BaseGrammar::wrap(command.index))};
}

QString PostgresSchemaGrammar::escapeString(QString value) const
{
    /* Different approach used for the MySQL and PostgreSQL, for MySQL are escaped more
       special characters but for PostrgreSQL only single-quote, it doesn't matter
       though, it will work anyway.
       On MySQL escaping of ^Z, \0, and \ is needed on some environments, described here:
       https://dev.mysql.com/doc/refman/8.0/en/string-literals.html
       On PostgreSQL escaping using \ is is more SQL standard conforming, described here,
       (especially look at the caution box):
       https://www.postgresql.org/docs/current/sql-syntax-lexical.html#SQL-SYNTAX-SPECIAL-CHARS*/

    return value.replace(QLatin1Char('\''), QStringLiteral("''"));
}

QString PostgresSchemaGrammar::getType(const ColumnDefinition &column) const
{
    switch (column.type) {
    case ColumnType::Char:
        return typeChar(column);

    case ColumnType::String:
        return typeString(column);

    case ColumnType::Text:
        return typeText(column);

    case ColumnType::TinyText:
        return typeTinyText(column);

    case ColumnType::MediumText:
        return typeMediumText(column);

    case ColumnType::LongText:
        return typeLongText(column);

    case ColumnType::Integer:
        return typeInteger(column);

    case ColumnType::TinyInteger:
        return typeTinyInteger(column);

    case ColumnType::SmallInteger:
        return typeSmallInteger(column);

    case ColumnType::MediumInteger:
        return typeMediumInteger(column);

    case ColumnType::BigInteger:
        return typeBigInteger(column);

    case ColumnType::Float:
        return typeFloat(column);

    case ColumnType::Double:
        return typeDouble(column);

    case ColumnType::Decimal:
        return typeDecimal(column);

    // PostgreSQL only
    case ColumnType::Real:
        return typeReal(column);

    case ColumnType::Boolean:
        return typeBoolean(column);

    case ColumnType::Enum:
        return typeEnum(column);

    // MySQL only
//    case ColumnType::Set:
//        return typeSet(column);

    case ColumnType::Json:
        return typeJson(column);

    case ColumnType::Jsonb:
        return typeJsonb(column);

    case ColumnType::Date:
        return typeDate(column);

    case ColumnType::DateTime:
        return typeDateTime(column);

    case ColumnType::DateTimeTz:
        return typeDateTimeTz(column);

    case ColumnType::Time:
        return typeTime(column);

    case ColumnType::TimeTz:
        return typeTimeTz(column);

    case ColumnType::Timestamp:
        return typeTimestamp(column);

    case ColumnType::TimestampTz:
        return typeTimestampTz(column);

    case ColumnType::Year:
        return typeYear(column);

    case ColumnType::Binary:
        return typeBinary(column);

    case ColumnType::Uuid:
        return typeUuid(column);

    case ColumnType::IpAddress:
        return typeIpAddress(column);

    case ColumnType::MacAddress:
        return typeMacAddress(column);

    case ColumnType::Geometry:
        return typeGeometry(column);

    case ColumnType::Point:
        return typePoint(column);

    case ColumnType::LineString:
        return typeLineString(column);

    case ColumnType::Polygon:
        return typePolygon(column);

    case ColumnType::GeometryCollection:
        return typeGeometryCollection(column);

    case ColumnType::MultiPoint:
        return typeMultiPoint(column);

    case ColumnType::MultiLineString:
        return typeMultiLineString(column);

    case ColumnType::MultiPolygon:
        return typeMultiPolygon(column);

    // PostgreSQL only
    case ColumnType::MultiPolygonZ:
        return typeMultiPolygonZ(column);

    default:
        throw Exceptions::RuntimeError(
                    QStringLiteral("Unsupported column type in %1().")
                    .arg(__tiny_func__));
    }
}

QString
PostgresSchemaGrammar::generatableColumn(QString &&type, const ColumnDefinition &column)
{
    if (!column.autoIncrement && column.generatedAs.isEmpty())
        return std::move(type);

    static const std::unordered_map<QString, QString> GeneratableMap {
        {QStringLiteral("smallint"), QStringLiteral("smallserial")},
        {QStringLiteral("integer"),  QStringLiteral("serial")},
        {QStringLiteral("bigint"),   QStringLiteral("bigserial")},
    };

    if (column.autoIncrement && column.generatedAs.isEmpty())
        return GeneratableMap.at(type);

    QString options;

    if (!column.generatedAs.isEmpty())
        options = QStringLiteral(" (%1)").arg(column.generatedAs);

    return QStringLiteral("%1 generated %2 as identity%3")
            .arg(type,
                 column.always ? QStringLiteral("always") : QStringLiteral("by default"),
                 options);
}

QString
PostgresSchemaGrammar::formatPostGisType(QString &&type, const ColumnDefinition &column)
{
    if (!column.isGeometry)
        return QStringLiteral("geography(%1, %2)")
                .arg(type, column.srid ? QString::number(*column.srid)
                                       : QStringLiteral("4326"));

    // NOTE api different, Eloquent uses the column.projection for this, I'm reusing the column.srid silverqx
    if (column.srid)
        return QStringLiteral("geometry(%1, %2)")
                .arg(type, QString::number(*column.srid));

    return QStringLiteral("geometry(%1)").arg(type);
}

QString PostgresSchemaGrammar::typeChar(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("char(%1)").arg(column.length);
}

QString PostgresSchemaGrammar::typeString(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("varchar(%1)").arg(column.length);
}

QString PostgresSchemaGrammar::typeTinyText(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("varchar(255)");
}

QString PostgresSchemaGrammar::typeText(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return text_;
}

QString PostgresSchemaGrammar::typeMediumText(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return text_;
}

QString PostgresSchemaGrammar::typeLongText(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return text_;
}

QString PostgresSchemaGrammar::typeBigInteger(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return generatableColumn("bigint", column);
}

QString PostgresSchemaGrammar::typeInteger(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return generatableColumn("integer", column);
}

QString PostgresSchemaGrammar::typeMediumInteger(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return generatableColumn("integer", column);
}

QString PostgresSchemaGrammar::typeTinyInteger(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return generatableColumn("smallint", column);
}

QString PostgresSchemaGrammar::typeSmallInteger(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return generatableColumn("smallint", column);
}

QString PostgresSchemaGrammar::typeFloat(const ColumnDefinition &column) const
{
    return typeDouble(column);
}

QString PostgresSchemaGrammar::typeDouble(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("double precision");
}

QString PostgresSchemaGrammar::typeReal(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("real");
}

QString PostgresSchemaGrammar::typeDecimal(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    if (!column.total)
        return QStringLiteral("decimal");

    return QStringLiteral("decimal(%1, %2)").arg(*column.total)
                                            // Follow the SQL standard
                                            .arg(column.places ? *column.places : 0);
}

QString PostgresSchemaGrammar::typeBoolean(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("boolean");
}

QString PostgresSchemaGrammar::typeEnum(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral(R"(varchar(255) check ("%1" in (%2)))")
            .arg(column.name, quoteString(column.allowed));
}

QString PostgresSchemaGrammar::typeJson(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("json");
}

QString PostgresSchemaGrammar::typeJsonb(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("jsonb");
}

QString PostgresSchemaGrammar::typeDate(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("date");
}

QString PostgresSchemaGrammar::typeDateTime(const ColumnDefinition &column) const
{
    return typeTimestamp(column);
}

QString PostgresSchemaGrammar::typeDateTimeTz(const ColumnDefinition &column) const
{
    return typeTimestampTz(column);
}

QString PostgresSchemaGrammar::typeTime(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("time%1 without time zone")
            .arg(column.precision > 0 ? QStringLiteral("(%1)").arg(column.precision)
                                      : EMPTY);
}

QString PostgresSchemaGrammar::typeTimeTz(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("time%1 with time zone")
            .arg(column.precision > 0 ? QStringLiteral("(%1)").arg(column.precision)
                                      : EMPTY);
}

QString PostgresSchemaGrammar::typeTimestamp(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    auto columnType =
            QStringLiteral("timestamp%1 without time zone")
            /* The behavior if the precision is omitted:
               >-1 is ok so the default will be timestamp(0), the same as
               in the MySQL grammar. */
            .arg(column.precision > -1 ? QStringLiteral("(%1)").arg(column.precision)
                                       : EMPTY);

    return column.useCurrent
            ? QStringLiteral("%1 default CURRENT_TIMESTAMP").arg(columnType)
            : std::move(columnType);
}

QString PostgresSchemaGrammar::typeTimestampTz(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    auto columnType =
            QStringLiteral("timestamp%1 with time zone")
            .arg(column.precision > 0 ? QStringLiteral("(%1)").arg(column.precision)
                                      : EMPTY);

    return column.useCurrent
            ? QStringLiteral("%1 default CURRENT_TIMESTAMP").arg(columnType)
            : std::move(columnType);
}

QString PostgresSchemaGrammar::typeYear(const ColumnDefinition &column) const
{
    return typeInteger(column);
}

QString PostgresSchemaGrammar::typeBinary(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("bytea");
}

QString PostgresSchemaGrammar::typeUuid(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("uuid");
}

QString PostgresSchemaGrammar::typeIpAddress(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("inet");
}

QString PostgresSchemaGrammar::typeMacAddress(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("macaddr");
}

QString PostgresSchemaGrammar::typeGeometry(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return formatPostGisType(QStringLiteral("geometry"), column);
}

QString PostgresSchemaGrammar::typePoint(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return formatPostGisType(QStringLiteral("point"), column);
}

QString PostgresSchemaGrammar::typeLineString(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return formatPostGisType(QStringLiteral("linestring"), column);
}

QString PostgresSchemaGrammar::typePolygon(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return formatPostGisType(QStringLiteral("polygon"), column);
}

QString
PostgresSchemaGrammar::typeGeometryCollection(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return formatPostGisType(QStringLiteral("geometrycollection"), column);
}

QString PostgresSchemaGrammar::typeMultiPoint(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return formatPostGisType(QStringLiteral("multipoint"), column);
}

QString PostgresSchemaGrammar::typeMultiLineString(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return formatPostGisType(QStringLiteral("multilinestring"), column);
}

QString PostgresSchemaGrammar::typeMultiPolygon(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return formatPostGisType(QStringLiteral("multipolygon"), column);
}

QString PostgresSchemaGrammar::typeMultiPolygonZ(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return formatPostGisType(QStringLiteral("multipolygonz"), column);
}

QString PostgresSchemaGrammar::modifyVirtualAs(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    if (column.virtualAs.isEmpty())
        return {};

    return QStringLiteral(" generated always as (%1)").arg(column.virtualAs);
}

QString PostgresSchemaGrammar::modifyStoredAs(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    if (column.storedAs.isEmpty())
        return {};

    return QStringLiteral(" generated always as (%1) stored").arg(column.storedAs);
}

QString PostgresSchemaGrammar::modifyCollate(const ColumnDefinition &column) const
{
    if (column.collation.isEmpty())
        return {};

    return QStringLiteral(" collate %1").arg(wrapValue(column.collation));
}

QString PostgresSchemaGrammar::modifyNullable(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return column.nullable ? QStringLiteral(" null") : QStringLiteral(" not null");
}

QString PostgresSchemaGrammar::modifyDefault(const ColumnDefinition &column) const
{
    // DUP schema silverqx
    const auto &defaultValue = column.defaultValue;

    if (!defaultValue.isValid() || defaultValue.isNull())
        return {};

    // Default value is already quoted and escaped
    return QStringLiteral(" default %1").arg(getDefaultValue(defaultValue));
}

QString PostgresSchemaGrammar::modifyIncrement(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    static const std::unordered_set serials {
        ColumnType::BigInteger,   ColumnType::Integer,     ColumnType::MediumInteger,
        ColumnType::SmallInteger, ColumnType::TinyInteger,
    };

    if ((serials.contains(column.type) || !column.generatedAs.isEmpty()) &&
        column.autoIncrement
    )
        return QStringLiteral(" primary key");

    return {};
}

} // namespace Orm::SchemaNs::Grammars

TINYORM_END_COMMON_NAMESPACE
