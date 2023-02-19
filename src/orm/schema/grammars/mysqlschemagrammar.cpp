#include "orm/schema/grammars/mysqlschemagrammar.hpp"

#include <unordered_set>

#include "orm/databaseconnection.hpp"
#include "orm/macros/threadlocal.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs::Grammars
{

/* Compile methods for the SchemaBuilder */

/* public */

QString MySqlSchemaGrammar::compileCreateDatabase(
        const QString &name, DatabaseConnection &connection) const
{
    return QStringLiteral("create database %1 "
                          "default character set %2 default collate %3")
            .arg(wrapValue(name),
                 wrapValue(connection.getConfig(charset_).value<QString>()),
                 wrapValue(connection.getConfig(collation_).value<QString>()));
}

QString MySqlSchemaGrammar::compileDropDatabaseIfExists(const QString &name) const
{
    return QStringLiteral("drop database if exists %1").arg(wrapValue(name));
}

QString MySqlSchemaGrammar::compileDropAllTables(const QVector<QString> &tables) const
{
    return QStringLiteral("drop table %1").arg(columnize(tables));
}

QString MySqlSchemaGrammar::compileDropAllViews(const QVector<QString> &views) const
{
    return QStringLiteral("drop view %1").arg(columnize(views));
}

QString
MySqlSchemaGrammar::compileGetAllTables(const QVector<QString> &/*unused*/) const
{
    return QStringLiteral("show full tables where table_type = 'BASE TABLE'");
}

QString
MySqlSchemaGrammar::compileGetAllViews(const QVector<QString> &/*unused*/) const
{
    return QStringLiteral("show full tables where table_type = 'VIEW'");
}

QString MySqlSchemaGrammar::compileEnableForeignKeyConstraints() const
{
    return QStringLiteral("set foreign_key_checks = on");
}

QString MySqlSchemaGrammar::compileDisableForeignKeyConstraints() const
{
    return QStringLiteral("set foreign_key_checks = off");
}

QString MySqlSchemaGrammar::compileTableExists() const
{
    return QStringLiteral("select * "
                          "from `information_schema`.`tables` "
                          "where `table_schema` = ? and `table_name` = ? and "
                            "`table_type` = 'BASE TABLE'");
}

QString MySqlSchemaGrammar::compileColumnListing(const QString &/*unused*/) const
{
    return QStringLiteral("select `column_name` as `column_name` "
                          "from `information_schema`.`columns` "
                            "where `table_schema` = ? and `table_name` = ?");
}

/* Compile methods for commands */

QVector<QString>
MySqlSchemaGrammar::compileCreate(const Blueprint &blueprint,
                                  const DatabaseConnection &connection) const
{
    // Primary SQL query for create table
    auto sqlCreateTable = compileCreateTable(blueprint);

    // Add the encoding option to the SQL for the table
    compileCreateEncoding(sqlCreateTable, connection, blueprint);

    // Add storage engine declaration to the SQL query if has been supplied
    compileCreateEngine(sqlCreateTable, connection, blueprint);

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

QVector<QString> MySqlSchemaGrammar::compileRename(const Blueprint &blueprint,
                                                   const RenameCommand &command) const
{
    return {QStringLiteral("rename table %1 to %2")
                .arg(wrapTable(blueprint), BaseGrammar::wrap(command.to))};
}

QVector<QString> MySqlSchemaGrammar::compileAdd(const Blueprint &blueprint,
                                                const BasicCommand &/*unused*/) const
{
    auto columns = prefixArray(Add, getColumns(blueprint));

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
MySqlSchemaGrammar::compileDropColumn(const Blueprint &blueprint,
                                      const DropColumnsCommand &command) const
{
    return {QStringLiteral("alter table %1 %2")
                .arg(wrapTable(blueprint),
                     columnizeWithoutWrap(prefixArray(Drop,
                                                      wrapArray(command.columns))))};
}

QVector<QString>
MySqlSchemaGrammar::compileRenameColumn(const Blueprint &blueprint,
                                        const RenameCommand &command) const
{
    return {QStringLiteral("alter table %1 rename column %2 to %3")
                .arg(wrapTable(blueprint), BaseGrammar::wrap(command.from),
                     BaseGrammar::wrap(command.to))};
}

QVector<QString>
MySqlSchemaGrammar::compilePrimary(const Blueprint &blueprint,
                                   const IndexCommand &command) const
{
    // CUR schema, check this, why it is doing, I have to make command non-const, I have checked it and name is not used in the compileKey(), so it's unnecessary, ask at Laravel's github isssues silverqx
//    command.name.clear();
//    const_cast<IndexCommand &>(command).name.clear();

    return {compileKey(blueprint, command, QStringLiteral("primary key"))};
}

QVector<QString>
MySqlSchemaGrammar::compileUnique(const Blueprint &blueprint,
                                  const IndexCommand &command) const
{
    return {compileKey(blueprint, command, Unique)};
}

QVector<QString>
MySqlSchemaGrammar::compileIndex(const Blueprint &blueprint,
                                 const IndexCommand &command) const
{
    return {compileKey(blueprint, command, Index)};
}

QVector<QString>
MySqlSchemaGrammar::compileFullText(const Blueprint &blueprint,
                                    const IndexCommand &command) const
{
    return {compileKey(blueprint, command, Fulltext)};
}

QVector<QString>
MySqlSchemaGrammar::compileSpatialIndex(const Blueprint &blueprint,
                                        const IndexCommand &command) const
{
    return {compileKey(blueprint, command, QStringLiteral("spatial index"))};
}

QVector<QString>
MySqlSchemaGrammar::compileDropPrimary(const Blueprint &blueprint,
                                       const IndexCommand &/*unused*/) const
{
    return {QStringLiteral("alter table %1 drop primary key")
                .arg(wrapTable(blueprint))};
}

QVector<QString>
MySqlSchemaGrammar::compileDropIndex(const Blueprint &blueprint,
                                     const IndexCommand &command) const
{
    return {QStringLiteral("alter table %1 drop index %2")
                .arg(wrapTable(blueprint), BaseGrammar::wrap(command.index))};
}

QVector<QString>
MySqlSchemaGrammar::compileDropForeign(const Blueprint &blueprint,
                                       const IndexCommand &command) const
{
    return {QStringLiteral("alter table %1 drop foreign key %2")
                .arg(wrapTable(blueprint), BaseGrammar::wrap(command.index))};
}

QVector<QString>
MySqlSchemaGrammar::compileRenameIndex(const Blueprint &blueprint,
                                       const RenameCommand &command) const
{
    return {QStringLiteral("alter table %1 rename index %2 to %3")
                .arg(wrapTable(blueprint), BaseGrammar::wrap(command.from),
                     BaseGrammar::wrap(command.to))};
}

QVector<QString>
MySqlSchemaGrammar::compileTableComment(const Blueprint &blueprint,
                                        const TableCommentCommand &command) const
{
    if (command.comment.isEmpty())
        return {};

    // All escaped special characters will be correctly saved in the comment
    return {QStringLiteral("alter table %1 comment = %2")
                .arg(wrapTable(blueprint),
                     quoteString(escapeString(command.comment)))};
}

QVector<QString>
MySqlSchemaGrammar::invokeCompileMethod(const CommandDefinition &command,
                                        const DatabaseConnection &connection,
                                        const Blueprint &blueprint) const
{
    const auto &basicCommand = reinterpret_cast<const BasicCommand &>(command);
    Q_ASSERT(typeid (QString) == typeid (basicCommand.name));
    const auto &name = basicCommand.name;

    /* Helps to avoid declare all compileXx() methods with a DatabaseConenction &
       parameter, only the compileCreate() needs connection argument. */
    if (name == Create)
        return compileCreate(blueprint, connection);

    /*! Type for the compileXx() methods. */
    using CompileMemFn =
            std::function<QVector<QString>(
                const MySqlSchemaGrammar &, const Blueprint &,
                const CommandDefinition &)>;

    const auto bind = [](auto &&compileMethod)
    {
        return [compileMethod = std::forward<decltype (compileMethod)>(compileMethod)]
               (const MySqlSchemaGrammar &grammar, const Blueprint &blueprint_,
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
        {Add,              bind(&MySqlSchemaGrammar::compileAdd)},
        {Rename,           bind(&MySqlSchemaGrammar::compileRename)},
        {Drop,             bind(&MySqlSchemaGrammar::compileDrop)},
        {DropIfExists,     bind(&MySqlSchemaGrammar::compileDropIfExists)},

        {DropColumn,       bind(&MySqlSchemaGrammar::compileDropColumn)},
        {RenameColumn,     bind(&MySqlSchemaGrammar::compileRenameColumn)},

        {Primary,          bind(&MySqlSchemaGrammar::compilePrimary)},
        {Unique,           bind(&MySqlSchemaGrammar::compileUnique)},
        {Index,            bind(&MySqlSchemaGrammar::compileIndex)},
        {Fulltext,         bind(&MySqlSchemaGrammar::compileFullText)},
        {SpatialIndex,     bind(&MySqlSchemaGrammar::compileSpatialIndex)},
        {Foreign,          bind(&MySqlSchemaGrammar::compileForeign)},

        {DropPrimary,      bind(&MySqlSchemaGrammar::compileDropPrimary)},
        {DropUnique,       bind(&MySqlSchemaGrammar::compileDropUnique)},
        {DropIndex,        bind(&MySqlSchemaGrammar::compileDropIndex)},
        {DropFullText,     bind(&MySqlSchemaGrammar::compileDropFullText)},
        {DropSpatialIndex, bind(&MySqlSchemaGrammar::compileDropSpatialIndex)},
        {DropForeign,      bind(&MySqlSchemaGrammar::compileDropForeign)},

        {RenameIndex,      bind(&MySqlSchemaGrammar::compileRenameIndex)},

        // MySQL and PostgreSQL specific
        {TableComment,     bind(&MySqlSchemaGrammar::compileTableComment)},
    };

    Q_ASSERT_X(cached.contains(name),
               "MySqlSchemaGrammar::invokeCompileMethod",
               QStringLiteral("Compile methods map doesn't contain the '%1' key "
                              "(unsupported command).")
               .arg(name)
               .toUtf8().constData());

    return std::invoke(cached.at(name), *this, blueprint, command);
}

/* protected */

QString MySqlSchemaGrammar::compileCreateTable(const Blueprint &blueprint) const
{
    return QStringLiteral("%1 table %2 (%3)")
            .arg(blueprint.isTemporary() ? QStringLiteral("create temporary")
                                         : Create,
                 wrapTable(blueprint),
                 columnizeWithoutWrap(getColumns(blueprint)));
}

QString MySqlSchemaGrammar::addModifiers(QString &&sql,
                                         const ColumnDefinition &column) const
{
    constexpr static std::array modifierMethods {
        &MySqlSchemaGrammar::modifyUnsigned,  &MySqlSchemaGrammar::modifyCharset,
        &MySqlSchemaGrammar::modifyCollate,   &MySqlSchemaGrammar::modifyVirtualAs,
        &MySqlSchemaGrammar::modifyStoredAs,  &MySqlSchemaGrammar::modifyNullable,
        &MySqlSchemaGrammar::modifyInvisible, &MySqlSchemaGrammar::modifySrid,
        &MySqlSchemaGrammar::modifyDefault,   &MySqlSchemaGrammar::modifyIncrement,
        &MySqlSchemaGrammar::modifyComment,   &MySqlSchemaGrammar::modifyAfter,
        &MySqlSchemaGrammar::modifyFirst,
    };

    for (const auto method : modifierMethods)
        sql += std::invoke(method, this, column);

    return std::move(sql);
}

void MySqlSchemaGrammar::compileCreateEncoding(
        QString &sql, const DatabaseConnection &connection, const Blueprint &blueprint)
{
    static const auto charsetTmpl = QStringLiteral(" default character set %1");
    static const auto collateTmpl = QStringLiteral(" collate %1");

    /* First we will set the character set if one has been set on either the create
       blueprint itself or on the root configuration for the connection that the
       table is being created on. We will add these to the create table query. */
    if (!blueprint.charset.isEmpty())
        sql += charsetTmpl.arg(blueprint.charset);

    else if (const auto charset = connection.getConfig(charset_).value<QString>();
             !charset.isEmpty()
    )
        sql += charsetTmpl.arg(charset);

    /* Next we will add the collation to the create table statement if one has been
       added to either this create table blueprint or the configuration for this
       connection that the query is targeting. We'll add it to this SQL query. */
    if (!blueprint.collation.isEmpty())
        sql += collateTmpl.arg(quoteString(blueprint.collation));
    else if (const auto collation = connection.getConfig(collation_).value<QString>();
             !collation.isEmpty()
    )
        sql += collateTmpl.arg(quoteString(collation));
}

void MySqlSchemaGrammar::compileCreateEngine(
        QString &sql, const DatabaseConnection &connection, const Blueprint &blueprint)
{
    static const auto engineTmpl = QStringLiteral(" engine = %1");

    if (!blueprint.engine.isEmpty())
        sql += engineTmpl.arg(blueprint.engine);

    else if (const auto engine = connection.getConfig(engine_).value<QString>();
             !engine.isEmpty()
    )
        sql += engineTmpl.arg(engine);
}

QVector<QString>
MySqlSchemaGrammar::compileAutoIncrementStartingValues(const Blueprint &blueprint) const
{
    const auto autoIncrementStartingValues = blueprint.autoIncrementStartingValues();

    // Nothing to compile
    if (autoIncrementStartingValues.isEmpty())
        return {};

    return autoIncrementStartingValues
            | ranges::views::transform([this, &blueprint](const auto &startingValue)
                                       -> QString
    {
        Q_ASSERT(startingValue.value);

        return QStringLiteral("alter table %1 auto_increment = %2")
                .arg(wrapTable(blueprint))
                .arg(*startingValue.value);
    })
            | ranges::to<QVector<QString>>();
}

QString
MySqlSchemaGrammar::compileKey(const Blueprint &blueprint, const IndexCommand &command,
                               const QString &type) const
{
    static const auto usingTmpl = QStringLiteral(" using %1");

    return QStringLiteral("alter table %1 add %2 %3%4(%5)")
            .arg(wrapTable(blueprint), type, BaseGrammar::wrap(command.index),
                 command.algorithm.isEmpty() ? ""
                                             : usingTmpl.arg(command.algorithm),
                 columnize(command.columns));
}

// Duplicate in the MysqlGrammar is OK
QString MySqlSchemaGrammar::wrapValue(QString value) const
{
    if (value == ASTERISK_C)
        return value;

    return QStringLiteral("`%1`").arg(value.replace(QStringLiteral("`"),
                                                    QStringLiteral("``")));
}

QString MySqlSchemaGrammar::escapeString(QString value) const
{
    /* Different approach used for the MySQL and PostgreSQL, for MySQL are escaped more
       special characters but for PostrgreSQL only single-quote, it doesn't matter
       though, it will work anyway.
       On MySQL escaping of ^Z, \0, and \ is needed on some environments, described here:
       https://dev.mysql.com/doc/refman/8.0/en/string-literals.html
       On PostgreSQL escaping using \ is is more SQL standard conforming, described here,
       (especially look at the caution box):
       https://www.postgresql.org/docs/current/sql-syntax-lexical.html#SQL-SYNTAX-SPECIAL-CHARS*/

    return value
            // No need to escape these
//            .replace(QChar(QChar::LineFeed), "\\n")
//            .replace(QChar(QChar::Tabulation), "\\t")
//            .replace(QChar(0x0008), "\\b")
//            .replace(QChar(QChar::CarriageReturn), "\\r")
//            .replace(QUOTE, "\\\"")
            .replace(QChar(0x001a), QStringLiteral("^Z"))
            .replace(QLatin1Char('\\'), QStringLiteral("\\\\"))
            .replace(QChar(QChar::Null), QStringLiteral("\\0"))
            .replace(SQUOTE, "''");
}

QString MySqlSchemaGrammar::getType(const ColumnDefinition &column) const
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
//    case ColumnType::Real:
//        return typeReal(column);

    case ColumnType::Boolean:
        return typeBoolean(column);

    case ColumnType::Enum:
        return typeEnum(column);

    // MySQL only
    case ColumnType::Set:
        return typeSet(column);

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
//    case ColumnType::MultiPolygonZ:
//        return typeMultiPolygonZ(column);

    default:
        throw Exceptions::RuntimeError(
                    QStringLiteral("Unsupported column type in %1().")
                    .arg(__tiny_func__));
    }
}

QString MySqlSchemaGrammar::typeChar(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("char(%1)").arg(column.length);
}

QString MySqlSchemaGrammar::typeString(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("varchar(%1)").arg(column.length);
}

QString MySqlSchemaGrammar::typeTinyText(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("tinytext");
}

QString MySqlSchemaGrammar::typeText(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return text_;
}

QString MySqlSchemaGrammar::typeMediumText(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("mediumtext");
}

QString MySqlSchemaGrammar::typeLongText(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("longtext");
}

QString MySqlSchemaGrammar::typeBigInteger(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("bigint");
}

QString MySqlSchemaGrammar::typeInteger(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("int");
}

QString MySqlSchemaGrammar::typeMediumInteger(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("mediumint");
}

QString MySqlSchemaGrammar::typeTinyInteger(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("tinyint");
}

QString MySqlSchemaGrammar::typeSmallInteger(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("smallint");
}

QString MySqlSchemaGrammar::typeFloat(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return typeDouble(column);
}

QString MySqlSchemaGrammar::typeDouble(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    if (column.total && column.places)
        return QStringLiteral("double(%1, %2)").arg(*column.total).arg(*column.places);

    return QStringLiteral("double");
}

QString MySqlSchemaGrammar::typeDecimal(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    if (!column.total)
        return QStringLiteral("decimal");

    return QStringLiteral("decimal(%1, %2)").arg(*column.total)
                                            // Follow the SQL standard
                                            .arg(column.places ? *column.places : 0);
}

QString MySqlSchemaGrammar::typeBoolean(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("tinyint(1)");
}

QString MySqlSchemaGrammar::typeEnum(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("enum(%1)").arg(quoteString(column.allowed));
}

QString MySqlSchemaGrammar::typeSet(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("set(%1)").arg(quoteString(column.allowed));
}

QString MySqlSchemaGrammar::typeJson(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("json");
}

QString MySqlSchemaGrammar::typeJsonb(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("json");
}

QString MySqlSchemaGrammar::typeDate(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("date");
}

QString MySqlSchemaGrammar::typeDateTime(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    auto columnType = column.precision > 0
                      ? QStringLiteral("datetime(%1)").arg(column.precision)
                      : QStringLiteral("datetime");

    const auto current = column.precision > 0
                         ? QStringLiteral("CURRENT_TIMESTAMP(%1)").arg(column.precision)
                         : QStringLiteral("CURRENT_TIMESTAMP");

    columnType = column.useCurrent
                 ? QStringLiteral("%1 default %2").arg(columnType, current)
                 : columnType;

    return column.useCurrentOnUpdate
            ? QStringLiteral("%1 on update %2").arg(columnType, current)
            : columnType;
}

QString MySqlSchemaGrammar::typeDateTimeTz(const ColumnDefinition &column) const
{
    return typeDateTime(column);
}

QString MySqlSchemaGrammar::typeTime(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return column.precision > 0 ? QStringLiteral("time(%1)").arg(column.precision)
                                : QStringLiteral("time");
}

QString MySqlSchemaGrammar::typeTimeTz(const ColumnDefinition &column) const
{
    return typeTime(column);
}

QString MySqlSchemaGrammar::typeTimestamp(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    /* The behavior if the precision is omitted (or 0 of course):
       >0 is ok so the default will be: timestamp and the MySQL default is 0 if omitted.
       The same as in the PostgreSQL grammar. */
    auto columnType = column.precision > 0
                      ? QStringLiteral("timestamp(%1)").arg(column.precision)
                      : QStringLiteral("timestamp");

    const auto current = column.precision > 0
                         ? QStringLiteral("CURRENT_TIMESTAMP(%1)").arg(column.precision)
                         : QStringLiteral("CURRENT_TIMESTAMP");

    columnType = column.useCurrent
                 ? QStringLiteral("%1 default %2").arg(columnType, current)
                 : columnType;

    return column.useCurrentOnUpdate
            ? QStringLiteral("%1 on update %2").arg(columnType, current)
            : columnType;
}

QString MySqlSchemaGrammar::typeTimestampTz(const ColumnDefinition &column) const
{
    return typeTimestamp(column);
}

QString MySqlSchemaGrammar::typeYear(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("year");
}

QString MySqlSchemaGrammar::typeBinary(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("blob");
}

QString MySqlSchemaGrammar::typeUuid(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("char(36)");
}

QString MySqlSchemaGrammar::typeIpAddress(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("varchar(45)");
}

QString MySqlSchemaGrammar::typeMacAddress(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("varchar(17)");
}

QString MySqlSchemaGrammar::typeGeometry(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("geometry");
}

QString MySqlSchemaGrammar::typePoint(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("point");
}

QString MySqlSchemaGrammar::typeLineString(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("linestring");
}

QString MySqlSchemaGrammar::typePolygon(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("polygon");
}

QString
MySqlSchemaGrammar::typeGeometryCollection(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("geometrycollection");
}

QString MySqlSchemaGrammar::typeMultiPoint(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("multipoint");
}

QString MySqlSchemaGrammar::typeMultiLineString(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("multilinestring");
}

QString MySqlSchemaGrammar::typeMultiPolygon(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("multipolygon");
}

QString MySqlSchemaGrammar::typeComputed(const ColumnDefinition &/*unused*/) const
{
    throw Exceptions::RuntimeError(
                "This database driver requires a type, see the virtualAs / storedAs "
                "modifiers.");
}

QString MySqlSchemaGrammar::modifyVirtualAs(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    if (column.virtualAs.isEmpty())
        return {};

    return QStringLiteral(" as (%1)").arg(column.virtualAs);
}

QString MySqlSchemaGrammar::modifyStoredAs(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    if (column.storedAs.isEmpty())
        return {};

    return QStringLiteral(" as (%1) stored").arg(column.storedAs);
}

QString MySqlSchemaGrammar::modifyUnsigned(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    if (!column.isUnsigned)
        return {};

    return QStringLiteral(" unsigned");
}

QString MySqlSchemaGrammar::modifyCharset(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    if (column.charset.isEmpty())
        return {};

    // Quotes are not used in the MySQL docs
    return QStringLiteral(" character set %1").arg(quoteString(column.charset));
}

QString MySqlSchemaGrammar::modifyCollate(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    if (column.collation.isEmpty())
        return {};

    return QStringLiteral(" collate %1").arg(quoteString(column.collation));
}

QString MySqlSchemaGrammar::modifyNullable(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    static const auto notNull = QStringLiteral(" not null");

    if (column.virtualAs.isEmpty() && column.storedAs.isEmpty())
        return column.nullable ? QStringLiteral(" null") : notNull;

    // Don't set null for virtual/stored columns, set 'not null' only
    if (column.nullable)
        return {};

    return notNull;
}

QString MySqlSchemaGrammar::modifyInvisible(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    if (!column.invisible)
        return {};

    return QStringLiteral(" invisible");
}

QString MySqlSchemaGrammar::modifyDefault(const ColumnDefinition &column) const
{
    const auto &defaultValue = column.defaultValue;

    if (!defaultValue.isValid() || defaultValue.isNull())
        return {};

    // Default value is already quoted and escaped
    return QStringLiteral(" default %1").arg(getDefaultValue(defaultValue));
}

QString MySqlSchemaGrammar::modifyIncrement(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    static const std::unordered_set serials {
        ColumnType::BigInteger,   ColumnType::Integer,     ColumnType::MediumInteger,
        ColumnType::SmallInteger, ColumnType::TinyInteger
    };

    // This reverse OR algebra is sick 😮🙃
    if (!column.autoIncrement || !serials.contains(column.type))
        return {};

    return QStringLiteral(" auto_increment primary key");
}

QString MySqlSchemaGrammar::modifyFirst(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    if (!column.first)
        return {};

    return QStringLiteral(" first");
}

QString MySqlSchemaGrammar::modifyAfter(const ColumnDefinition &column) const
{
    if (column.after.isEmpty())
        return {};

    return QStringLiteral(" after %1").arg(BaseGrammar::wrap(column.after));
}

QString MySqlSchemaGrammar::modifyComment(const ColumnDefinition &column) const
{
    if (column.comment.isEmpty())
        return {};

    // All escaped special characters will be correctly saved in the comment
    return QStringLiteral(" comment %1").arg(quoteString(escapeString(column.comment)));
}

QString MySqlSchemaGrammar::modifySrid(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    if (const auto &srid = column.srid;
        srid && *srid > 0
    )
        return QStringLiteral(" srid %1").arg(*srid);

    return {};
}

} // namespace Orm::SchemaNs::Grammars

TINYORM_END_COMMON_NAMESPACE
