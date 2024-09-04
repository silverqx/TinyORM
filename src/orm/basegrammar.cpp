#include "orm/basegrammar.hpp"

#include "orm/exceptions/runtimeerror.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

/*
   wrap methods are only for column names, table names and identifiers, they use
   primarily Column type and QString type.
   wrapValue() for one string column/table/identifier.
   wrapTable() for table, columnize() for columns.

   parameter()/parametrize() methods are for values, it replaces value/s with ? or use
   a raw expression if was passed (Query::Expression), parameter uses QVariant only and
   parametrize uses Container type and Parametrize constraint.

   columnize() is used for column names containers (constrained by ColumnContainer
   concept) and it calls wrapArray() and other wrap related methods internally,
   columnize uses ColumnContainer constraint, it converts a vector of column names
   into a wrapped comma delimited string.
   Columns/tables/identifiers can also be the Query::Expression.
   The Query::Expression is always converted to the QString and appended to the query.

   quoteString() can be used to quote string literals, it also supports containers.

   If next time you will think that some method for quoting is missing then it doesn't,
   for parameter bindings (?) use parameter()/parametrize(), for column names use
   wrap() related methods and for quoting raw strings eg. in the schema use
   the quoteString(), all these methods support container types.
*/

/* public */

const QString &BaseGrammar::getDateFormat() const
{
    static const auto cachedFormat = u"yyyy-MM-dd HH:mm:ss"_s;

    return cachedFormat;
}

const QString &BaseGrammar::getTimeFormat() const
{
    static const auto cachedFormat = u"HH:mm:ss"_s;

    return cachedFormat;
}

QString BaseGrammar::wrap(const QString &value, const bool prefixAlias) const // NOLINT(misc-no-recursion)
{
    /* If the value being wrapped has a column alias we will need to separate out
       the pieces so we can wrap each of the segments of the expression on its
       own, and then join these both back together using the "as" connector. */
    if (value.contains(u" as "_s, Qt::CaseInsensitive))
        return wrapAliasedValue(value, prefixAlias);

    // FEATURE json columns, this code has to be in the Grammars::Grammar silverqx
    /* If the given value is a JSON selector we will wrap it differently than a
       traditional value. We will need to split this path and wrap each part
       wrapped, etc. Otherwise, we will simply wrap the value as a string. */
//    if (isJsonSelector(value))
//        return wrapJsonSelector(value);

    return wrapSegments(value.split(DOT));
}

QString BaseGrammar::wrap(const Column &value) const
{
    return std::holds_alternative<Expression>(value)
            ? getValue(std::get<Expression>(value)).value<QString>()
            : wrap(std::get<QString>(value));
}

QString BaseGrammar::wrapTable(const QString &table) const // NOLINT(misc-no-recursion)
{
    return wrap(NOSPACE.arg(m_tablePrefix, table), true);
}

QString BaseGrammar::wrapTable(const FromClause &table) const
{
    if (std::holds_alternative<std::monostate>(table))
        // Not InvalidArgumentError because table argument was not passed by user
        throw Exceptions::RuntimeError(
                u"Unexpected std::monostate value in %1()."_s.arg(__tiny_func__));

    if (std::holds_alternative<Expression>(table))
        return getValue(std::get<Expression>(table)).value<QString>();

    return wrapTable(std::get<QString>(table));
}

QString BaseGrammar::quoteString(const QString &value)
{
    return TMPL_SQUOTES.arg(value);
}

bool BaseGrammar::isExpression(const QVariant &value)
{
    return value.canConvert<Expression>();
}

QVariant BaseGrammar::getValue(const QVariant &expression)
{
    return expression.value<Expression>().getValue();
}

QVariant BaseGrammar::getValue(const Expression &expression) noexcept
{
    return expression.getValue();
}

BaseGrammar &BaseGrammar::setTablePrefix(const QString &prefix)
{
    m_tablePrefix = prefix;

    return *this;
}

QString BaseGrammar::unqualifyColumn(const QString &column)
{
    const auto lastDotIndex = column.lastIndexOf(DOT);

    // Nothing to unqualify, a dot not found
    if (lastDotIndex == -1)
        return column;

    return QStringView(column.constBegin() + lastDotIndex + 1, column.constEnd())
            .trimmed().toString();
}

QString BaseGrammar::getFromWithoutAlias(const QString &from)
{
    return std::move(getSegmentsFromAlias(from).first()); // clazy:exclude=detaching-temporary
}

QString BaseGrammar::getAliasFromFrom(const QString &from)
{
    return std::move(getSegmentsFromAlias(from).last()); // clazy:exclude=detaching-temporary
}

/* protected */

QString BaseGrammar::parameter(const QVariant &value)
{
    return isExpression(value) ? getValue(value).value<QString>()
                               : QChar::fromLatin1('?');
}

QString BaseGrammar::wrapAliasedValue(const QString &value, const bool prefixAlias) const // NOLINT(misc-no-recursion)
{
    auto segments = getSegmentsFromAlias(value);

    /* If we are wrapping a table we need to prefix the alias with the table prefix
       as well in order to generate proper syntax. If this is a column of course
       no prefix is necessary. The condition will be true when from wrapTable. */
    if (prefixAlias)
        segments[1] = NOSPACE.arg(m_tablePrefix, segments[1]);

    return u"%1 as %2"_s.arg(wrap(segments.constFirst()), wrapValue(segments[1]));
}

QString BaseGrammar::wrapValue(QString value) const
{
    if (value == ASTERISK_C)
        return value;

    // Don't change to prepend()/append(), no perf. gain, I have tested it
    return TMPL_DQUOTES.arg(value.replace(QUOTE, u"\"\""_s));
}

QString BaseGrammar::wrapSegments(QStringList segments) const // NOLINT(misc-no-recursion)
{
    const auto size = segments.size();
    // eg. table_name.column is qualified
    const auto isQualifiedSegment = size > 1;

    for (auto i = 0; i < size; ++i)
        if (i == 0 && isQualifiedSegment)
            segments[i] = wrapTable(segments[i]);
        else
            segments[i] = wrapValue(segments[i]);

    return segments.join(DOT);
}

QStringList BaseGrammar::getSegmentsFromAlias(const QString &aliasedExpression)
{
    const auto segmentsView = QStringView(aliasedExpression)
                              .split(u" as "_s, Qt::KeepEmptyParts, Qt::CaseInsensitive);

    Q_ASSERT(!segmentsView.isEmpty() && segmentsView.size() <= 2);

    QStringList segments;
    segments.reserve(segmentsView.size());

    // Remove leading/ending whitespaces
    for (const auto segement : segmentsView)
        segments << segement.trimmed().toString();

    return segments;
}

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE
