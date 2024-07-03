#include "orm/schema/grammars/sqliteschemagrammar.hpp"

#include <QRegularExpression>

#include <unordered_set>

#include <range/v3/view/filter.hpp>
#include <range/v3/view/move.hpp>
#include <range/v3/view/remove_if.hpp>

#include "orm/exceptions/runtimeerror.hpp"
#include "orm/schema/blueprint.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs::Grammars
{

/* public */

/* Compile methods for the SchemaBuilder */

QString
SQLiteSchemaGrammar::compileDropAllTables(const QList<QString> &/*unused*/) const
{
    return QStringLiteral("delete from sqlite_master "
                          "where type in ('table', 'index', 'trigger')");
}

QString
SQLiteSchemaGrammar::compileDropAllViews(const QList<QString> &/*unused*/) const
{
    return QStringLiteral("delete from sqlite_master where type in ('view')");
}

QString
SQLiteSchemaGrammar::compileGetAllTables(const QList<QString> &/*unused*/) const // NOLINT(google-default-arguments)
{
    return QStringLiteral("select name, type "
                          "from sqlite_master "
                          "where type = 'table' and name not like 'sqlite_%'");
}

QString SQLiteSchemaGrammar::compileGetAllViews(const QList<QString> &/*unused*/) const // NOLINT(google-default-arguments)
{
    return QStringLiteral("select name, type "
                          "from sqlite_master where type = 'view'");
}

QString SQLiteSchemaGrammar::compileEnableForeignKeyConstraints() const
{
    return QStringLiteral("pragma foreign_keys = on");
}

QString SQLiteSchemaGrammar::compileDisableForeignKeyConstraints() const
{
    return QStringLiteral("pragma foreign_keys = off");
}

QString SQLiteSchemaGrammar::compileEnableWriteableSchema()
{
    return QStringLiteral("pragma writable_schema = on");
}

QString SQLiteSchemaGrammar::compileDisableWriteableSchema()
{
    return QStringLiteral("pragma writable_schema = off");
}

QString SQLiteSchemaGrammar::compileRebuild()
{
    return QStringLiteral("vacuum");
}

QString SQLiteSchemaGrammar::compileTableExists() const
{
    return QStringLiteral(
                "select * from sqlite_master where type = 'table' and name = ?");
}

QString SQLiteSchemaGrammar::compileColumnListing(const QString &table) const // NOLINT(google-default-arguments)
{
    return QStringLiteral("pragma table_info(%1)").arg(BaseGrammar::wrap(table));
}

/* Compile methods for commands */

QList<QString> SQLiteSchemaGrammar::compileCreate(const Blueprint &blueprint) const
{
    return {QStringLiteral("%1 table %2 (%3%4%5)")
                .arg(blueprint.isTemporary() ? QStringLiteral("create temporary")
                                             : Create,
                     wrapTable(blueprint),
                     columnizeWithoutWrap(getColumns(blueprint)),
                     addForeignKeys(blueprint),
                     addPrimaryKeys(blueprint))};
}

QList<QString> SQLiteSchemaGrammar::compileRename(const Blueprint &blueprint,
                                                  const RenameCommand &command) const
{
    return {QStringLiteral("alter table %1 rename to %2")
                .arg(wrapTable(blueprint), BaseGrammar::wrap(command.to))};
}

QList<QString> SQLiteSchemaGrammar::compileAdd(const Blueprint &blueprint,
                                               const BasicCommand &/*unused*/) const
{
    // TODO regex, avoid it silverqx
    static const QRegularExpression regex(QStringLiteral(R"(as \(.*\) stored)"));

    const auto columns = prefixArray(QStringLiteral("add column"), getColumns(blueprint));

    return columns
            | ranges::views::remove_if([&regex = regex](const auto &column)
    {
        return regex.match(column).hasMatch();
    })
            | ranges::views::transform([this, &blueprint](const auto &column) -> QString
    {
        return QStringLiteral("alter table %1 %2").arg(wrapTable(blueprint), column);
    })
            | ranges::to<QList<QString>>();
}

QList<QString>
SQLiteSchemaGrammar::compileDropColumn(const Blueprint &blueprint,
                                       const DropColumnsCommand &command) const
{
    QList<QString> sql;
    sql.reserve(command.columns.size());

    for (const auto &column : command.columns)
        sql << QStringLiteral("alter table %1 drop column %2")
               .arg(wrapTable(blueprint), BaseGrammar::wrap(column));

    return sql;
}

QList<QString>
SQLiteSchemaGrammar::compileRenameColumn(const Blueprint &blueprint,
                                         const RenameCommand &command) const
{
    return {QStringLiteral("alter table %1 rename column %2 to %3")
                .arg(wrapTable(blueprint), BaseGrammar::wrap(command.from),
                     BaseGrammar::wrap(command.to))};
}

QList<QString> SQLiteSchemaGrammar::compileUnique(const Blueprint &blueprint,
                                                  const IndexCommand &command) const
{
    return {QStringLiteral("create unique index %1 on %2 (%3)")
                .arg(BaseGrammar::wrap(command.index),
                     wrapTable(blueprint),
                     columnize(command.columns))};
}

QList<QString> SQLiteSchemaGrammar::compileIndex(const Blueprint &blueprint,
                                                 const IndexCommand &command) const
{
    return {QStringLiteral("create index %1 on %2 (%3)")
                .arg(BaseGrammar::wrap(command.index),
                     wrapTable(blueprint),
                     columnize(command.columns))};
}

QList<QString>
SQLiteSchemaGrammar::compileSpatialIndex(const Blueprint &/*unused*/,    // NOLINT(readability-convert-member-functions-to-static)
                                         const IndexCommand &/*unused*/) const
{
    throw Exceptions::RuntimeError(
                "The database driver in use does not support spatial indexes.");
}

QList<QString>
SQLiteSchemaGrammar::compileDropPrimary(const Blueprint &/*unused*/,    // NOLINT(readability-convert-member-functions-to-static)
                                        const IndexCommand &/*unused*/) const
{
    // CUR schema sqlite, unify all this exceptions silverqx
    throw Exceptions::RuntimeError(
                "The database driver in use does not support dropping primary indexes "
                "(you would need to re-create the table).");
}

QList<QString> SQLiteSchemaGrammar::compileDropIndex(const Blueprint &/*unused*/,
                                                     const IndexCommand &command) const
{
    return {QStringLiteral("drop index %1").arg(BaseGrammar::wrap(command.index))};
}

QList<QString>
SQLiteSchemaGrammar::compileDropSpatialIndex(const Blueprint &/*unused*/,    // NOLINT(readability-convert-member-functions-to-static)
                                             const IndexCommand &/*unused*/) const
{
    throw Exceptions::RuntimeError(
                "The database driver in use does not support dropping spatial indexes.");
}

QList<QString>
SQLiteSchemaGrammar::compileDropForeign(const Blueprint &/*unused*/,    // NOLINT(readability-convert-member-functions-to-static)
                                        const IndexCommand &/*unused*/) const
{
    throw Exceptions::RuntimeError(
                "The database driver in use does not support dropping foreign key "
                "indexes (you would need to re-create the table).");
}

QList<QString>
SQLiteSchemaGrammar::compileRenameIndex(const Blueprint &/*unused*/,     // NOLINT(readability-convert-member-functions-to-static)
                                        const RenameCommand &/*unused*/) const
{
    throw Exceptions::RuntimeError(
                "The database driver in use does not support renaming indexes, please "
                "drop and re-create index manually.");
}

QList<QString>
SQLiteSchemaGrammar::invokeCompileMethod(const CommandDefinition &command,
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
                const SQLiteSchemaGrammar &, const Blueprint &,
                const CommandDefinition &)>;

    const auto bind = [](auto &&compileMethod)
    {
        return [compileMethod = std::forward<decltype (compileMethod)>(compileMethod)]
               (const SQLiteSchemaGrammar &grammar, const Blueprint &blueprint_,
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
        {Add,              bind(&SQLiteSchemaGrammar::compileAdd)},
        {Change,           bind(&SQLiteSchemaGrammar::compileChange)},
        {Rename,           bind(&SQLiteSchemaGrammar::compileRename)},
        {Drop,             bind(&SQLiteSchemaGrammar::compileDrop)},
        {DropIfExists,     bind(&SQLiteSchemaGrammar::compileDropIfExists)},

        {DropColumn,       bind(&SQLiteSchemaGrammar::compileDropColumn)},
        {RenameColumn,     bind(&SQLiteSchemaGrammar::compileRenameColumn)},

        {Primary,          nullptr},
        {Unique,           bind(&SQLiteSchemaGrammar::compileUnique)},
        {Index,            bind(&SQLiteSchemaGrammar::compileIndex)},
        {Fulltext,         bind(&SQLiteSchemaGrammar::compileFullText)},
        {SpatialIndex,     bind(&SQLiteSchemaGrammar::compileSpatialIndex)},
        {Foreign,          nullptr},

        {DropPrimary,      bind(&SQLiteSchemaGrammar::compileDropPrimary)},
        {DropUnique,       bind(&SQLiteSchemaGrammar::compileDropUnique)},
        {DropIndex,        bind(&SQLiteSchemaGrammar::compileDropIndex)},
        {DropFullText,     bind(&SQLiteSchemaGrammar::compileDropFullText)},
        {DropSpatialIndex, bind(&SQLiteSchemaGrammar::compileDropSpatialIndex)},
        {DropForeign,      bind(&SQLiteSchemaGrammar::compileDropForeign)},

        {RenameIndex,      bind(&SQLiteSchemaGrammar::compileRenameIndex)},

        // PostgreSQL specific
        {Comment,          nullptr},
        // PostgreSQL and MySQL specific
        {TableComment,     nullptr},
    };

    Q_ASSERT_X(cached.contains(name),
               "SQLiteSchemaGrammar::invokeCompileMethod",
               QStringLiteral("Compile methods map doesn't contain the '%1' key "
                              "(unsupported command).")
               .arg(name)
               .toUtf8().constData());

    /* Needed because of nullptr values so user is still able to call table.primary() or
       table.foreign(), appropriate command is added in the blueprint and here is called
       nothing. */
    const auto &method = cached.at(name);

    return method ? std::invoke(method, *this, blueprint, command)
                  : QList<QString>();
}

/* protected */

/* Compile methods for commands */

QString SQLiteSchemaGrammar::addForeignKeys(const Blueprint &blueprint) const
{
    // Prevent clazy detach warning
    const auto foreignCommands = getCommandsByName(blueprint, Foreign);

    QString sql;

    /* Loop through all of the foreign key commands and add them to the create table SQL
       we are building, since SQLite needs foreign keys on the tables creation. */
    for (const auto &foreignBase : foreignCommands) {
        Q_ASSERT(foreignBase);
        /* static_cast should not be used in this type of casting, from a base to derived
           so use reinterpret_cast instead to be clear that it's ok (primarily for
           compiler and especially for linters/static analyzers). */
        const auto &foreign =
                *std::reinterpret_pointer_cast<ForeignKeyCommand>(foreignBase);

        sql += getForeignKey(foreign);

        // Append on delete and on update if defined
        if (!foreign.onDelete.isEmpty())
            sql += QStringLiteral(" on delete %1").arg(foreign.onDelete);

        if (!foreign.onUpdate.isEmpty())
            sql += QStringLiteral(" on update %1").arg(foreign.onUpdate);
    }

    return sql;
}

QString SQLiteSchemaGrammar::getForeignKey(const ForeignKeyCommand &foreign) const
{
    /* We need to columnize the columns that the foreign key is being defined for
       so that it is a properly formatted list. Once we have done this, we can
       return the foreign key SQL declaration to the calling method for use. */
    return QStringLiteral(", foreign key(%1) references %2(%3)")
            .arg(columnize(foreign.columns),
                 BaseGrammar::wrapTable(foreign.on),
                 columnize(foreign.references));
}

QString SQLiteSchemaGrammar::addPrimaryKeys(const Blueprint &blueprint) const
{
    auto primary = getCommandByName(blueprint, Primary);

    if (!primary)
        return {};

    return QStringLiteral(", primary key (%1)")
            .arg(columnize(
                     std::reinterpret_pointer_cast<IndexCommand>(primary)->columns));
}

std::shared_ptr<CommandDefinition>
SQLiteSchemaGrammar::getCommandByName(const Blueprint &blueprint, const QString &name)
{
    auto commands = getCommandsByName(blueprint, name);

    return commands.isEmpty() ? nullptr : std::move(commands.first());
}

QList<std::shared_ptr<CommandDefinition>>
SQLiteSchemaGrammar::getCommandsByName(const Blueprint &blueprint, const QString &name)
{
    return blueprint.getCommands()
            | ranges::views::filter([&name](const auto &command)
    {
        return std::reinterpret_pointer_cast<BasicCommand>(command)->name == name;
    })
            | ranges::to<QList<std::shared_ptr<CommandDefinition>>>();
}

/* Others */

QString SQLiteSchemaGrammar::addModifiers(QString &&sql,
                                          const ColumnDefinition &column) const
{
    constexpr static std::array modifierMethods {
        &SQLiteSchemaGrammar::modifyVirtualAs, &SQLiteSchemaGrammar::modifyStoredAs,
        &SQLiteSchemaGrammar::modifyNullable,  &SQLiteSchemaGrammar::modifyDefault,
        &SQLiteSchemaGrammar::modifyIncrement,
    };

    for (const auto method : modifierMethods)
        sql += std::invoke(method, this, column);

    return std::move(sql);
}

QString SQLiteSchemaGrammar::escapeString(QString value) const // clazy:exclude=function-args-by-ref
{
    /* Different approach used for the MySQL and PostgreSQL, for MySQL are escaped more
       special characters but for PostgreSQL only single-quote, it doesn't matter
       though, it will work anyway.
       On MySQL escaping of ^Z, \0, and \ is needed on some environments, described here:
       https://dev.mysql.com/doc/refman/9.0/en/string-literals.html
       On PostgreSQL escaping using \ is is more SQL standard conforming, described here,
       (especially look at the caution box):
       https://www.postgresql.org/docs/current/sql-syntax-lexical.html#SQL-SYNTAX-SPECIAL-CHARS*/

    return value.replace(SQUOTE, QStringLiteral("''"));
}

QString SQLiteSchemaGrammar::getType(ColumnDefinition &column) const
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
//    case ColumnType::MultiPolygonZ:
//        return typeMultiPolygonZ(column);

    default:
        throw Exceptions::RuntimeError(
                    QStringLiteral("Unsupported column type in %1().")
                    .arg(__tiny_func__));
    }
}

QString SQLiteSchemaGrammar::typeChar(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return varchar_;
}

QString SQLiteSchemaGrammar::typeString(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return varchar_;
}

QString SQLiteSchemaGrammar::typeTinyText(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return text_;
}

QString SQLiteSchemaGrammar::typeText(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return text_;
}

QString SQLiteSchemaGrammar::typeMediumText(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return text_;
}

QString SQLiteSchemaGrammar::typeLongText(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return text_;
}

QString SQLiteSchemaGrammar::typeBigInteger(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return integer_;
}

QString SQLiteSchemaGrammar::typeInteger(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return integer_;
}

QString SQLiteSchemaGrammar::typeMediumInteger(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return integer_;
}

QString SQLiteSchemaGrammar::typeTinyInteger(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return integer_;
}

QString SQLiteSchemaGrammar::typeSmallInteger(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return integer_;
}

QString SQLiteSchemaGrammar::typeFloat(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return float_;
}

QString SQLiteSchemaGrammar::typeDouble(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return float_;
}

//QString SQLiteSchemaGrammar::typeReal(const ColumnDefinition &/*unused*/) const
//{
//    return QStringLiteral("real");
//}

QString SQLiteSchemaGrammar::typeDecimal(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("numeric");
}

QString SQLiteSchemaGrammar::typeBoolean(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("tinyint(1)");
}

QString SQLiteSchemaGrammar::typeEnum(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral(R"(varchar check ("%1" in (%2)))")
            .arg(column.name, quoteString(column.allowed));
}

QString SQLiteSchemaGrammar::typeJson(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return text_;
}

QString SQLiteSchemaGrammar::typeJsonb(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return text_;
}

/* Note:
   SQLite does not have a storage class set aside for storing dates and/or times.
   Instead, the built-in Date And Time Functions of SQLite are capable of storing
   dates and times as TEXT, REAL, or INTEGER values.
   https://www.sqlite.org/datatype3.html. */

QString SQLiteSchemaGrammar::typeDate(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("date");
}

QString SQLiteSchemaGrammar::typeDateTime(ColumnDefinition &column) const
{
    return typeTimestamp(column);
}

QString SQLiteSchemaGrammar::typeDateTimeTz(ColumnDefinition &column) const
{
    return typeDateTime(column);
}

QString SQLiteSchemaGrammar::typeTime(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("time");
}

QString SQLiteSchemaGrammar::typeTimeTz(const ColumnDefinition &column) const
{
    return typeTime(column);
}

QString SQLiteSchemaGrammar::typeTimestamp(ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    if (column.useCurrent)
        column.defaultValue = Expression(QStringLiteral("current_timestamp"));

    return QStringLiteral("datetime");
}

QString SQLiteSchemaGrammar::typeTimestampTz(ColumnDefinition &column) const
{
    return typeTimestamp(column);
}

QString SQLiteSchemaGrammar::typeYear(const ColumnDefinition &column) const
{
    return typeInteger(column);
}

QString SQLiteSchemaGrammar::typeTinyBinary(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return blob_;
}

QString SQLiteSchemaGrammar::typeBinary(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return blob_;
}

QString SQLiteSchemaGrammar::typeMediumBinary(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return blob_;
}

QString SQLiteSchemaGrammar::typeLongBinary(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return blob_;
}

QString SQLiteSchemaGrammar::typeUuid(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return varchar_;
}

QString SQLiteSchemaGrammar::typeIpAddress(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return varchar_;
}

QString SQLiteSchemaGrammar::typeMacAddress(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return varchar_;
}

QString SQLiteSchemaGrammar::typeGeometry(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("geometry");
}

QString SQLiteSchemaGrammar::typePoint(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("point");
}

QString SQLiteSchemaGrammar::typeLineString(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("linestring");
}

QString SQLiteSchemaGrammar::typePolygon(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("polygon");
}

QString
SQLiteSchemaGrammar::typeGeometryCollection(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("geometrycollection");
}

QString SQLiteSchemaGrammar::typeMultiPoint(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("multipoint");
}

QString SQLiteSchemaGrammar::typeMultiLineString(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("multilinestring");
}

QString SQLiteSchemaGrammar::typeMultiPolygon(const ColumnDefinition &/*unused*/) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("multipolygon");
}

//QString SQLiteSchemaGrammar::typeMultiPolygonZ(const ColumnDefinition &column) const
//{
//    return QStringLiteral("");
//}

QString SQLiteSchemaGrammar::typeComputed(const ColumnDefinition &/*unused*/) const
{
    throw Exceptions::RuntimeError(
                "This database driver requires a type, see the virtualAs / storedAs "
                "modifiers.");
}

QString SQLiteSchemaGrammar::modifyVirtualAs(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    // FEATURE schema json silverqx
    if (!column.virtualAs || column.virtualAs->isEmpty())
        return {};

    return QStringLiteral(" generated always as (%1)").arg(*column.virtualAs);
}

QString SQLiteSchemaGrammar::modifyStoredAs(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    // FEATURE schema json silverqx
    if (!column.storedAs || column.storedAs->isEmpty())
        return {};

    return QStringLiteral(" generated always as (%1) stored").arg(*column.storedAs);
}

QString SQLiteSchemaGrammar::modifyNullable(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    /* SQLite doesn't need any special logic for generated columns (virtualAs and
       storedAs). Also, for the nullable columns return "" and not " null" because
       SQLite documentation tells nothing about NULL during a column creation or addition,
       it only describes the NOT NULL constraint. I have checked it and it also works
       correctly with the NULL, but it can be ignored behind the scene because the NULL
       is the default behavior. */
    return column.nullable && *column.nullable ? QString("")
                                               : QStringLiteral(" not null");
}

QString SQLiteSchemaGrammar::modifyDefault(const ColumnDefinition &column) const
{
    const auto &defaultValue = column.defaultValue;

    /* From SQLite docs:
       Generated columns may not have a default value (they may not use the "DEFAULT"
       clause). The value of a generated column is always the value specified by
       the expression that follows the "AS" keyword. */
    if (!defaultValue.isValid() || defaultValue.isNull() ||
        (column.virtualAs && !column.virtualAs->isEmpty()) ||
        (column.storedAs  && !column.storedAs->isEmpty())
    )
        return {};

    // Default value is already quoted and escaped inside the getDefaultValue()
    return QStringLiteral(" default %1").arg(getDefaultValue(defaultValue));
}

QString SQLiteSchemaGrammar::modifyIncrement(const ColumnDefinition &column) const // NOLINT(readability-convert-member-functions-to-static)
{
    static const std::unordered_set serials {
        ColumnType::BigInteger,   ColumnType::Integer,     ColumnType::MediumInteger,
        ColumnType::SmallInteger, ColumnType::TinyInteger,
    };

    if (serials.contains(column.type) && column.autoIncrement)
        return QStringLiteral(" primary key autoincrement");

    return {};
}

} // namespace Orm::SchemaNs::Grammars

TINYORM_END_COMMON_NAMESPACE
