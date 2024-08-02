#include "orm/query/grammars/grammar.hpp"

#include "orm/databaseconnection.hpp"
#include "orm/macros/likely.hpp"
#include "orm/query/joinclause.hpp"

#ifdef TINYORM_USING_TINYDRIVERS
#  include "orm/exceptions/runtimeerror.hpp"
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Query::Grammars
{

/* public */

QString Grammar::compileSelect(QueryBuilder &query) const
{
    /* If the query does not have any columns set, we'll set the columns to the
       * character to just get all of the columns from the database. Then we
       can build the query and concatenate all the pieces together as one. */
    auto original = query.getColumns();

    if (original.isEmpty())
        query.setColumns({ASTERISK});

    /* To compile the query, we'll spin through each component of the query and
       see if that component exists. If it does we'll just call the compiler
       function for the component which is responsible for making the SQL. */
    auto sql = concatenate(compileComponents(query));

    // Restore original columns value
    query.setColumns(std::move(original));

    return sql;
}

QString Grammar::compileExists(QueryBuilder &query) const
{
    return QStringLiteral("select exists(%1) as %2").arg(compileSelect(query),
                                                         wrap(QStringLiteral("exists")));
}

QString Grammar::compileInsert(const QueryBuilder &query,
                               const QList<QVariantMap> &values) const
{
    const auto table = wrapTable(query.getFrom());

    // FEATURE insert with empty values, this code will never be triggered, because check in the QueryBuilder::insert, even all other code works correctly and support empty values silverqx
    if (values.isEmpty())
        return QStringLiteral("insert into %1 default values").arg(table);

    return QStringLiteral("insert into %1 (%2) values %3").arg(
                table,
                // Columns are obtained only from a first QMap
                columnize(values.constFirst().keys()),
                columnizeWithoutWrap(compileInsertToVector(values)));
}

QString Grammar::compileInsertOrIgnore(const QueryBuilder &/*unused*/,
                                       const QList<QVariantMap> &/*unused*/) const
{
    throw Exceptions::RuntimeError(
                "This database engine does not support inserting while ignoring "
                "errors.");
}

QString Grammar::compileUpdate(QueryBuilder &query,
                               const QList<UpdateItem> &values) const
{
    const auto table   = wrapTable(query.getFrom());
    const auto columns = compileUpdateColumns(values);
    const auto wheres  = compileWheres(query);

    return query.getJoins().isEmpty()
            ? compileUpdateWithoutJoins(query, table, columns, wheres)
            : compileUpdateWithJoins(query, table, columns, wheres);
}

QList<QVariant>
Grammar::prepareBindingsForUpdate(const BindingsMap &bindings,
                                  const QList<UpdateItem> &values)
{
    /* The following ignored diagnostic is correct, I could fix this problem with:
       const auto it = bindings.find(BindingType::JOIN);
       const auto &joinBindings = it.value;
       But I want to how at least one example of ignored diagnostic in the code. */
#if defined(__GNUG__) && !defined(__clang__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wdangling-reference"
#endif
    const auto &joinBindings = bindings.find(BindingType::JOIN).value();
#if defined(__GNUG__) && !defined(__clang__)
#  pragma GCC diagnostic pop
#endif

    QList<QVariant> preparedBindings;
    preparedBindings.reserve(
                joinBindings.size() + values.size() +
                // Rest of the bindings
                computeReserveForBindingsMap(bindings, {BindingType::SELECT,
                                                        BindingType::JOIN}));

    // Join bindings have to go first, I don't remember why 🫤
    preparedBindings << joinBindings;

    // Merge update values bindings
    std::transform(values.cbegin(), values.cend(), std::back_inserter(preparedBindings),
                   [](const auto &updateItem)
    {
        return updateItem.value;
    });

    /* Flatten bindings map and exclude select and join bindings and than merge
       all remaining bindings from flatten bindings map. */
    const auto flatten = flatBindingsForUpdateDelete(bindings, {BindingType::SELECT,
                                                                BindingType::JOIN});

    // std::copy() is ok, 'flatten' contains vector of references
    std::copy(flatten.cbegin(), flatten.cend(), std::back_inserter(preparedBindings));

    return preparedBindings;
}

QString Grammar::compileUpsert(
            QueryBuilder &/*unused*/, const QList<QVariantMap> &/*unused*/,
            const QStringList &/*unused*/, const QStringList &/*unused*/) const
{
    throw Exceptions::RuntimeError("This database engine does not support upserts.");
}

QString Grammar::compileDelete(QueryBuilder &query) const
{
    const auto table  = wrapTable(query.getFrom());
    const auto wheres = compileWheres(query);

    return query.getJoins().isEmpty() ? compileDeleteWithoutJoins(query, table, wheres)
                                      : compileDeleteWithJoins(query, table, wheres);
}

QList<QVariant> Grammar::prepareBindingsForDelete(const BindingsMap &bindings)
{
    QList<QVariant> preparedBindings;
    preparedBindings.reserve(
                computeReserveForBindingsMap(bindings, {BindingType::SELECT}));

    /* Flatten bindings map and exclude select bindings and than merge all remaining
       bindings from flatten bindings map. */
    const auto flatten = flatBindingsForUpdateDelete(bindings, {BindingType::SELECT});

    // std::copy() is ok, 'flatten' contains vector of references
    std::copy(flatten.cbegin(), flatten.cend(), std::back_inserter(preparedBindings));

    return preparedBindings;
}

std::unordered_map<QString, QList<QVariant>>
Grammar::compileTruncate(const QueryBuilder &query) const
{
    return {{QStringLiteral("truncate table %1").arg(wrapTable(query.getFrom())), {}}};
}

QString Grammar::compileRandom(const QString &/*unused*/) const
{
    return QStringLiteral("RANDOM()");
}

const std::unordered_set<QString> &Grammar::getOperators() const
{
    /* I make it this way, I don't declare it as pure virtual intentionally, this gives
       me oportunity to instantiate the Grammar class eg. in tests. */
    static const std::unordered_set<QString> cachedOperators;

    return cachedOperators;
}

/* protected */

bool Grammar::shouldCompileAggregate(const std::optional<AggregateItem> &aggregate)
{
    return aggregate.has_value() && !aggregate->function.isEmpty();
}

bool Grammar::shouldCompileColumns(const QueryBuilder &query)
{
    /* If the query is actually performing an aggregating select, we will let
       compileAggregate() to handle the building of the select clauses, as it will need
       some more syntax that is best handled by that function to keep things neat. */
    return !query.getAggregate() && !query.getColumns().isEmpty();
}

bool Grammar::shouldCompileFrom(const FromClause &from)
{
    return !std::holds_alternative<std::monostate>(from) ||
            (std::holds_alternative<QString>(from) &&
             !std::get<QString>(from).isEmpty());
}

QStringList Grammar::compileComponents(const QueryBuilder &query) const
{
    const auto &compileMap = getCompileMap();

    /* This is not 100% correct as the getCompileMap() is virtual, but it's guaranteed
       that all compileMap-s have the same size, so it's safe to cache this size. */
    static const auto compileMapSize = compileMap.size();
    // The same size for all instances has to be guaranteed as it's static
    Q_ASSERT(compileMapSize == 11);

    QStringList sql;
    sql.reserve(compileMapSize);

    for (const auto &component : compileMap)
        if (component.isset && component.isset(query))
            sql << std::invoke(component.compileMethod, *this, query);

    return sql;
}

QString Grammar::compileAggregate(const QueryBuilder &query) const
{
    /* Whether the aggregate contains a value is checked earlier by
       the shouldCompileAggregate() method. */
    const auto &[function, columns] = *query.getAggregate(); // NOLINT(bugprone-unchecked-optional-access)
    const auto &distinct = query.getDistinct();

    auto column = columnize(columns);

    /* If the query has a "distinct" constraint and we're not asking for all columns
       we need to prepend "distinct" onto the column name so that the query takes
       it into account when it performs the aggregating operations on the data. */
    if (std::holds_alternative<bool>(distinct) && query.getDistinct<bool>() &&
        column != ASTERISK
    ) T_LIKELY
        column = QStringLiteral("distinct %1").arg(column);

    else if (std::holds_alternative<QStringList>(distinct)) T_UNLIKELY
        column = QStringLiteral("distinct %1")
                 .arg(columnize(std::get<QStringList>(distinct)));

    return QStringLiteral("select %1(%2) as %3").arg(function, column,
                                                     wrap(QStringLiteral("aggregate")));
}

QString Grammar::compileColumns(const QueryBuilder &query) const
{
    QString select;

    const auto &distinct = query.getDistinct();

    if (!std::holds_alternative<bool>(distinct))
        throw Exceptions::RuntimeError(
                QStringLiteral("Connection '%1' doesn't support defining more distinct "
                               "columns.")
                .arg(query.getConnection().getName()));

    if (std::get<bool>(distinct))
        select = QStringLiteral("select distinct %1");
    else
        select = QStringLiteral("select %1");

    return select.arg(columnize(query.getColumns()));
}

QString Grammar::compileFrom(const QueryBuilder &query) const
{
    return QStringLiteral("from %1").arg(wrapTable(query.getFrom()));
}

QString Grammar::compileWheres(const QueryBuilder &query) const
{
    const auto sql = compileWheresToVector(query);

    if (!sql.isEmpty())
        return concatenateWhereClauses(query, sql);

    return {};
}

QStringList Grammar::compileWheresToVector(const QueryBuilder &query) const
{
    const auto &wheres = query.getWheres();

    QStringList compiledWheres;
    compiledWheres.reserve(wheres.size());

    for (const auto &where : wheres)
        compiledWheres << SPACE_IN
                          .arg(where.condition,
                               std::invoke(getWhereMethod(where.type), *this, where));

    return compiledWheres;
}

QString Grammar::concatenateWhereClauses(const QueryBuilder &query,
                                         const QStringList &sql)
{
    // Is it a query instance of the JoinClause?
    const auto conjunction = dynamic_cast<const JoinClause *>(&query) == nullptr
                             ? QStringLiteral("where")
                             : QStringLiteral("on");

    return SPACE_IN.arg(conjunction, removeLeadingBoolean(sql.join(SPACE)));
}

QString Grammar::compileJoins(const QueryBuilder &query) const
{
    const auto &joins = query.getJoins();

    QStringList sql;
    sql.reserve(joins.size());

    for (const auto &join : joins)
        sql << QStringLiteral("%1 join %2 %3").arg(join->getType(),
                                                   wrapTable(join->getTable()),
                                                   compileWheres(*join));

    return sql.join(SPACE);
}

QString Grammar::compileGroups(const QueryBuilder &query) const
{
    return QStringLiteral("group by %1").arg(columnize(query.getGroups()));
}

QString Grammar::compileHavings(const QueryBuilder &query) const
{
    const auto &havings = query.getHavings();

    QStringList compiledHavings;
    compiledHavings.reserve(havings.size());

    for (const auto &having : havings)
        compiledHavings << compileHaving(having);

    return QStringLiteral("having %1").arg(removeLeadingBoolean(
                                               compiledHavings.join(SPACE)));
}

QString Grammar::compileHaving(const HavingConditionItem &having) const
{
    /* If the having clause is "raw", we can just return the clause straight away
       without doing any more processing on it. Otherwise, we will compile the
       clause into SQL based on the components that make it up from builder. */
    switch (having.type) {
    T_LIKELY
    case HavingType::BASIC:
        return compileBasicHaving(having);

    T_UNLIKELY
    case HavingType::RAW:
        return SPACE_IN.arg(having.condition, having.sql);

    T_UNLIKELY
    default:
#ifndef TINYORM_DEBUG
        throw Exceptions::RuntimeError(
                    QStringLiteral("Unexpected value for enum struct HavingType."));
#else
        Q_UNREACHABLE();
#endif
    }
}

QString Grammar::compileBasicHaving(const HavingConditionItem &having) const
{
    return QStringLiteral("%1 %2 %3 %4").arg(having.condition, wrap(having.column),
                                             having.comparison, parameter(having.value));
}

QString Grammar::compileOrders(const QueryBuilder &query) const
{
    if (query.getOrders().isEmpty())
        return QLatin1String("");

    return QStringLiteral("order by %1")
            .arg(columnizeWithoutWrap(compileOrdersToVector(query)));
}

QStringList Grammar::compileOrdersToVector(const QueryBuilder &query) const
{
    const auto &orders = query.getOrders();

    QStringList compiledOrders;
    compiledOrders.reserve(orders.size());

    for (const auto &order : orders)
        if (order.sql.isEmpty()) T_LIKELY
            compiledOrders << SPACE_IN.arg(wrap(order.column),
                                           order.direction.toLower());
        else T_UNLIKELY
            compiledOrders << order.sql;

    return compiledOrders;
}

QString Grammar::compileLimit(const QueryBuilder &query) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("limit %1").arg(query.getLimit());
}

QString Grammar::compileOffset(const QueryBuilder &query) const // NOLINT(readability-convert-member-functions-to-static)
{
    return QStringLiteral("offset %1").arg(query.getOffset());
}

QString Grammar::compileLock(const QueryBuilder &query) const
{
    const auto &lock = query.getLock();

    if (std::holds_alternative<QString>(lock))
        return std::get<QString>(lock);

    return QLatin1String("");
}

QString Grammar::whereBasic(const WhereConditionItem &where) const
{
    // FEATURE postgres, try operators with ? vs pdo str_replace(?, ??) https://wiki.php.net/rfc/pdo_escape_placeholders silverqx
    return QStringLiteral("%1 %2 %3").arg(wrap(where.column),
                                          where.comparison,
                                          parameter(where.value));
}

QString Grammar::whereNested(const WhereConditionItem &where) const
{
    /* Here we will calculate what portion of the string we need to remove. If this
       is a join clause query, we need to remove the "on" portion of the SQL and
       if it is a normal query we need to take the leading "where" of queries. */
    auto compiled = compileWheres(*where.nestedQuery);

    const auto offset = compiled.indexOf(SPACE) + 1;

    return PARENTH_ONE.arg(compiled.remove(0, offset));
}

QString Grammar::whereColumn(const WhereConditionItem &where) const
{
    /* In this where type where.column contains first column and where,value contains
       second column. */
    return QStringLiteral("%1 %2 %3").arg(wrap(where.column),
                                          where.comparison,
                                          wrap(where.columnTwo));
}

QString Grammar::whereIn(const WhereConditionItem &where) const
{
    if (where.values.isEmpty())
        return QStringLiteral("0 = 1");

    return QStringLiteral("%1 in (%2)").arg(wrap(where.column),
                                            parametrize(where.values));
}

QString Grammar::whereNotIn(const WhereConditionItem &where) const
{
    if (where.values.isEmpty())
        return QStringLiteral("1 = 1");

    return QStringLiteral("%1 not in (%2)").arg(wrap(where.column),
                                                parametrize(where.values));
}

QString Grammar::whereNull(const WhereConditionItem &where) const
{
    return QStringLiteral("%1 is null").arg(wrap(where.column));
}

QString Grammar::whereNotNull(const WhereConditionItem &where) const
{
    return QStringLiteral("%1 is not null").arg(wrap(where.column));
}

QString Grammar::whereRaw(const WhereConditionItem &where) const // NOLINT(readability-convert-member-functions-to-static)
{
    return where.sql;
}

QString Grammar::whereExists(const WhereConditionItem &where) const
{
    // Compile the nested query (QueryBuilder instance)
    if (where.nestedQuery)
        return QStringLiteral("exists (%1)").arg(compileSelect(*where.nestedQuery));

    Q_ASSERT(std::holds_alternative<Expression>(where.column));

    // Sub-query already compiled in the QueryBuilder using the createSub()
    return QStringLiteral("exists %1").arg(wrap(where.column));
}

QString Grammar::whereNotExists(const WhereConditionItem &where) const
{
    // Compile the nested query (QueryBuilder instance)
    if (where.nestedQuery)
        return QStringLiteral("not exists (%1)").arg(compileSelect(*where.nestedQuery));

    Q_ASSERT(std::holds_alternative<Expression>(where.column));

    // Sub-query already compiled in the QueryBuilder using the createSub()
    return QStringLiteral("not exists %1").arg(wrap(where.column));
}

QString Grammar::whereRowValues(const WhereConditionItem &where) const
{
    return QStringLiteral("(%1) %2 (%3)").arg(columnize(where.columns),
                                              where.comparison,
                                              parametrize(where.values));
}

QString Grammar::whereBetween(const WhereConditionItem &where) const
{
    const auto between = where.nope ? QStringLiteral("not between")
                                    : QStringLiteral("between");

    return QStringLiteral("%1 %2 %3 and %4").arg(wrap(where.column), between,
                                                 parameter(where.between.min),
                                                 parameter(where.between.max));
}

QString Grammar::whereBetweenColumns(const WhereConditionItem &where) const
{
    auto between = where.nope ? QStringLiteral("not between")
                              : QStringLiteral("between");

    return QStringLiteral("%1 %2 %3 and %4").arg(wrap(where.column),
                                                 std::move(between),
                                                 wrap(where.betweenColumns.min),
                                                 wrap(where.betweenColumns.max));
}

QString Grammar::whereDate(const WhereConditionItem &where) const
{
    return dateBasedWhere(QStringLiteral("date"), where);
}

QString Grammar::whereTime(const WhereConditionItem &where) const
{
    return dateBasedWhere(QStringLiteral("time"), where);
}

QString Grammar::whereDay(const WhereConditionItem &where) const
{
    return dateBasedWhere(QStringLiteral("day"), where);
}

QString Grammar::whereMonth(const WhereConditionItem &where) const
{
    return dateBasedWhere(QStringLiteral("month"), where);
}

QString Grammar::whereYear(const WhereConditionItem &where) const
{
    return dateBasedWhere(QStringLiteral("year"), where);
}

QString
Grammar::dateBasedWhere(const QString &type, const WhereConditionItem &where) const
{
    return QStringLiteral("%1(%2) %3 %4").arg(type,
                                              wrap(where.column),
                                              where.comparison,
                                              parameter(where.value));
}

QStringList
Grammar::compileInsertToVector(const QList<QVariantMap> &values) const
{
    /* We need to build a list of parameter place-holders of values that are bound
       to the query. Each insert should have the exact same amount of parameter
       bindings so we will loop through the record and parameterize them all. */
    QStringList compiledParameters;
    compiledParameters.reserve(values.size());

    for (const auto &valuesMap : values)
        compiledParameters << PARENTH_ONE.arg(parametrize(valuesMap));

    return compiledParameters;
}

QString
Grammar::compileUpdateColumns(const QList<UpdateItem> &values) const
{
    QStringList compiledAssignments;
    compiledAssignments.reserve(values.size());

    for (const auto &assignment : values)
        compiledAssignments << QStringLiteral("%1 = %2").arg(
                                   wrap(assignment.column),
                                   parameter(assignment.value));

    return columnizeWithoutWrap(compiledAssignments);
}

QString
Grammar::compileUpdateWithoutJoins(const QueryBuilder &/*unused*/, const QString &table,
                                   const QString &columns, const QString &wheres) const
{
    // The table argument is already wrapped
    return QStringLiteral("update %1 set %2 %3").arg(table, columns, wheres);
}

QString
Grammar::compileUpdateWithJoins(const QueryBuilder &query, const QString &table,
                                const QString &columns, const QString &wheres) const
{
    const auto joins = compileJoins(query);

    // The table argument is already wrapped
    return QStringLiteral("update %1 %2 set %3 %4").arg(table, joins, columns, wheres);
}

QString
Grammar::compileDeleteWithoutJoins(const QueryBuilder &/*unused*/, const QString &table,
                                   const QString &wheres) const
{
    // The table argument is already wrapped
    return QStringLiteral("delete from %1 %2").arg(table, wheres);
}

QString Grammar::compileDeleteWithJoins(const QueryBuilder &query, const QString &table,
                                        const QString &wheres) const
{
    const auto alias = getAliasFromFrom(table);

    const auto joins = compileJoins(query);

    /* Alias has to be after the delete keyword and aliased table definition after the
       from keyword. */
    return QStringLiteral("delete %1 from %2 %3 %4").arg(alias, table, joins, wheres);
}

QString Grammar::concatenate(const QStringList &segments)
{
    QString result;
    result.reserve(ContainerUtils::countStringSizes(segments, 1) + 8);

    for (const auto &segment : segments) {
        if (segment.isEmpty())
            continue;

        result += segment;
        result += SPACE;
    }

    return result.trimmed();
}

QString Grammar::removeLeadingBoolean(QString &&statement)
{
    // Skip all whitespaces after and/or, to avoid trimmed() for performance reasons
    const auto firstChar = [&statement](const auto from)
    {
        for (auto i = from; i < statement.size(); ++i)
            if (statement.at(i) != SPACE)
                return i;

        // Return initial value if space has not been found, should never happen :/
        return from;
    };

    static const auto AndTmpl = QStringLiteral("and ");
    static const auto OrTmpl =  QStringLiteral("or ");

    // RegExp not used for performance reasons
    /* Before and/or could not be whitespace, current implementation doesn't include
       whitespaces before. */
    if (statement.startsWith(AndTmpl))
        return statement.sliced(firstChar(4));

    if (statement.startsWith(OrTmpl))
        return statement.sliced(firstChar(3));

    return std::move(statement);
}

QList<std::reference_wrapper<const QVariant>>
Grammar::flatBindingsForUpdateDelete(const BindingsMap &bindings,
                                     const QList<BindingType> &exclude)
{
    QList<std::reference_wrapper<const QVariant>> cleanBindingsFlatten;
    cleanBindingsFlatten.reserve(computeReserveForBindingsMap(bindings, exclude));

    for (auto itBindings = bindings.constBegin();
         itBindings != bindings.constEnd(); ++itBindings
    )
        if (!exclude.isEmpty() && exclude.contains(itBindings.key()))
            continue;
        else
            for (const auto &binding : itBindings.value())
                cleanBindingsFlatten << std::cref(binding);

    return cleanBindingsFlatten;
}

QList<QVariant>::size_type
Grammar::computeReserveForBindingsMap(const BindingsMap &bindings,
                                      const QList<BindingType> &exclude)
{
    QList<QVariant>::size_type size = 0;

    for (auto itBindings = bindings.constBegin();
         itBindings != bindings.constEnd(); ++itBindings
    )
        if (!exclude.isEmpty() && exclude.contains(itBindings.key()))
            continue;
        else
            size += itBindings.value().size();

    return size;
}

} // namespace Orm::Query::Grammars

TINYORM_END_COMMON_NAMESPACE
