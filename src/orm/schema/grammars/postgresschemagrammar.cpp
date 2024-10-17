#include "orm/schema/grammars/postgresschemagrammar.hpp"

#include "orm/databaseconnection.hpp"
#include "orm/exceptions/logicerror.hpp"
#include "orm/utils/type.hpp"

#ifdef TINYORM_USING_TINYDRIVERS
#  include "orm/exceptions/runtimeerror.hpp"
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs::Grammars
{

/* public */

/* Compile methods for the SchemaBuilder */

QString PostgresSchemaGrammar::compileCreateDatabase(
        const QString &name, DatabaseConnection &connection) const
{
    return u"create database %1 encoding %2"_s
            .arg(wrapValue(name),
                 wrapValue(connection.getConfig(charset_).value<QString>()));
}

QString PostgresSchemaGrammar::compileDropDatabaseIfExists(const QString &name) const
{
    return u"drop database if exists %1"_s.arg(wrapValue(name));
}

QString PostgresSchemaGrammar::compileDropAllTables(const QList<QString> &tables) const
{
    return u"drop table %1 cascade"_s.arg(columnizeWithoutWrap(escapeNames(tables)));
}

QString PostgresSchemaGrammar::compileDropAllViews(const QList<QString> &views) const
{
    return u"drop view %1 cascade"_s.arg(columnizeWithoutWrap(escapeNames(views)));
}

QString
PostgresSchemaGrammar::compileGetAllTables(const QList<QString> &databases) const // NOLINT(google-default-arguments)
{
    return u"select tablename, "
              "concat('\"', schemaname, '\".\"', tablename, '\"') as qualifiedname "
            "from pg_catalog.pg_tables "
              "where schemaname in (%1)"_s
            .arg(quoteString(databases));
}

QString
PostgresSchemaGrammar::compileGetAllViews(const QList<QString> &databases) const // NOLINT(google-default-arguments)
{
    return u"select viewname, "
              "concat('\"', schemaname, '\".\"', viewname, '\"') as qualifiedname "
            "from pg_catalog.pg_views "
              "where schemaname in (%1)"_s
            .arg(quoteString(databases));
}

QString PostgresSchemaGrammar::compileEnableForeignKeyConstraints() const
{
    return u"set constraints all immediate"_s;
}

QString PostgresSchemaGrammar::compileDisableForeignKeyConstraints() const
{
    return u"set constraints all deferred"_s;
}

QString PostgresSchemaGrammar::compileTableExists() const
{
    return u"select * from information_schema.tables "
            "where table_catalog = ? and table_schema = ? and "
              "table_name = ? and table_type = 'BASE TABLE'"_s;
}

QString PostgresSchemaGrammar::compileColumnListing(const QString &/*unused*/) const // NOLINT(google-default-arguments)
{
    return u"select column_name from information_schema.columns "
            "where table_catalog = ? and table_schema = ? and table_name = ?"_s;
}

/* Compile methods for commands */

QList<QString>
PostgresSchemaGrammar::compileCreate(const Blueprint &blueprint) const
{
    return {u"%1 table %2 (%3)"_s
                .arg(blueprint.isTemporary() ? u"create temporary"_s : Create,
                     wrapTable(blueprint),
                     columnizeWithoutWrap(getColumns(blueprint)))};
}

QList<QString>
PostgresSchemaGrammar::compileRename(const Blueprint &blueprint,
                                     const RenameCommand &command) const
{
    return {u"alter table %1 rename to %2"_s.arg(wrapTable(blueprint),
                                                 BaseGrammar::wrap(command.to))};
}

QList<QString>
PostgresSchemaGrammar::compileAdd(const Blueprint &blueprint,
                                  const BasicCommand &/*unused*/) const
{
    return {u"alter table %1 %2"_s
                .arg(wrapTable(blueprint),
                     columnizeWithoutWrap(
                         prefixArray(u"add column"_s, getColumns(blueprint))))};
}

QList<QString>
PostgresSchemaGrammar::compileChange(const Blueprint &blueprint,
                                     const BasicCommand &/*unused*/) const
{
    auto changedColumns = blueprint.getChangedColumns();

    QList<QString> columns;
    columns.reserve(changedColumns.size());

    for (auto &column : changedColumns) {
        QList<QString> changes;
        changes.reserve(m_modifierMethodsForChangeSize + 1);

        const auto collate = modifyCollate(column);

        // The column type with the collate has to be defined at once
        changes << u"type %1%2"_s
                   .arg(getType(column),
                        collate.isEmpty() ? EMPTY : collate.constFirst());

        // All other modifiers have to be alone so the "alter column" can be prepended
        changes << getModifiersForChange(column);

        columns << columnizeWithoutWrap(
                       prefixArray(u"alter column %1"_s.arg(wrap(column)), changes));
    }

    return {u"alter table %1 %2"_s.arg(wrapTable(blueprint),
                                       columnizeWithoutWrap(columns))};
}

QList<QString>
PostgresSchemaGrammar::compileDropColumn(const Blueprint &blueprint,
                                         const DropColumnsCommand &command) const
{
    return {u"alter table %1 %2"_s
                .arg(wrapTable(blueprint),
                     columnizeWithoutWrap(prefixArray("drop column",
                                                      wrapArray(command.columns))))};
}

QList<QString>
PostgresSchemaGrammar::compileRenameColumn(const Blueprint &blueprint,
                                           const RenameCommand &command) const
{
    return {u"alter table %1 rename column %2 to %3"_s
                .arg(wrapTable(blueprint), BaseGrammar::wrap(command.from),
                     BaseGrammar::wrap(command.to))};
}

QList<QString>
PostgresSchemaGrammar::compilePrimary(const Blueprint &blueprint,
                                      const IndexCommand &command) const
{
    return {u"alter table %1 add primary key (%2)"_s
                .arg(wrapTable(blueprint), columnize(command.columns))};
}

QList<QString>
PostgresSchemaGrammar::compileUnique(const Blueprint &blueprint,
                                     const IndexCommand &command) const
{
    return {u"alter table %1 add constraint %2 unique (%3)"_s
                .arg(wrapTable(blueprint), BaseGrammar::wrap(command.index),
                     columnize(command.columns))};
}

QList<QString>
PostgresSchemaGrammar::compileIndex(const Blueprint &blueprint,
                                    const IndexCommand &command) const
{
    const auto algorithm = command.algorithm.isEmpty()
                           ? EMPTY
                           : u" using %1"_s.arg(command.algorithm);

    return {u"create index %1 on %2%3 (%4)"_s
                .arg(BaseGrammar::wrap(command.index), wrapTable(blueprint),
                     algorithm, columnize(command.columns))};
}

QList<QString>
PostgresSchemaGrammar::compileFullText(const Blueprint &blueprint,
                                       const IndexCommand &command) const
{
    static const auto TsVectorTmpl = u"to_tsvector(%1, %2)"_s;

    const auto language = command.language.isEmpty() ? u"english"_s
                                                     : command.language;

    const auto columns = command.columns
            | ranges::views::transform([this, &language, &TsVectorTmpl = TsVectorTmpl]
                                       (const auto &column)
    {
        return TsVectorTmpl.arg(quoteString(language), BaseGrammar::wrap(column));
    })
            | ranges::to<QList<QString>>();

    /* Double (()) described here, simply it's a expression not the column name:
       https://www.postgresql.org/docs/10/indexes-expressional.html */
    return {u"create index %1 on %2 using gin ((%3))"_s
                .arg(BaseGrammar::wrap(command.index),
                     wrapTable(blueprint),
                     ContainerUtils::join(columns, u" || "_s))};
}

QList<QString>
PostgresSchemaGrammar::compileSpatialIndex(const Blueprint &blueprint,
                                           const IndexCommand &command) const
{
    // TODO schema, get rid of all const_cast<> in schema grammars, this is only one from all compileXyz() methods which needs to modify the command, if there will be also another commands that need to modify the command then make the command non-const silverqx
    const_cast<IndexCommand &>(command).algorithm = u"gist"_s; // NOLINT(cppcoreguidelines-pro-type-const-cast)

    return compileIndex(blueprint, command);
}

QList<QString>
PostgresSchemaGrammar::compileForeign(const Blueprint &blueprint,
                                      const ForeignKeyCommand &command) const
{
    auto sqlCommands = SchemaGrammar::compileForeign(blueprint, command);

    Q_ASSERT(sqlCommands.size() == 1);

    auto &sql = sqlCommands.first();

    const auto isDeferrable = command.deferrable.has_value();

    if (isDeferrable)
        sql += command.deferrable ? u" deferrable"_s
                                  : u" not deferrable"_s;

    if (isDeferrable && *command.deferrable && command.initiallyImmediate)
        sql += command.initiallyImmediate ? u" initially immediate"_s
                                          : u" initially deferred"_s;

    if (command.notValid && *command.notValid)
        sql += u" not valid"_s;

    return sqlCommands;
}

QList<QString>
PostgresSchemaGrammar::compileDropPrimary(const Blueprint &blueprint,
                                          const IndexCommand &/*unused*/) const
{
    const auto index = BaseGrammar::wrap(u"%1_pkey"_s
                                         .arg(blueprint.getTable()));

    return {u"alter table %1 drop constraint %2"_s
            .arg(wrapTable(blueprint), index)};
}

QList<QString>
PostgresSchemaGrammar::compileDropIndex(const Blueprint &/*unused*/,
                                        const IndexCommand &command) const
{
    return {u"drop index %1"_s.arg(BaseGrammar::wrap(command.index))};
}

QList<QString>
PostgresSchemaGrammar::compileRenameIndex(const Blueprint &/*unused*/,
                                          const RenameCommand &command) const
{
    return {u"alter index %1 rename to %2"_s.arg(BaseGrammar::wrap(command.from),
                                                 BaseGrammar::wrap(command.to))};
}

QList<QString>
PostgresSchemaGrammar::compileComment(const Blueprint &blueprint,
                                      const CommentCommand &command) const
{
    const auto isCommentEmpty = command.comment.isEmpty();

    if (isCommentEmpty && !command.change)
        return {};

    return {u"comment on column %1.%2 is %3"_s
                .arg(wrapTable(blueprint), BaseGrammar::wrap(command.column),
                                     // Remove a column comment (used during change())
                     isCommentEmpty ? null_
                                    : quoteString(escapeString(command.comment)))};
}

QList<QString>
PostgresSchemaGrammar::compileTableComment(const Blueprint &blueprint,
                                           const TableCommentCommand &command) const
{
    if (command.comment.isEmpty())
        return {};

    // All escaped special characters will be correctly saved in the comment
    return {u"comment on table %1 is %2"_s
                .arg(wrapTable(blueprint),
                     quoteString(escapeString(command.comment)))};
}

QList<QString>
PostgresSchemaGrammar::invokeCompileMethod(const CommandDefinition &command,
                                           const DatabaseConnection &/*unused*/,
                                           const Blueprint &blueprint) const
{
    const auto &basicCommand = reinterpret_cast<const BasicCommand &>(command); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    Q_ASSERT(typeid (QString) == typeid (basicCommand.name));
    const auto &name = basicCommand.name;

    if (name == Create)
        return compileCreate(blueprint);

    /*! Type for the compileXx() methods. */
    using CompileMemFn =
            std::function<QList<QString>(
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
                    reinterpret_cast<decltype (argumentType<1>(compileMethod))>(command_); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)

            return std::invoke(compileMethod, grammar, blueprint_, castCommand);
        };
    };

    /* Pointers to a command's compile member methods by a command name, yes yes c++ 😂.
       I have to map by QString instead of enum struct because a command.name is used
       to look up, I could use enum struct but I would have to map
       QString(command.name) -> enum. */
    static const std::unordered_map<QString, CompileMemFn> cached {
        {Add,              bind(&PostgresSchemaGrammar::compileAdd)},
        {Change,           bind(&PostgresSchemaGrammar::compileChange)},
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

        // PostgreSQL and MySQL specific
        {AutoIncrementStartingValue,
                           bind(&PostgresSchemaGrammar::compileAutoIncrementStartingValue)},
        {TableComment,     bind(&PostgresSchemaGrammar::compileTableComment)},
    };

    Q_ASSERT_X(cached.contains(name),
               "PostgresSchemaGrammar::invokeCompileMethod",
               u"Compile methods map doesn't contain the '%1' key "
                "(unsupported command)."_s
               .arg(name)
               .toUtf8().constData());

    return std::invoke(cached.at(name), *this, blueprint, command);
}

const std::vector<SchemaGrammar::FluentCommandItem> &
PostgresSchemaGrammar::getFluentCommands() const
{
    static const std::vector<SchemaGrammar::FluentCommandItem> cached {
        {AutoIncrementStartingValue, shouldAddAutoIncrementStartingValue},
        {Comment, [](const ColumnDefinition &column)
                  { return !column.comment.isEmpty() || column.change; }},
    };

    return cached;
}

/* protected */

QString PostgresSchemaGrammar::addModifiers(QString &&sql,
                                            const ColumnDefinition &column) const
{
    constexpr static std::array modifierMethods {
        &PostgresSchemaGrammar::modifyCollate,    &PostgresSchemaGrammar::modifyIncrement,
        &PostgresSchemaGrammar::modifyNullable,   &PostgresSchemaGrammar::modifyDefault,
        &PostgresSchemaGrammar::modifyVirtualAs,  &PostgresSchemaGrammar::modifyStoredAs,
        &PostgresSchemaGrammar::modifyGeneratedAs,
    };

    for (const auto method : modifierMethods)
        /* Postgres is different here, it returns a vector as it needs to return
           2 modifiers from the modifyGeneratedAs(). */
        sql += ContainerUtils::join(std::invoke(method, this, column), EMPTY);

    return std::move(sql);
}

QList<QString>
PostgresSchemaGrammar::getModifiersForChange(const ColumnDefinition &column) const
{
    QList<QString> modifiers;
    modifiers.reserve(static_cast<decltype (modifiers)::size_type>(
                          m_modifierMethodsForChangeSize));

    for (const auto method : m_modifierMethodsForChange)
        modifiers << std::invoke(method, this, column);

    return modifiers;
}

QList<QString>
PostgresSchemaGrammar::compileAutoIncrementStartingValue( // NOLINT(readability-convert-member-functions-to-static)
        const Blueprint &blueprint,
        const AutoIncrementStartingValueCommand &command) const
{
    return {uR"(alter sequence "%1_%2_seq" restart with %3)"_s
                .arg(blueprint.getTable(), command.column)
                .arg(command.startingValue)};
}

QList<QString>
PostgresSchemaGrammar::compileDropConstraint(const Blueprint &blueprint,
                                             const IndexCommand &command) const
{
    return {u"alter table %1 drop constraint %2"_s
                .arg(wrapTable(blueprint), BaseGrammar::wrap(command.index))};
}

QString PostgresSchemaGrammar::escapeString(QString value) const
{
    /* Different approach used for the MySQL and PostgreSQL, for MySQL are escaped more
       special characters but for PostgreSQL only single-quote, it doesn't matter
       though, it will work anyway.
       On MySQL escaping of ^Z, \0, and \ is needed on some environments, described here:
       https://dev.mysql.com/doc/refman/9.1/en/string-literals.html
       On PostgreSQL escaping using \ is is more SQL standard conforming, described here,
       (especially look at the caution box):
       https://www.postgresql.org/docs/current/sql-syntax-lexical.html#SQL-SYNTAX-SPECIAL-CHARS*/

    return value.replace(SQUOTE, u"''"_s);
}

QString PostgresSchemaGrammar::getType(ColumnDefinition &column) const
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
    case ColumnType::MultiPolygonZ:
        return typeMultiPolygonZ(column);

    default:
        throw Exceptions::RuntimeError(
                    u"Unsupported column type in %1()."_s.arg(__tiny_func__));
    }
}

QString
PostgresSchemaGrammar::formatPostGisType(const QString &type,
                                         const ColumnDefinition &column)
{
    if (!column.isGeometry)
        return u"geography(%1, %2)"_s
                .arg(type, column.srid ? QString::number(*column.srid)
                                       : u"4326"_s);

    // NOTE api different, Eloquent uses the column.projection for this, I'm reusing the column.srid silverqx
    if (column.srid)
        return u"geometry(%1, %2)"_s.arg(type, QString::number(*column.srid));

    return u"geometry(%1)"_s.arg(type);
}

QString PostgresSchemaGrammar::typeChar(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"char(%1)"_s.arg(column.length);
}

QString PostgresSchemaGrammar::typeString(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"varchar(%1)"_s.arg(column.length);
}

QString PostgresSchemaGrammar::typeTinyText(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"varchar(255)"_s;
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
    return column.autoIncrement && column.generatedAs.isNull() // Can't be generatedAs.isEmpty()!
            ? u"bigserial"_s
            : u"bigint"_s;
}

QString PostgresSchemaGrammar::typeInteger(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return column.autoIncrement && column.generatedAs.isNull() // Can't be generatedAs.isEmpty()!
            ? u"serial"_s
            : u"integer"_s;
}

QString PostgresSchemaGrammar::typeMediumInteger(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return typeInteger(column);
}

QString PostgresSchemaGrammar::typeTinyInteger(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return typeSmallInteger(column);
}

QString PostgresSchemaGrammar::typeSmallInteger(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return column.autoIncrement && column.generatedAs.isNull() // Can't be generatedAs.isEmpty()!
            ? u"smallserial"_s
            : u"smallint"_s;
}

QString PostgresSchemaGrammar::typeFloat(const ColumnDefinition &column) const
{
    return typeDouble(column);
}

QString PostgresSchemaGrammar::typeDouble(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"double precision"_s;
}

QString PostgresSchemaGrammar::typeReal(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"real"_s;
}

QString PostgresSchemaGrammar::typeDecimal(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    if (!column.total)
        return u"decimal"_s;

    return u"decimal(%1, %2)"_s.arg(*column.total)
                               // Follow the SQL standard
                               .arg(column.places ? *column.places : 0);
}

QString PostgresSchemaGrammar::typeBoolean(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"boolean"_s;
}

QString PostgresSchemaGrammar::typeEnum(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return uR"(varchar(255) check ("%1" in (%2)))"_s.arg(column.name,
                                                         quoteString(column.allowed));
}

QString PostgresSchemaGrammar::typeJson(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"json"_s;
}

QString PostgresSchemaGrammar::typeJsonb(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"jsonb"_s;
}

QString PostgresSchemaGrammar::typeDate(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"date"_s;
}

QString PostgresSchemaGrammar::typeDateTime(ColumnDefinition &column) const
{
    return typeTimestamp(column);
}

QString PostgresSchemaGrammar::typeDateTimeTz(ColumnDefinition &column) const
{
    return typeTimestampTz(column);
}

QString PostgresSchemaGrammar::typeTime(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"time%1 without time zone"_s
            /* The behavior if the precision is omitted (or 0 of course):
               PostgreSQL default is 6 if omitted, from docs: If no precision is
               specified in a constant specification, it defaults to the precision
               of the literal value (but not more than 6 digits).
               So the >-1 is ok, the default will be time(0). */
            .arg(column.precision && *column.precision > -1
                 ? u"(%1)"_s.arg(*column.precision)
                 : EMPTY);
}

QString PostgresSchemaGrammar::typeTimeTz(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"time%1 with time zone"_s
            .arg(column.precision && *column.precision > -1
                 ? u"(%1)"_s.arg(*column.precision)
                 : EMPTY);
}

QString PostgresSchemaGrammar::typeTimestamp(ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    if (column.useCurrent)
        column.defaultValue = Expression(u"current_timestamp"_s);

    return u"timestamp%1 without time zone"_s
            /* The behavior if the precision is omitted (or 0 of course):
               PostgreSQL default is 6 if omitted, from docs: If no precision is
               specified in a constant specification, it defaults to the precision
               of the literal value (but not more than 6 digits).
               So the >-1 is ok, the default will be timestamp(0). */
            .arg(column.precision && *column.precision > -1
                 ? u"(%1)"_s.arg(*column.precision) : EMPTY);
}

QString PostgresSchemaGrammar::typeTimestampTz(ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    if (column.useCurrent)
        column.defaultValue = Expression(u"current_timestamp"_s);

    return u"timestamp%1 with time zone"_s
            .arg(column.precision && *column.precision > -1
                 ? u"(%1)"_s.arg(*column.precision) : EMPTY);
}

QString PostgresSchemaGrammar::typeYear(const ColumnDefinition &column) const
{
    return typeInteger(column);
}

QString PostgresSchemaGrammar::typeTinyBinary(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return bytea_;
}

QString PostgresSchemaGrammar::typeBinary(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return bytea_;
}

QString PostgresSchemaGrammar::typeMediumBinary(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return bytea_;
}

QString PostgresSchemaGrammar::typeLongBinary(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return bytea_;
}

QString PostgresSchemaGrammar::typeUuid(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"uuid"_s;
}

QString PostgresSchemaGrammar::typeIpAddress(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"inet"_s;
}

QString PostgresSchemaGrammar::typeMacAddress(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return u"macaddr"_s;
}

QString PostgresSchemaGrammar::typeGeometry(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return formatPostGisType(u"geometry"_s, column);
}

QString PostgresSchemaGrammar::typePoint(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return formatPostGisType(u"point"_s, column);
}

QString PostgresSchemaGrammar::typeLineString(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return formatPostGisType(u"linestring"_s, column);
}

QString PostgresSchemaGrammar::typePolygon(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return formatPostGisType(u"polygon"_s, column);
}

QString
PostgresSchemaGrammar::typeGeometryCollection(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return formatPostGisType(u"geometrycollection"_s, column);
}

QString PostgresSchemaGrammar::typeMultiPoint(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return formatPostGisType(u"multipoint"_s, column);
}

QString PostgresSchemaGrammar::typeMultiLineString(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return formatPostGisType(u"multilinestring"_s, column);
}

QString PostgresSchemaGrammar::typeMultiPolygon(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return formatPostGisType(u"multipolygon"_s, column);
}

QString PostgresSchemaGrammar::typeMultiPolygonZ(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return formatPostGisType(u"multipolygonz"_s, column);
}

QList<QString>
PostgresSchemaGrammar::modifyCollate(const ColumnDefinition &column) const
{
    if (column.collation.isEmpty())
        return {};

    return {u" collate %1"_s.arg(wrapValue(column.collation))};
}

QList<QString>
PostgresSchemaGrammar::modifyIncrement(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    static const std::unordered_set serials {
        ColumnType::BigInteger,   ColumnType::Integer,     ColumnType::MediumInteger,
        ColumnType::SmallInteger, ColumnType::TinyInteger,
    };

    // I'm not going to invert this condition for the early return 🤯
    if (!column.change &&
        (serials.contains(column.type) || !column.generatedAs.isNull()) && // Can't be generatedAs.isEmpty()!
        column.autoIncrement
    )
        return {u" primary key"_s};

    return {};
}

QList<QString>
PostgresSchemaGrammar::modifyNullable(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    if (column.change)
        return {column.nullable && *column.nullable ? u"drop not null"_s
                                                    : u"set not null"_s};

    /* PostgreSQL doesn't need any special logic for generated columns (virtualAs and
       storedAs), it accepts both, null and also not null for generated columns, I have
       tried it. */
    return {column.nullable && *column.nullable ? u" null"_s : u" not null"_s};
}

QList<QString>
PostgresSchemaGrammar::modifyDefault(const ColumnDefinition &column) const
{
    const auto &defaultValue = column.defaultValue;
    const auto isNotValidOrNull = !defaultValue.isValid() || defaultValue.isNull();

    // Default value is already quoted and escaped inside the getDefaultValue()

    if (column.change)
        return {isNotValidOrNull ? u"drop default"_s
                                 : u"set default %1"_s
                                   .arg(getDefaultValue(defaultValue))};

    if (isNotValidOrNull)
        return {};

    return {u" default %1"_s.arg(getDefaultValue(defaultValue))};
}

QList<QString>
PostgresSchemaGrammar::modifyVirtualAs(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    /* Currently, PostgreSQL 15 doesn't support virtual generated columns, only stored,
       so this method is useless. */

    if (!column.virtualAs)
        return {};

    if (column.change) {
        if (column.virtualAs->isEmpty())
            return {u"drop expression if exists"_s};

        throwModifyingGeneratedColumn();
    }

    return {u" generated always as (%1)"_s.arg(*column.virtualAs)};
}

QList<QString>
PostgresSchemaGrammar::modifyStoredAs(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    if (!column.storedAs)
        return {};

    if (column.change) {
        if (column.storedAs->isEmpty())
            return {u"drop expression if exists"_s};

        throwModifyingGeneratedColumn();
    }

    return {u" generated always as (%1) stored"_s.arg(*column.storedAs)};
}

QList<QString>
PostgresSchemaGrammar::modifyGeneratedAs(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    QString sql;
    sql.reserve(100);

    /* generatedAs.isNull() mean it was not defined at all, and isEmpty() it was called
       like generatedAs(). */
    if (!column.generatedAs.isNull())
        sql += u" generated %1 as identity%2"_s
                    // ALWAYS and BY DEFAULT clause
               .arg(column.always ? u"always"_s : u"by default"_s,
                    // Sequence options clause
                    !column.generatedAs.isEmpty() ? u" (%1)"_s.arg(column.generatedAs)
                                                  : EMPTY);

    if (column.change) {
        QList<QString> changes;
        changes.reserve(2);

        changes << u"drop identity if exists"_s;

        if (!sql.isEmpty())
            changes << std::move(sql.prepend(Add));

        return changes;
    }

    return {sql};
}

/* private */

void PostgresSchemaGrammar::throwModifyingGeneratedColumn()
{
    throw Exceptions::LogicError(
                "The PostgreSQL database does not support modifying generated columns.");
}

} // namespace Orm::SchemaNs::Grammars

TINYORM_END_COMMON_NAMESPACE
