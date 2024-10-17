#include "orm/schema/grammars/mysqlschemagrammar.hpp"

#include "orm/databaseconnection.hpp"
#include "orm/utils/type.hpp"

#ifdef TINYORM_USING_TINYDRIVERS
#  include "orm/exceptions/runtimeerror.hpp"
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs::Grammars
{

/* public */

/* Compile methods for the SchemaBuilder */

QString MySqlSchemaGrammar::compileCreateDatabase(
        const QString &name, DatabaseConnection &connection) const
{
    return u"create database %1 default character set %2 default collate %3"_s
            .arg(wrapValue(name),
                 wrapValue(connection.getConfig(charset_).value<QString>()),
                 wrapValue(connection.getConfig(collation_).value<QString>()));
}

QString MySqlSchemaGrammar::compileDropDatabaseIfExists(const QString &name) const
{
    return u"drop database if exists %1"_s.arg(wrapValue(name));
}

QString MySqlSchemaGrammar::compileDropAllTables(const QList<QString> &tables) const
{
    return u"drop table %1"_s.arg(columnize(tables));
}

QString MySqlSchemaGrammar::compileDropAllViews(const QList<QString> &views) const
{
    return u"drop view %1"_s.arg(columnize(views));
}

QString
MySqlSchemaGrammar::compileGetAllTables(const QList<QString> &/*unused*/) const // NOLINT(google-default-arguments)
{
    return u"show full tables where table_type = 'BASE TABLE'"_s;
}

QString
MySqlSchemaGrammar::compileGetAllViews(const QList<QString> &/*unused*/) const // NOLINT(google-default-arguments)
{
    return u"show full tables where table_type = 'VIEW'"_s;
}

QString MySqlSchemaGrammar::compileEnableForeignKeyConstraints() const
{
    return u"set foreign_key_checks = on"_s;
}

QString MySqlSchemaGrammar::compileDisableForeignKeyConstraints() const
{
    return u"set foreign_key_checks = off"_s;
}

QString MySqlSchemaGrammar::compileTableExists() const
{
    return u"select * from `information_schema`.`tables` "
            "where `table_schema` = ? and `table_name` = ? and "
              "`table_type` = 'BASE TABLE'"_s;
}

QString MySqlSchemaGrammar::compileColumnListing(const QString &/*unused*/) const // NOLINT(google-default-arguments)
{
    return u"select `column_name` as `column_name` from `information_schema`.`columns` "
            "where `table_schema` = ? and `table_name` = ?"_s;
}

/* Compile methods for commands */

QList<QString>
MySqlSchemaGrammar::compileCreate(const Blueprint &blueprint,
                                  const DatabaseConnection &connection) const
{
    // Primary SQL query for create table
    auto sqlCreateTable = compileCreateTable(blueprint);

    // Add the encoding option to the SQL query for the table
    compileCreateEncoding(sqlCreateTable, connection, blueprint);

    // Add storage engine declaration to the SQL query if has been supplied
    compileCreateEngine(sqlCreateTable, connection, blueprint);

    return {std::move(sqlCreateTable)};
}

QList<QString> MySqlSchemaGrammar::compileRename(const Blueprint &blueprint,
                                                 const RenameCommand &command) const
{
    return {u"rename table %1 to %2"_s
                .arg(wrapTable(blueprint), BaseGrammar::wrap(command.to))};
}

QList<QString> MySqlSchemaGrammar::compileAdd(const Blueprint &blueprint,
                                              const BasicCommand &/*unused*/) const
{
    return {u"alter table %1 %2"_s
                .arg(wrapTable(blueprint),
                     columnizeWithoutWrap(
                         prefixArray(u"add column"_s, getColumns(blueprint))))};
}

QList<QString> MySqlSchemaGrammar::compileChange(const Blueprint &blueprint,
                                                 const BasicCommand &/*unused*/) const
{
    auto changedColumns = blueprint.getChangedColumns();

    QList<QString> columns;
    columns.reserve(changedColumns.size());

    for (auto &column : changedColumns) {
        const auto isRenaming = !column.renameTo.isEmpty();

        columns << addModifiers(
                       u"%1 %2%3 %4"_s
                       .arg(isRenaming ? u"change column"_s : u"modify column"_s,
                            wrap(column),
                            isRenaming ? u" %1"_s.arg(BaseGrammar::wrap(column.renameTo))
                                       : EMPTY,
                            getType(column)),
                       column);
    }

    return {u"alter table %1 %2"_s.arg(wrapTable(blueprint),
                                       columnizeWithoutWrap(columns))};
}

QList<QString>
MySqlSchemaGrammar::compileDropColumn(const Blueprint &blueprint,
                                      const DropColumnsCommand &command) const
{
    return {u"alter table %1 %2"_s
                .arg(wrapTable(blueprint),
                     columnizeWithoutWrap(prefixArray(Drop,
                                                      wrapArray(command.columns))))};
}

QList<QString>
MySqlSchemaGrammar::compileRenameColumn(const Blueprint &blueprint,
                                        const RenameCommand &command) const
{
    return {u"alter table %1 rename column %2 to %3"_s
                .arg(wrapTable(blueprint), BaseGrammar::wrap(command.from),
                     BaseGrammar::wrap(command.to))};
}

QList<QString>
MySqlSchemaGrammar::compilePrimary(const Blueprint &blueprint,
                                   const IndexCommand &command) const
{
    return {u"alter table %1 add primary key %2(%3)"_s
                .arg(wrapTable(blueprint),
                     command.algorithm.isEmpty() ? EMPTY
                                                 : u"using %1"_s.arg(command.algorithm),
                     columnize(command.columns))};
}

QList<QString>
MySqlSchemaGrammar::compileUnique(const Blueprint &blueprint,
                                  const IndexCommand &command) const
{
    return {compileKey(blueprint, command, u"unique index"_s)};
}

QList<QString>
MySqlSchemaGrammar::compileIndex(const Blueprint &blueprint,
                                 const IndexCommand &command) const
{
    return {compileKey(blueprint, command, Index)};
}

QList<QString>
MySqlSchemaGrammar::compileFullText(const Blueprint &blueprint,
                                    const IndexCommand &command) const
{
    return {compileKey(blueprint, command, u"fulltext index"_s)};
}

QList<QString>
MySqlSchemaGrammar::compileSpatialIndex(const Blueprint &blueprint,
                                        const IndexCommand &command) const
{
    return {compileKey(blueprint, command, u"spatial index"_s)};
}

QList<QString>
MySqlSchemaGrammar::compileDropPrimary(const Blueprint &blueprint,
                                       const IndexCommand &/*unused*/) const
{
    return {u"alter table %1 drop primary key"_s.arg(wrapTable(blueprint))};
}

QList<QString>
MySqlSchemaGrammar::compileDropIndex(const Blueprint &blueprint,
                                     const IndexCommand &command) const
{
    return {u"alter table %1 drop index %2"_s
                .arg(wrapTable(blueprint), BaseGrammar::wrap(command.index))};
}

QList<QString>
MySqlSchemaGrammar::compileDropForeign(const Blueprint &blueprint,
                                       const IndexCommand &command) const
{
    return {u"alter table %1 drop foreign key %2"_s
                .arg(wrapTable(blueprint), BaseGrammar::wrap(command.index))};
}

QList<QString>
MySqlSchemaGrammar::compileRenameIndex(const Blueprint &blueprint,
                                       const RenameCommand &command) const
{
    return {u"alter table %1 rename index %2 to %3"_s
                .arg(wrapTable(blueprint), BaseGrammar::wrap(command.from),
                     BaseGrammar::wrap(command.to))};
}

QList<QString>
MySqlSchemaGrammar::compileTableComment(const Blueprint &blueprint,
                                        const TableCommentCommand &command) const
{
    if (command.comment.isEmpty())
        return {};

    // All escaped special characters will be correctly saved in the comment
    return {u"alter table %1 comment = %2"_s
                .arg(wrapTable(blueprint),
                     quoteString(escapeString(command.comment)))};
}

QList<QString>
MySqlSchemaGrammar::invokeCompileMethod(const CommandDefinition &command,
                                        const DatabaseConnection &connection,
                                        const Blueprint &blueprint) const
{
    const auto &basicCommand = reinterpret_cast<const BasicCommand &>(command); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    Q_ASSERT(typeid (QString) == typeid (basicCommand.name));
    const auto &name = basicCommand.name;

    /* Helps to avoid declare all compileXx() methods with a DatabaseConenction &
       parameter, only the compileCreate() needs connection argument. */
    if (name == Create)
        return compileCreate(blueprint, connection);

    /*! Type for the compileXx() methods. */
    using CompileMemFn =
            std::function<QList<QString>(
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
                    reinterpret_cast<decltype (argumentType<1>(compileMethod))>(command_); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)

            return std::invoke(compileMethod, grammar, blueprint_, castCommand);
        };
    };

    /* Pointers to a command's compile member methods by a command name, yes yes c++ 😂.
       I have to map by QString instead of enum struct because a command.name is used
       to look up, I could use enum struct but I would have to map
       QString(command.name) -> enum. */
    static const std::unordered_map<QString, CompileMemFn> cached {
        {Add,              bind(&MySqlSchemaGrammar::compileAdd)},
        {Change,           bind(&MySqlSchemaGrammar::compileChange)},
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
        {AutoIncrementStartingValue,
                           bind(&MySqlSchemaGrammar::compileAutoIncrementStartingValue)},
        {TableComment,     bind(&MySqlSchemaGrammar::compileTableComment)},

        /* PostgreSQL specific, this is not needed for MySQL, it uses modifier for column
           comments, the Comment command will never by invoked, but I'm adding
           for reference so it's clearly visible what's up. */
//        {Comment,          nullptr},
    };

    Q_ASSERT_X(cached.contains(name),
               "MySqlSchemaGrammar::invokeCompileMethod",
               u"Compile methods map doesn't contain the '%1' key "
                "(unsupported command)."_s
               .arg(name)
               .toUtf8().constData());

    return std::invoke(cached.at(name), *this, blueprint, command);
}

const std::vector<SchemaGrammar::FluentCommandItem> &
MySqlSchemaGrammar::getFluentCommands() const
{
    static const std::vector<SchemaGrammar::FluentCommandItem> cached {
        {AutoIncrementStartingValue, shouldAddAutoIncrementStartingValue},
    };

    return cached;
}

/* protected */

QString MySqlSchemaGrammar::compileCreateTable(const Blueprint &blueprint) const
{
    return u"%1 table %2 (%3)"_s
            .arg(blueprint.isTemporary() ? u"create temporary"_s
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
        &MySqlSchemaGrammar::modifyDefault,   &MySqlSchemaGrammar::modifyOnUpdate,
        &MySqlSchemaGrammar::modifyIncrement, &MySqlSchemaGrammar::modifyComment,
        &MySqlSchemaGrammar::modifyAfter,     &MySqlSchemaGrammar::modifyFirst,
    };

    for (const auto method : modifierMethods)
        sql += std::invoke(method, this, column);

    return std::move(sql);
}

void MySqlSchemaGrammar::compileCreateEncoding(
        QString &sql, const DatabaseConnection &connection, const Blueprint &blueprint)
{
    static const auto charsetTmpl = u" default character set %1"_s;
    static const auto collateTmpl = u" collate %1"_s;

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
    static const auto engineTmpl = u" engine = %1"_s;

    if (!blueprint.engine.isEmpty())
        sql += engineTmpl.arg(blueprint.engine);

    else if (const auto engine = connection.getConfig(engine_).value<QString>();
             !engine.isEmpty()
    )
        sql += engineTmpl.arg(engine);
}

QList<QString>
MySqlSchemaGrammar::compileAutoIncrementStartingValue(
        const Blueprint &blueprint,
        const AutoIncrementStartingValueCommand &command) const
{
    return {u"alter table %1 auto_increment = %2"_s.arg(wrapTable(blueprint))
                                                   .arg(command.startingValue)};
}

QString
MySqlSchemaGrammar::compileKey(const Blueprint &blueprint, const IndexCommand &command,
                               const QString &type) const
{
    return u"alter table %1 add %2 %3%4(%5)"_s
            .arg(wrapTable(blueprint), type, BaseGrammar::wrap(command.index),
                 command.algorithm.isEmpty() ? EMPTY
                                             : u" using %1"_s.arg(command.algorithm),
                 columnize(command.columns));
}

// Duplicate in the MysqlGrammar is OK
QString MySqlSchemaGrammar::wrapValue(QString value) const
{
    if (value == ASTERISK_C)
        return value;

    return u"`%1`"_s.arg(value.replace(u"`"_s, u"``"_s));
}

QString MySqlSchemaGrammar::escapeString(QString value) const
{
    /* Different approach used for the MySQL and PostgreSQL, for MySQL are escaped more
       special characters but for PostgreSQL only single-quote, it doesn't matter
       though, it will work anyway.
       On MySQL escaping of ^Z, \0, and \ is needed on some environments, described here:
       https://dev.mysql.com/doc/refman/9.1/en/string-literals.html
       On PostgreSQL escaping using \ is is more SQL standard conforming, described here,
       (especially look at the caution box):
       https://www.postgresql.org/docs/current/sql-syntax-lexical.html#SQL-SYNTAX-SPECIAL-CHARS*/

    return value
            // No need to escape these
//            .replace(QChar(QChar::LineFeed), uR"(\n)"_s)
//            .replace(QChar(QChar::Tabulation), uR"(\t)"_s)
//            .replace(QChar(0x0008), uR"(\b)"_s)
//            .replace(QChar(QChar::CarriageReturn), uR"(\r)"_s)
//            .replace(QUOTE, uR"(\")"_s)
            .replace(QChar(0x001a), u"^Z"_s)
            .replace(u'\\', uR"(\\)"_s)
            .replace(QChar(QChar::Null), uR"(\0)"_s)
            .replace(SQUOTE, u"''"_s);
}

QString MySqlSchemaGrammar::getType(ColumnDefinition &column) const
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

    case ColumnType::TinyBinary:
        return typeTinyBinary(column);

    case ColumnType::MediumBinary:
        return typeMediumBinary(column);

    case ColumnType::LongBinary:
        return typeLongBinary(column);

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
                    u"Unsupported column type in %1()."_s.arg(__tiny_func__));
    }
}

QString MySqlSchemaGrammar::typeChar(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"char(%1)"_s.arg(column.length);
}

QString MySqlSchemaGrammar::typeString(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"varchar(%1)"_s.arg(column.length);
}

QString MySqlSchemaGrammar::typeTinyText(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"tinytext"_s;
}

QString MySqlSchemaGrammar::typeText(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return text_;
}

QString MySqlSchemaGrammar::typeMediumText(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"mediumtext"_s;
}

QString MySqlSchemaGrammar::typeLongText(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"longtext"_s;
}

QString MySqlSchemaGrammar::typeBigInteger(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"bigint"_s;
}

QString MySqlSchemaGrammar::typeInteger(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"int"_s;
}

QString MySqlSchemaGrammar::typeMediumInteger(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"mediumint"_s;
}

QString MySqlSchemaGrammar::typeTinyInteger(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"tinyint"_s;
}

QString MySqlSchemaGrammar::typeSmallInteger(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"smallint"_s;
}

QString MySqlSchemaGrammar::typeFloat(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return typeDouble(column);
}

QString MySqlSchemaGrammar::typeDouble(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    if (column.total && column.places)
        return u"double(%1, %2)"_s.arg(*column.total).arg(*column.places);

    return u"double"_s;
}

QString MySqlSchemaGrammar::typeDecimal(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    if (!column.total)
        return u"decimal"_s;

    return u"decimal(%1, %2)"_s.arg(*column.total)
                               // Follow the SQL standard
                               .arg(column.places ? *column.places : 0);
}

QString MySqlSchemaGrammar::typeBoolean(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"tinyint(1)"_s;
}

QString MySqlSchemaGrammar::typeEnum(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"enum(%1)"_s.arg(quoteString(column.allowed));
}

QString MySqlSchemaGrammar::typeSet(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"set(%1)"_s.arg(quoteString(column.allowed));
}

QString MySqlSchemaGrammar::typeJson(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"json"_s;
}

QString MySqlSchemaGrammar::typeJsonb(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"json"_s;
}

QString MySqlSchemaGrammar::typeDate(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"date"_s;
}

QString MySqlSchemaGrammar::typeDateTime(ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    const auto precision = column.precision.value_or(0);

    const auto current = precision > 0 ? u"current_timestamp(%1)"_s.arg(precision)
                                       : u"current_timestamp"_s;

    if (column.useCurrent)
        column.defaultValue = Expression(current);

    if (column.useCurrentOnUpdate)
        column.onUpdate = Expression(current);

    return precision > 0 ? u"datetime(%1)"_s.arg(precision)
                         : u"datetime"_s;
}

QString MySqlSchemaGrammar::typeDateTimeTz(ColumnDefinition &column) const
{
    return typeDateTime(column);
}

QString MySqlSchemaGrammar::typeTime(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return column.precision && *column.precision > 0
            ? u"time(%1)"_s.arg(*column.precision)
            : u"time"_s;
}

QString MySqlSchemaGrammar::typeTimeTz(const ColumnDefinition &column) const
{
    return typeTime(column);
}

QString MySqlSchemaGrammar::typeTimestamp(ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    const auto precision = column.precision.value_or(0);

    const auto current = precision > 0 ? u"current_timestamp(%1)"_s.arg(precision)
                                       : u"current_timestamp"_s;

    if (column.useCurrent)
        column.defaultValue = Expression(current);

    if (column.useCurrentOnUpdate)
        column.onUpdate = Expression(current);

    /* The behavior if the precision is omitted (or 0 of course):
       MySQL default is 0 if omitted, from docs: the default precision is 0.
       (This differs from the standard SQL default of 6, for compatibility with previous
       MySQL versions.)
       So the >0 is ok, the default will be timestamp aka timestamp(0). */
    return precision > 0 ? u"timestamp(%1)"_s.arg(precision)
                         : u"timestamp"_s;
}

QString MySqlSchemaGrammar::typeTimestampTz(ColumnDefinition &column) const
{
    return typeTimestamp(column);
}

QString MySqlSchemaGrammar::typeYear(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"year"_s;
}

QString MySqlSchemaGrammar::typeTinyBinary(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"tinyblob"_s;
}

QString MySqlSchemaGrammar::typeBinary(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return blob_;
}

QString MySqlSchemaGrammar::typeMediumBinary(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"mediumblob"_s;
}

QString MySqlSchemaGrammar::typeLongBinary(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"longblob"_s;
}

QString MySqlSchemaGrammar::typeUuid(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"char(36)"_s;
}

QString MySqlSchemaGrammar::typeIpAddress(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"varchar(45)"_s;
}

QString MySqlSchemaGrammar::typeMacAddress(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"varchar(17)"_s;
}

QString MySqlSchemaGrammar::typeGeometry(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"geometry"_s;
}

QString MySqlSchemaGrammar::typePoint(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"point"_s;
}

QString MySqlSchemaGrammar::typeLineString(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"linestring"_s;
}

QString MySqlSchemaGrammar::typePolygon(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"polygon"_s;
}

QString
MySqlSchemaGrammar::typeGeometryCollection(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"geometrycollection"_s;
}

QString MySqlSchemaGrammar::typeMultiPoint(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"multipoint"_s;
}

QString MySqlSchemaGrammar::typeMultiLineString(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"multilinestring"_s;
}

QString MySqlSchemaGrammar::typeMultiPolygon(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"multipolygon"_s;
}

QString MySqlSchemaGrammar::typeComputed(const ColumnDefinition &/*unused*/) const
{
    throw Exceptions::RuntimeError(
                "This database driver requires a type, see the virtualAs / storedAs "
                "modifiers.");
}

QString MySqlSchemaGrammar::modifyUnsigned(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    if (!column.isUnsigned)
        return {};

    return u" unsigned"_s;
}

QString MySqlSchemaGrammar::modifyCharset(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    if (column.charset.isEmpty())
        return {};

    // Quotes are not used in the MySQL docs
    return u" character set %1"_s.arg(quoteString(column.charset));
}

QString MySqlSchemaGrammar::modifyCollate(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    if (column.collation.isEmpty())
        return {};

    return u" collate %1"_s.arg(quoteString(column.collation));
}

QString MySqlSchemaGrammar::modifyVirtualAs(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    if (!column.virtualAs || column.virtualAs->isEmpty())
        return {};

    return u" generated always as (%1)"_s.arg(*column.virtualAs);
}

QString MySqlSchemaGrammar::modifyStoredAs(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    if (!column.storedAs || column.storedAs->isEmpty())
        return {};

    return u" generated always as (%1) stored"_s.arg(*column.storedAs);
}

QString MySqlSchemaGrammar::modifyNullable(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    // NOTE api different, I want the nullable to behave the same as everywhere else silverqx
    /* MySQL doesn't need any special logic for generated columns (virtualAs and
       storedAs), it accepts both, null and also not null for generated columns, I have
       tried it. */
    if (!m_isMaria ||
        ((!column.virtualAs || column.virtualAs->isEmpty()) &&
         (!column.storedAs  || column.storedAs->isEmpty()))
    )
        return column.nullable && *column.nullable ? u" null"_s
                                                   : u" not null"_s;

    /* MariaDB doesn't support setting a nullable modifier (NULL or NOT NULL)
       on generated columns, a query fails if nullable is set. The best that can be done
       is to ignore it. 🫤 */
    return {};
}

QString MySqlSchemaGrammar::modifyInvisible(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    if (!column.invisible)
        return {};

    return u" invisible"_s;
}

QString MySqlSchemaGrammar::modifySrid(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    if (const auto &srid = column.srid;
        srid && *srid > 0
    )
        return u" srid %1"_s.arg(*srid);

    return {};
}

QString MySqlSchemaGrammar::modifyDefault(const ColumnDefinition &column) const
{
    const auto &defaultValue = column.defaultValue;

    if (!defaultValue.isValid() || defaultValue.isNull())
        return {};

    // Default value is already quoted and escaped inside the getDefaultValue()
    return u" default %1"_s.arg(getDefaultValue(defaultValue));
}

QString MySqlSchemaGrammar::modifyOnUpdate(const ColumnDefinition &column) const
{
    const auto &onUpdate = column.onUpdate;

    if (!onUpdate.isValid() || onUpdate.isNull())
        return {};

    // Default value is already quoted and escaped inside the getDefaultValue()
    return u" on update %1"_s.arg(getDefaultValue(onUpdate));
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

    return u" auto_increment primary key"_s;
}

QString MySqlSchemaGrammar::modifyComment(const ColumnDefinition &column) const
{
    if (column.comment.isEmpty())
        return {};

    // All escaped special characters will be correctly saved in the comment
    return u" comment %1"_s.arg(quoteString(escapeString(column.comment)));
}

QString MySqlSchemaGrammar::modifyAfter(const ColumnDefinition &column) const
{
    if (column.after.isEmpty())
        return {};

    return u" after %1"_s.arg(BaseGrammar::wrap(column.after));
}

QString MySqlSchemaGrammar::modifyFirst(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    if (!column.first)
        return {};

    return u" first"_s;
}

} // namespace Orm::SchemaNs::Grammars

TINYORM_END_COMMON_NAMESPACE
