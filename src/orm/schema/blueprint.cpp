#include "orm/schema/blueprint.hpp"

#include <range/v3/action/remove_if.hpp>
#include <range/v3/algorithm/contains.hpp>
#include <range/v3/view/filter.hpp>

#include "orm/databaseconnection.hpp"
#include "orm/macros/likely.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using ContainerUtils = Orm::Utils::Container;

namespace Orm::SchemaNs
{

/* public */

int Blueprint::DefaultStringLength = SchemaNs::DefaultStringLength;

Blueprint::Blueprint(
            QString table, const std::function<void(Blueprint &)> &callback,
            QString &&prefix
)
    : m_table(std::move(table))
    , m_prefix(std::move(prefix))
{
    /* Pretty sure that there will be at least 2 columns and 1-2 commands, so reserve
       some memory to avoid resizing. */
    m_columns.reserve(4);

    if (callback)
        std::invoke(callback, *this);
}

void Blueprint::build(DatabaseConnection &connection, const SchemaGrammar &grammar)
{
    // TODO clazy, old clazy check range-loop, remove after ugprade to newer clazy 1.11, it was divided to two checks in clazy 1.11 silverqx
    for (const auto &queryString : toSql(connection, grammar)) // clazy:exclude=range-loop,range-loop-detach
        /* All compile methods in the SchemaBuilders are unprepared, eg. the PostgreSQL
           driver even doesn't allow to send DDL commands as prepared statements,
           MySQL driver supports to send DDL commands as prepared statements. */
        connection.unprepared(queryString);
}

QList<QString> Blueprint::toSql(const DatabaseConnection &connection,
                                const SchemaGrammar &grammar)
{
    addImpliedCommands(grammar);

    // FUTURE schema, sqlite silverqx
    /* Each type of command has a corresponding compiler function on the schema
       grammar which is used to build the necessary SQL statements to build
       the blueprint element, so we'll just call that compilers function. */
//    ensureCommandsAreValid(connection);

    QList<QString> statements;
    // Reserve * 2 might be enough, can't be predicted :/
    statements.reserve(
                static_cast<decltype (statements)::size_type>(m_commands.size()) * 2);

    // CUR schema sqlite, divide to 2 call, hasCompileMethod and then invoke, get rid of sql.isEmpty() silverqx
    for (const auto &command : m_commands)
        if (auto sql = grammar.invokeCompileMethod(*command, connection, *this);
            !sql.isEmpty()
        )
            statements << std::move(sql);

    return statements;
}

const BasicCommand &Blueprint::create()
{
    // First {} is for the base class
    return addCommand({{}, Create});
}

void Blueprint::after(const QString &column,
                      const std::function<void(Blueprint &)> &callback)
{
    m_after = column;

    std::invoke(callback, *this);

    m_after.clear();
}

const BasicCommand &Blueprint::drop()
{
    return addCommand({{}, Drop});
}

const BasicCommand &Blueprint::dropIfExists()
{
    return addCommand({{}, DropIfExists});
}

const DropColumnsCommand &Blueprint::dropColumns(const QList<QString> &columns)
{
    return addCommand<DropColumnsCommand>(
                {{}, DropColumn, QList<Column>(columns.cbegin(), columns.cend())});
}

const DropColumnsCommand &Blueprint::dropColumn(const QString &column)
{
    return addCommand<DropColumnsCommand>({{}, DropColumn, {column}});
}

void Blueprint::dropTimestamps()
{
    dropColumns({CREATED_AT, UPDATED_AT});
}

const RenameCommand &Blueprint::rename(const QString &to)
{
    return addCommand<RenameCommand>({{}, Rename, {}, to});
}

const RenameCommand &Blueprint::renameColumn(const QString &from, const QString &to)
{
    return addCommand<RenameCommand>({{}, RenameColumn, from, to});
}

IndexDefinitionReference
Blueprint::primary(const QList<QString> &columns, const QString &indexName,
                   const QString &algorithm)
{
    return indexCommand(Primary, columns, indexName, algorithm);
}

IndexDefinitionReference
Blueprint::unique(const QList<QString> &columns, const QString &indexName,
                  const QString &algorithm)
{
    return indexCommand(Unique, columns, indexName, algorithm);
}

IndexDefinitionReference
Blueprint::index(const QList<QString> &columns, const QString &indexName,
                 const QString &algorithm)
{
    return indexCommand(Index, columns, indexName, algorithm); // NOLINT(readability-suspicious-call-argument)
}

IndexDefinitionReference
Blueprint::fullText(const QList<QString> &columns, const QString &indexName,
                    const QString &algorithm, const QString &language)
{
    return indexCommand(Fulltext, columns, indexName, algorithm, language);
}

// TODO schema, it looks like spatial index can not be created on multiple columns on mysql, if its true remove QList overloads, error 1070 Too many key parts specified; max 1 parts allowed silverqx
IndexDefinitionReference
Blueprint::spatialIndex(const QList<QString> &columns, const QString &indexName)
{
    return indexCommand(SpatialIndex, columns, indexName);
}

IndexDefinitionReference
Blueprint::rawIndex(const Expression &expression, const QString &indexName)
{
    return addCommand<IndexCommand>({{}, Index, indexName, {expression}});
}

ForeignKeyDefinitionReference
Blueprint::foreign(const QList<QString> &columns, const QString &indexName)
{
    return addCommand<ForeignKeyCommand>(
                {{}, Foreign,
                 indexName.isEmpty() ? createIndexName(Foreign, columns)
                                     : indexName,
                 QList<Column>(columns.cbegin(), columns.cend())});

}

const IndexCommand &Blueprint::dropPrimary(const QList<QString> &columns)
{
    return dropIndexCommand(DropPrimary, Primary, columns);
}

const IndexCommand &Blueprint::dropUnique(const QList<QString> &columns)
{
    return dropIndexCommand(DropUnique, Unique, columns);
}

const IndexCommand &Blueprint::dropIndex(const QList<QString> &columns)
{
    return dropIndexCommand(DropIndex, Index, columns);
}

const IndexCommand &Blueprint::dropFullText(const QList<QString> &columns)
{
    return dropIndexCommand(DropFullText, Fulltext, columns);
}

const IndexCommand &Blueprint::dropSpatialIndex(const QList<QString> &columns)
{
    return dropIndexCommand(DropSpatialIndex, SpatialIndex, columns);
}

const IndexCommand &Blueprint::dropForeign(const QList<QString> &columns)
{
    return dropIndexCommand(DropForeign, Foreign, columns);
}

const DropColumnsCommand &Blueprint::dropConstrainedForeignId(const QString &column)
{
    dropForeign(QList<QString> {column});

    return dropColumn(column);
}

const RenameCommand &Blueprint::renameIndex(const QString &from, const QString &to)
{
    return addCommand<RenameCommand>({{}, RenameIndex, from, to});
}

ForeignIdColumnDefinitionReference Blueprint::foreignId(const QString &column)
{
    return {*this, unsignedBigInteger(column)};
}

ForeignIdColumnDefinitionReference Blueprint::foreignUuid(const QString &column)
{
    return {*this, uuid(column)};
}

ColumnDefinitionReference<>
Blueprint::integer(const QString &column, const bool autoIncrement,
                   const bool isUnsigned)
{
    return addColumn(ColumnType::Integer, column, {.autoIncrement = autoIncrement,
                                                   .isUnsigned    = isUnsigned});
}

ColumnDefinitionReference<>
Blueprint::tinyInteger(const QString &column, const bool autoIncrement,
                       const bool isUnsigned)
{
    return addColumn(ColumnType::TinyInteger, column, {.autoIncrement = autoIncrement,
                                                       .isUnsigned    = isUnsigned});
}

ColumnDefinitionReference<>
Blueprint::smallInteger(const QString &column, const bool autoIncrement,
                        const bool isUnsigned)
{
    return addColumn(ColumnType::SmallInteger, column, {.autoIncrement = autoIncrement,
                                                        .isUnsigned    = isUnsigned});
}

ColumnDefinitionReference<>
Blueprint::mediumInteger(const QString &column, const bool autoIncrement,
                         const bool isUnsigned)
{
    return addColumn(ColumnType::MediumInteger, column, {.autoIncrement = autoIncrement,
                                                         .isUnsigned    = isUnsigned});
}

ColumnDefinitionReference<>
Blueprint::bigInteger(const QString &column, const bool autoIncrement,
                      const bool isUnsigned)
{
    return addColumn(ColumnType::BigInteger, column, {.autoIncrement = autoIncrement,
                                                      .isUnsigned    = isUnsigned});
}

ColumnDefinitionReference<> Blueprint::Char(const QString &column, const int length)
{
    return addColumn(ColumnType::Char, column, {.length = length});
}

ColumnDefinitionReference<> Blueprint::string(const QString &column, const int length)
{
    return addColumn(ColumnType::String, column, {.length = length});
}

ColumnDefinitionReference<> Blueprint::tinyText(const QString &column)
{
    return addColumn(ColumnType::TinyText, column);
}

ColumnDefinitionReference<> Blueprint::text(const QString &column)
{
    return addColumn(ColumnType::Text, column);
}

ColumnDefinitionReference<> Blueprint::mediumText(const QString &column)
{
    return addColumn(ColumnType::MediumText, column);
}

ColumnDefinitionReference<> Blueprint::longText(const QString &column)
{
    return addColumn(ColumnType::LongText, column);
}

// TODO schema, fix and unify real/double/float types with latest standard and remove deprecated APIs, what I have understood from the MySQL and PostgreSQL docs it should be like this; I should have real and double (double precision) types without any total, places or precision params and then I should have float type that should have precision param, currently this total and places params are deprecated as described in https://dev.mysql.com/doc/refman/9.0/en/floating-point-types.html and will be removed in future, also add this column types to tests and to the playground also check numeric/decimal type, its different animal silverqx
ColumnDefinitionReference<>
Blueprint::Float(const QString &column, const std::optional<int> total,
                 const std::optional<int> places, const bool isUnsigned)
{
    return addColumn(ColumnType::Float, column,
                     {.places = places, .total = total, .isUnsigned = isUnsigned});
}

ColumnDefinitionReference<>
Blueprint::Double(const QString &column, const std::optional<int> total,
                  const std::optional<int> places, const bool isUnsigned)
{
    return addColumn(ColumnType::Double, column,
                     {.places = places, .total = total, .isUnsigned = isUnsigned});
}

ColumnDefinitionReference<>
Blueprint::decimal(const QString &column, const std::optional<int> total,
                   const std::optional<int> places, const bool isUnsigned)
{
    return addColumn(ColumnType::Decimal, column,
                     {.places = places, .total = total, .isUnsigned = isUnsigned});
}

ColumnDefinitionReference<> Blueprint::boolean(const QString &column)
{
    return addColumn(ColumnType::Boolean, column);
}

ColumnDefinitionReference<>
Blueprint::Enum(const QString &column, const QList<QString> &allowed)
{
    return addColumn(ColumnType::Enum, column, {.allowed = allowed});
}

ColumnDefinitionReference<>
Blueprint::set(const QString &column, const QList<QString> &allowed)
{
    return addColumn(ColumnType::Set, column, {.allowed = allowed});
}

ColumnDefinitionReference<> Blueprint::json(const QString &column)
{
    return addColumn(ColumnType::Json, column);
}

ColumnDefinitionReference<> Blueprint::jsonb(const QString &column)
{
    return addColumn(ColumnType::Jsonb, column);
}

ColumnDefinitionReference<> Blueprint::date(const QString &column)
{
    return addColumn(ColumnType::Date, column);
}

ColumnDefinitionReference<>
Blueprint::datetime(const QString &column, const std::optional<int> precision)
{
    return addColumn(ColumnType::DateTime, column, {.precision = precision});
}

ColumnDefinitionReference<>
Blueprint::datetimeTz(const QString &column, const std::optional<int> precision)
{
    return addColumn(ColumnType::DateTimeTz, column, {.precision = precision});
}

ColumnDefinitionReference<>
Blueprint::time(const QString &column, const std::optional<int> precision)
{
    return addColumn(ColumnType::Time, column, {.precision = precision});
}

ColumnDefinitionReference<>
Blueprint::timeTz(const QString &column, const std::optional<int> precision)
{
    return addColumn(ColumnType::TimeTz, column, {.precision = precision});
}

ColumnDefinitionReference<>
Blueprint::timestamp(const QString &column, const std::optional<int> precision)
{
    return addColumn(ColumnType::Timestamp, column, {.precision = precision});
}

ColumnDefinitionReference<>
Blueprint::timestampTz(const QString &column, const std::optional<int> precision)
{
    return addColumn(ColumnType::TimestampTz, column, {.precision = precision});
}

void Blueprint::timestamps(const std::optional<int> precision)
{
    timestamp(CREATED_AT, precision).nullable();

    timestamp(UPDATED_AT, precision).nullable();
}

void Blueprint::timestampsTz(const std::optional<int> precision)
{
    timestampTz(CREATED_AT, precision).nullable();

    timestampTz(UPDATED_AT, precision).nullable();
}

ColumnDefinitionReference<>
Blueprint::softDeletes(const QString &column, const std::optional<int> precision)
{
    return timestamp(column, precision).nullable();
}

ColumnDefinitionReference<>
Blueprint::softDeletesTz(const QString &column, const std::optional<int> precision)
{
    return timestampTz(column, precision).nullable();
}

ColumnDefinitionReference<> Blueprint::year(const QString &column)
{
    return addColumn(ColumnType::Year, column);
}

ColumnDefinitionReference<> Blueprint::tinyBinary(const QString &column)
{
    return addColumn(ColumnType::TinyBinary, column);
}

ColumnDefinitionReference<> Blueprint::binary(const QString &column)
{
    return addColumn(ColumnType::Binary, column);
}

ColumnDefinitionReference<> Blueprint::mediumBinary(const QString &column)
{
    return addColumn(ColumnType::MediumBinary, column);
}

ColumnDefinitionReference<> Blueprint::longBinary(const QString &column)
{
    return addColumn(ColumnType::LongBinary, column);
}

ColumnDefinitionReference<> Blueprint::uuid(const QString &column)
{
    return addColumn(ColumnType::Uuid, column);
}

ColumnDefinitionReference<> Blueprint::ipAddress(const QString &column)
{
    return addColumn(ColumnType::IpAddress, column);
}

ColumnDefinitionReference<> Blueprint::macAddress(const QString &column)
{
    return addColumn(ColumnType::MacAddress, column);
}

ColumnDefinitionReference<> Blueprint::geometry(const QString &column)
{
    return addColumn(ColumnType::Geometry, column);
}

ColumnDefinitionReference<>
Blueprint::point(const QString &column, const std::optional<quint32> srid)
{
    return addColumn(ColumnType::Point, column, {.srid = srid});
}

ColumnDefinitionReference<> Blueprint::lineString(const QString &column)
{
    return addColumn(ColumnType::LineString, column);
}

ColumnDefinitionReference<> Blueprint::polygon(const QString &column)
{
    return addColumn(ColumnType::Polygon, column);
}

ColumnDefinitionReference<> Blueprint::geometryCollection(const QString &column)
{
    return addColumn(ColumnType::GeometryCollection, column);
}

ColumnDefinitionReference<> Blueprint::multiPoint(const QString &column)
{
    return addColumn(ColumnType::MultiPoint, column);
}

ColumnDefinitionReference<> Blueprint::multiLineString(const QString &column)
{
    return addColumn(ColumnType::MultiLineString, column);
}

ColumnDefinitionReference<> Blueprint::multiPolygon(const QString &column)
{
    return addColumn(ColumnType::MultiPolygon, column);
}

ColumnDefinitionReference<> Blueprint::multiPolygonZ(const QString &column)
{
    return addColumn(ColumnType::MultiPolygonZ, column);
}

// TEST schema, test computed column silverqx
ColumnDefinitionReference<>
Blueprint::computed(const QString &column, const QString &expression)
{
    return addColumn(ColumnType::Computed, column, {.expression = expression});
}

ColumnDefinitionReference<> Blueprint::rememberToken()
{
    return string(QStringLiteral("remember_token"), 100).nullable();
}

const TableCommentCommand &Blueprint::comment(QString comment)
{
    return addCommand<TableCommentCommand>({{}, TableComment, std::move(comment)});
}

ColumnDefinitionReference<>
Blueprint::addColumn(const ColumnType type, const QString &name,
                     ColumnDefinition &&definition)
{
    definition.type = type;
    definition.name = name;

    return addColumnDefinition(std::move(definition));
}

QList<ColumnDefinition> Blueprint::getAddedColumns() const
{
    QList<ColumnDefinition> added;
    added.reserve(m_columns.size());

    std::ranges::copy_if(m_columns, std::back_inserter(added),
                         [](const auto &column)
    {
        return !column.change;
    });

    return added;
}

QList<ColumnDefinition> Blueprint::getChangedColumns() const
{
    QList<ColumnDefinition> changed;
    changed.reserve(m_columns.size());

    std::ranges::copy_if(m_columns, std::back_inserter(changed),
                         [](const auto &column)
    {
        return column.change;
    });

    return changed;
}

bool Blueprint::creating() const
{
    return ranges::contains(m_commands, Create, [](const auto &command)
    {
        /* We can be at 100% sure that all commands will be convertible at least
           to the BasicCommand, because the basic requirement is that every command must
           have a name. */
        return reinterpret_cast<const BasicCommand &>(*command).name; // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    });
}

Blueprint &Blueprint::removeColumn(const QString &name)
{
    m_columns |= ranges::actions::remove_if([&name](const auto &column)
    {
        return column.name == name;
    });

    return *this;
}

void Blueprint::defaultStringLength(const int length) noexcept
{
    DefaultStringLength = length;
}

/* protected */

ColumnDefinitionReference<> Blueprint::addColumnDefinition(ColumnDefinition &&definition)
{
    m_columns << std::move(definition);

    auto &definitionRef = m_columns.last(); // clazy:exclude=detaching-member

    if (!m_after.isEmpty()) {
        definitionRef.after.swap(m_after);

        m_after = definitionRef.name;
    }

    return definitionRef;
}

void Blueprint::addImpliedCommands(const SchemaGrammar &grammar)
{
    if (!getAddedColumns().isEmpty() && !creating())
        m_commands.emplace_front(createCommand<BasicCommand>({{}, Add}));

    if (!getChangedColumns().isEmpty() && !creating())
        m_commands.emplace_front(createCommand<BasicCommand>({{}, Change}));

    addFluentIndexes();

    addFluentCommands(grammar);
}

void Blueprint::addFluentIndexes()
{
    /*! Index item for fluent indexes. */
    struct FluentIndexItem
    {
        /*! Index name for the database. */
        std::reference_wrapper<std::variant<std::monostate, QString, bool>> name;
        /*! Index type in the string format. */
        QString type;
    };

    for (auto &column : m_columns)
        /* Loop over all column's index data members to check if any index has been
           defined. */
        for (const std::array indexes {
                 std::to_array<FluentIndexItem>({
                     {std::ref(column.index),        Index},
                     {std::ref(column.primary),      Primary},
                     {std::ref(column.fulltext),     Fulltext},
                     {std::ref(column.spatialIndex), SpatialIndex},
                     {std::ref(column.unique),       Unique}
                 })
             };
             const auto &indexItem : indexes
        ) {
            auto &index = indexItem.name.get();

            if (std::holds_alternative<std::monostate>(index))
                continue;

            /* If the index has been specified on the given column, but is simply equal
               to "true" (boolean), no name has been specified for this index so the
               index method can be called without a name and it will generate one. */
            if (std::holds_alternative<bool>(index) && std::get<bool>(index)) {
                indexCommand(indexItem.type, {column.name}, "");

                index = false;

                break;
            }

            /* If the index has been specified on the given column, and it has a string
               value, we'll go ahead and call the index method and pass the name for
               the index since the developer specified the explicit name for this. */
            if (std::holds_alternative<QString>(index))
                if (const auto &indexName = std::get<QString>(index);
                    !indexName.isEmpty()
                ) {
                    indexCommand(indexItem.type, {column.name}, indexName);

                    index = false;

                    break;
                }

            /* Here still can be case where:
               std::holds_alternative<bool>(index) && std::get<bool>(index) == false,
               Because of that the previous block still has to be wrapped in the
               if (std::holds_alternative<QString>(index)). */
        }
}

void Blueprint::addFluentCommands(const SchemaGrammar &grammar)
{
    /* Column auto-increment starting value (MySQL/PostgreSQL) and
       a column comment (PostgreSQL) are all fluent commands. */

    // Get the fluent commands for the grammar
    for (const auto &[commandName, shouldAdd] : grammar.getFluentCommands())
        /* Loop over all columns and check if a new fluent command should be added
           (check if command values are set and are not empty). */
        for (const auto &column : std::as_const(m_columns))

            // AutoIncrementStartingValue command (MySQL/PostgreSQL)
            if (commandName == AutoIncrementStartingValue &&
                /* The column.startingValue || column.from is checked
                   in the SchemaGrammar::shouldAddAutoIncrementStartingValue(). */
                std::invoke(shouldAdd, column)
            ) T_LIKELY
                addCommand<AutoIncrementStartingValueCommand>(
                        {{}, commandName, column.name,
                         column.startingValue ? *column.startingValue
                                              // Alias for the 'startingValue'
                                              : *column.from}); // NOLINT(bugprone-unchecked-optional-access)

            // Comment command (PostgreSQL)
            else if (commandName == Comment && std::invoke(shouldAdd, column)) T_UNLIKELY
                addCommand<CommentCommand>(
                        {{}, commandName, column.name, column.comment, column.change});
}

IndexDefinitionReference
Blueprint::indexCommand(const QString &type, const QList<QString> &columns,
                        const QString &indexName, const QString &algorithm,
                        const QString &language)
{
    /* If no name was specified for this index, we will create one using a basic
       convention of the table name, followed by the columns, followed by an
       index type, such as primary or index, which makes the index unique,
       eg. posts_user_id_foreign or users_name_unique. */
    return addCommand<IndexCommand>(
                {{}, type,
                 indexName.isEmpty() ? createIndexName(type, columns)
                                     : indexName,
                 QList<Column>(columns.cbegin(), columns.cend()),
                 algorithm, language});
}

const IndexCommand &
Blueprint::dropIndexCommand(const QString &command, const QString &type,
                            const QList<QString> &columns)
{
    /* Although it looks weird so these two methods works, I tested them, believe me,
       the dropPrimary("") drops the primary key on both MySQL and PostgreSQL. */

    // Used by dropPrimary("")
    if (columns.isEmpty())
        return indexCommand(command, {}, {});

    /* If the given "index" is actually the vector of columns, the developer means
       to drop an index merely by specifying the columns involved without the
       conventional name, so we will build the index name from the columns. */
    return dropIndexCommand(command, createIndexName(type, columns));
}

// NOTE api different, Eloquent doesn't have overload like this silverqx
const IndexCommand &
Blueprint::dropIndexCommand(const QString &command, const QString &indexName)
{
    return indexCommand(command, {}, indexName);
}

QString
Blueprint::createIndexName(const QString &type, const QList<QString> &columns) const
{
    auto index = QStringLiteral("%1_%2_%3")
                 .arg(NOSPACE.arg(m_prefix, m_table),
                      ContainerUtils::join(columns, UNDERSCORE),
                      type)
                 .toLower();

    return index.replace(DASH, UNDERSCORE).replace(DOT, UNDERSCORE);
}

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE
