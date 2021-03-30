#include "orm/basegrammar.hpp"

#include "orm/query/expression.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

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

QString BaseGrammar::wrap(const Expression &value, const bool) const
{
    return getValue(value).value<QString>();
}

QString BaseGrammar::wrapTable(const QString &table) const
{
    return wrap(m_tablePrefix + table, true);
}

QString BaseGrammar::wrapTable(const Expression &table) const
{
    return getValue(table).value<QString>();
}

// FEATURE expression, this api (method overloads) which takes Expression as parameter looks strange and inconsistent because wrap() takes QString/Expression, wrapTable() the same, instead getValue() takes QVariant/Expression and parameter takes QVariant only, this is a consequence of that columns are always passed as QString (Expression overload are not never called), but values are passed as QVariant and CAN CONTAIN QVariant(Expression), so investigate in the future and it is also related to the another feature todo, which propose to add Expression overloads for methods in the query builder silverqx
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

QString BaseGrammar::columnize(QStringList columns) const
{
    return wrapArray(columns).join(QStringLiteral(", "));
}

QString BaseGrammar::columnize(const QStringList &columns, const bool isTorrentsTable) const
{
    // BUG Qt and mysql json column silverqx
    /* Qt don't know how to iterate the result with json column, so I have to manually manage
       columns in the select clause. */
    if (isTorrentsTable && (columns.size() == 1) && (columns.at(0) == "*")) {
//        static const QString cached = wrapArray({
//            "id, name, progress, eta, size, seeds, total_seeds, leechers, "
//            "total_leechers, remaining, added_on, hash, status, "
//            "movie_detail_index, savepath"
//        }).join(QStringLiteral(", "));
        static const QString cached =
                wrapArray(QStringList {"id, name, size, progress, added_on, hash"})
                .join(QStringLiteral(", "));

        return cached;
    }

    return columnize(columns);
}

QString BaseGrammar::parameter(const QVariant &value) const
{
    // TODO rethink expressions, how to work with them and pass them to the query builder 🤔 silverqx
    return isExpression(value)
            ? value.value<Expression>().getValue().value<QString>()
            : QStringLiteral("?");
}

QString BaseGrammar::getTablePrefix() const
{
    return m_tablePrefix;
}

BaseGrammar &BaseGrammar::setTablePrefix(const QString &prefix)
{
    m_tablePrefix = prefix;

    return *this;
}

QString BaseGrammar::wrapAliasedValue(const QString &value, const bool prefixAlias) const
{
    auto segments = getSegmentsFromFrom(value);

    /* If we are wrapping a table we need to prefix the alias with the table prefix
       as well in order to generate proper syntax. If this is a column of course
       no prefix is necessary. The condition will be true when from wrapTable. */
    if (prefixAlias)
        segments[1] = m_tablePrefix + segments[1];

    return wrap(segments[0]) + " as "  + wrapValue(segments[1]);
}

QString BaseGrammar::wrapValue(QString value) const
{
    if (value == '*')
        return value;

    return '"' + value.replace(QChar('"'), QStringLiteral("\"\"")) + '"';
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
    // Prevent clazy warning
    const auto columnList = column.split(QChar('.'));

    return columnList.last().trimmed();
}

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
