#include "orm/schema/grammars/schemagrammar.hpp"

#include "orm/databaseconnection.hpp"
#include "orm/exceptions/invalidargumenterror.hpp"
#include "orm/schema/blueprint.hpp"

#ifdef TINYORM_USING_TINYDRIVERS
#  include "orm/exceptions/runtimeerror.hpp"
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs::Grammars
{

/* public */

/* Compile methods for the SchemaBuilder */

// CUR schema sqlite, check all this NotImplemented and remove which are not used silverqx
QString SchemaGrammar::compileCreateDatabase(const QString &/*unused*/,
                                             DatabaseConnection &connection) const
{
    throw Exceptions::LogicError(
                QStringLiteral("%1 database driver does not support creating databases.")
                .arg(connection.driverName()));
}

QString SchemaGrammar::compileDropDatabaseIfExists(const QString &/*unused*/) const
{
    throw Exceptions::LogicError(
                "This database driver does not support dropping databases.");
}

QString SchemaGrammar::compileDropAllTables(const QList<QString> &/*unused*/) const
{
    throw Exceptions::RuntimeError(NotImplemented);
}

QString SchemaGrammar::compileDropAllViews(const QList<QString> &/*unused*/) const
{
    throw Exceptions::RuntimeError(NotImplemented);
}

QString SchemaGrammar::compileGetAllTables(const QList<QString> &/*unused*/) const // NOLINT(google-default-arguments)
{
    throw Exceptions::RuntimeError(NotImplemented);
}

QString SchemaGrammar::compileGetAllViews(const QList<QString> &/*unused*/) const // NOLINT(google-default-arguments)
{
    throw Exceptions::RuntimeError(NotImplemented);
}

QString SchemaGrammar::compileTableExists() const
{
    throw Exceptions::RuntimeError(NotImplemented);
}

QList<QString>
SchemaGrammar::compileChange(const Blueprint &/*unused*/, // NOLINT(readability-convert-member-functions-to-static)
                             const BasicCommand &/*unused*/) const
{
    throw Exceptions::LogicError(
                "This database driver does not support changing columns.");
}

/* Compile methods for commands */

QList<QString>
SchemaGrammar::compileDrop(const Blueprint &blueprint,
                           const BasicCommand &/*unused*/) const
{
    return {QStringLiteral("drop table %1").arg(wrapTable(blueprint))};
}

QList<QString>
SchemaGrammar::compileDropIfExists(const Blueprint &blueprint,
                                   const BasicCommand &/*unused*/) const
{
    return {QStringLiteral("drop table if exists %1").arg(wrapTable(blueprint))};
}

QList<QString>
SchemaGrammar::compileFullText(const Blueprint &/*unused*/,
                               const IndexCommand &/*unused*/) const
{
    // CUR schema, check runtime vs logic exception silverqx
    // CUR schema, add driver and maybe connection names to this type of exceptions silverqx
    throw Exceptions::RuntimeError(NotImplemented);
}

QList<QString>
SchemaGrammar::compileForeign(const Blueprint &blueprint,
                              const ForeignKeyCommand &command) const
{
    /* We need to prepare several of the elements of the foreign key definition
       before we can create the SQL, such as wrapping the tables and convert
       an array of columns to comma-delimited strings for the SQL queries. */
    auto sql = QStringLiteral("alter table %1 add constraint %2 ")
               .arg(wrapTable(blueprint), BaseGrammar::wrap(command.index));

    /* Once we have the initial portion of the SQL statement we will add on the
       key name, table name, and referenced columns. These will complete the
       main portion of the SQL statement and this SQL will almost be done. */
    sql += QStringLiteral("foreign key (%1) references %2 (%3)")
           .arg(columnize(command.columns),
                BaseGrammar::wrapTable(command.on),
                columnize(command.references));

    /* Once we have the basic foreign key creation statement constructed we can
       build out the syntax for what should happen on an update or delete of
       the affected columns, which will get something like "cascade", etc. */
    if (!command.onDelete.isEmpty())
        sql += QStringLiteral(" on delete %1").arg(command.onDelete);

    if (!command.onUpdate.isEmpty())
        sql += QStringLiteral(" on update %1").arg(command.onUpdate);

    return {sql};
}

QList<QString>
SchemaGrammar::compileDropFullText(const Blueprint &/*unused*/,
                                   const IndexCommand &/*unused*/) const
{
    // CUR schema sqlite, add getConnection() to Blueprint and print exception like in compileCreateDatabase() silverqx
    throw Exceptions::RuntimeError(NotImplemented);
}

QList<QString>
SchemaGrammar::compileTableComment(const Blueprint &/*unused*/,
                                   const TableCommentCommand &/*unused*/) const
{
    throw Exceptions::RuntimeError(NotImplemented);
}

/* Others */

QString SchemaGrammar::wrap(const ColumnDefinition &column, bool prefixAlias) const
{
    return BaseGrammar::wrap(column.name, prefixAlias);
}

QString SchemaGrammar::wrapTable(const Blueprint &blueprint) const
{
    return BaseGrammar::wrapTable(blueprint.getTable());
}

/* protected */


bool SchemaGrammar::shouldAddAutoIncrementStartingValue(
        const ColumnDefinition &column) noexcept
{
    return column.autoIncrement && (column.startingValue || column.from);
}

QStringList SchemaGrammar::getColumns(const Blueprint &blueprint) const
{
    auto addedColumns = blueprint.getAddedColumns();

    QStringList columns;
    columns.reserve(addedColumns.size());

    for (auto &&column : addedColumns) {
        /* Each of the column types have their own compiler functions which are tasked
           with turning the column definition into its SQL format for this platform
           used by the connection. The column's modifiers are compiled and added. */
        auto sql = SPACE_IN.arg(wrap(column), getType(column));

        columns << addModifiers(std::move(sql), column);
    }

    return columns;
}

QString SchemaGrammar::getDefaultValue(const QVariant &value) const
{
    if (isExpression(value))
        return getValue(value).value<QString>();

    if (!value.canConvert<QString>())
        throw Exceptions::InvalidArgumentError(
                "The default value has to be convertible to the QString.");

    // Send boolean values as '0'/'1'
    return value.typeId() == QMetaType::Bool
            ? quoteString(QString::number(value.value<int>()))
            : quoteString(escapeString(value.value<QString>()));
}

QString SchemaGrammar::typeComputed(const ColumnDefinition &/*unused*/) const
{
    throw Exceptions::RuntimeError(
                "This database driver does not support the computed type.");
}

} // namespace Orm::SchemaNs::Grammars

TINYORM_END_COMMON_NAMESPACE
