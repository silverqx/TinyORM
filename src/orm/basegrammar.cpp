#include "orm/basegrammar.hpp"

#include "orm/runtimeerror.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

/*
   wrap methods are only for column names, table names and identifiers.
   parameter()/parametrize() methods are for values.
   columnize() is used for column names containers (constrained by ColumnContainer
   concept) and it calls wrapArray() internally.
   Values or columns/tables/identifiers can also be the Query::Expression.
   The Query::Expression is always converted to the QString and appended to the query.
   quoteString() can be used to quote string literals, it is not used anywhere for now.
*/

const QString &BaseGrammar::getDateFormat() const
{
    static const auto cachedFormat = QStringLiteral("yyyy-MM-dd HH:mm:ss");

    return cachedFormat;
}

QString BaseGrammar::wrap(const QString &value, const bool prefixAlias) const
{
    /* If the value being wrapped has a column alias we will need to separate out
       the pieces so we can wrap each of the segments of the expression on its
       own, and then join these both back together using the "as" connector. */
    if (value.contains(" as "))
        return wrapAliasedValue(value, prefixAlias);

    // FEATURE json columns, this code has to be in the Grammars::Grammar silverqx
    /* If the given value is a JSON selector we will wrap it differently than a
       traditional value. We will need to split this path and wrap each part
       wrapped, etc. Otherwise, we will simply wrap the value as a string. */
//    if (isJsonSelector(value))
//        return wrapJsonSelector(value);

    return wrapSegments(value.split(QChar('.')));
}

// CUR finish commented, also add comment when is which overload used and how it works silverqx
//QString BaseGrammar::wrap(const Expression &value, const bool) const
//{
//    return getValue(value).value<QString>();
//}

//QString BaseGrammar::wrap(const QVariant &value) const
//{
//    return isExpression(value) ? getValue(value).value<QString>()
//                               : wrap(value.value<QString>());
//}

QString BaseGrammar::wrap(const Column &value) const
{
    return std::holds_alternative<Expression>(value)
            ? getValue(std::get<Expression>(value)).value<QString>()
            : wrap(std::get<QString>(value));
}

QString BaseGrammar::wrapTable(const QString &table) const
{
    return wrap(QStringLiteral("%1%2").arg(m_tablePrefix, table), true);
}

QString BaseGrammar::wrapTable(const FromClause &table) const
{
    if (std::holds_alternative<std::monostate>(table))
        throw RuntimeError("std::monostate in wrapTable().");
    else if (std::holds_alternative<Expression>(table))
        return getValue(std::get<Expression>(table)).value<QString>();

    return wrapTable(std::get<QString>(table));
}

QString BaseGrammar::quoteString(const QString &value) const
{
    return QStringLiteral("'%1'").arg(value);
}

//QString BaseGrammar::wrapTable(const Expression &table) const
//{
//    return getValue(table).value<QString>();
//}

// CUR remove all expressions todos when I finish expressions for columns and add comment why I have different overloads for columns and values, described below silverqx
// FEATURE expressions, this api (method overloads) which takes Expression as parameter looks strange and inconsistent because wrap() takes QString/Expression, wrapTable() the same, instead getValue() takes QVariant/Expression and parameter takes QVariant only, this is a consequence of that columns are always passed as QString (Expression overload are not never called), but values are passed as QVariant and CAN CONTAIN QVariant(Expression), so investigate in the future and it is also related to the another feature todo, which propose to add Expression overloads for methods in the query builder silverqx
bool BaseGrammar::isExpression(const QVariant &value) const
{
    return value.canConvert<Expression>();
}

QVariant BaseGrammar::getValue(const QVariant &expression) const
{
    return expression.value<Expression>().getValue();
}

QVariant BaseGrammar::getValue(const Expression &expression) const
{
    return expression.getValue();
}

BaseGrammar &BaseGrammar::setTablePrefix(const QString &prefix)
{
    m_tablePrefix = prefix;

    return *this;
}

QString BaseGrammar::parameter(const QVariant &value) const
{
    // FEATURE expressions, how to work with them and pass them to the query builder 🤔 silverqx
    return isExpression(value) ? getValue(value).value<QString>()
                               : QStringLiteral("?");
}

QString BaseGrammar::wrapAliasedValue(const QString &value, const bool prefixAlias) const
{
    auto segments = getSegmentsFromFrom(value);

    /* If we are wrapping a table we need to prefix the alias with the table prefix
       as well in order to generate proper syntax. If this is a column of course
       no prefix is necessary. The condition will be true when from wrapTable. */
    if (prefixAlias)
        segments[1] = QStringLiteral("%1%2").arg(m_tablePrefix, segments[1]);

    return QStringLiteral("%1 as %2").arg(wrap(segments[0]), wrapValue(segments[1]));
}

QString BaseGrammar::wrapValue(QString value) const
{
    if (value == '*')
        return value;

    return QStringLiteral("\"%1\"").arg(value.replace(QStringLiteral("\""),
                                                      QStringLiteral("\"\"")));
}

QString BaseGrammar::wrapSegments(QStringList segments) const
{
    const auto size = segments.size();
    // eg table_name.column is qualified
    const auto isQualifiedSegment = size > 1;

    for (auto i = 0; i < size; ++i)
        if (i == 0 && isQualifiedSegment)
            segments[i] = wrapTable(segments[i]);
        else
            segments[i] = wrapValue(segments[i]);

    return segments.join(QChar('.'));
}

QStringList BaseGrammar::getSegmentsFromFrom(const QString &from) const
{
    auto segments = from.split(QStringLiteral(" as "), Qt::KeepEmptyParts,
                               Qt::CaseInsensitive);

    // Remove leading/ending whitespaces
    for (auto &segemnt : segments)
        segemnt = segemnt.trimmed();

    return segments;
}

QString BaseGrammar::getFromWithoutAlias(const QString &from) const
{
    // Prevent clazy warning
    const auto segments = getSegmentsFromFrom(from);

    return segments.first();
}

QString BaseGrammar::getAliasFromFrom(const QString &from) const
{
    // Prevent clazy warning
    const auto segments = getSegmentsFromFrom(from);

    return segments.last();
}

QString BaseGrammar::unqualifyColumn(const QString &column) const
{
    return column.split(QChar('.')).last().trimmed();
}

QString BaseGrammar::columnizeInternal(const QVector<QString> &columns) const
{
    QString columnized;

    if (columns.isEmpty())
        return columnized;

    const auto end = columns.cend() - 1;
    auto it = columns.begin();

    for (; it < end; ++it)
        columnized += QStringLiteral("%1, ").arg(*it);

    if (it == end)
        columnized += *it;

    return columnized;
}

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
