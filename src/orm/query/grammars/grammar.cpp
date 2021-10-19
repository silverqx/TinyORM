#include "orm/query/grammars/grammar.hpp"

#include "orm/databaseconnection.hpp"
#include "orm/macros.hpp"
#include "orm/query/joinclause.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Query::Grammars
{

QString Grammar::compileSelect(QueryBuilder &query) const
{
    /* If the query does not have any columns set, we'll set the columns to the
       * character to just get all of the columns from the database. Then we
       can build the query and concatenate all the pieces together as one. */
    const auto original = query.getColumns();

    if (original.isEmpty())
        query.setColumns({ASTERISK});

    /* To compile the query, we'll spin through each component of the query and
       see if that component exists. If it does we'll just call the compiler
       function for the component which is responsible for making the SQL. */
    const auto sql = concatenate(compileComponents(query));

    // Restore original columns value
    query.setColumns(original);

    return sql;
}

QString Grammar::compileInsert(const QueryBuilder &query,
                               const QVector<QVariantMap> &values) const
{
    const auto table = wrapTable(query.getFrom());

    // FEATURE insert with empty values, this code will never be triggered, because check in the QueryBuilder::insert, even all other code works correctly and support empty values silverqx
    if (values.isEmpty())
        return QStringLiteral("insert into %1 default values").arg(table);

    return QStringLiteral("insert into %1 (%2) values %3").arg(
                table,
                // Columns are obtained only from a first QMap
                columnize(values.at(0).keys()),
                compileInsertToVector(values).join(COMMA));
}

QString Grammar::compileInsertOrIgnore(const QueryBuilder &,
                                       const QVector<QVariantMap> &) const
{
    throw Exceptions::RuntimeError(
                "This database engine does not support inserting while ignoring "
                "errors.");
}

QString Grammar::compileUpdate(QueryBuilder &query,
                               const QVector<UpdateItem> &values) const
{
    const auto table   = wrapTable(query.getFrom());
    const auto columns = compileUpdateColumns(values);
    const auto wheres  = compileWheres(query);

    return query.getJoins().isEmpty()
            ? compileUpdateWithoutJoins(query, table, columns, wheres)
            : compileUpdateWithJoins(query, table, columns, wheres);
}

QVector<QVariant>
Grammar::prepareBindingsForUpdate(const BindingsMap &bindings,
                                  const QVector<UpdateItem> &values) const
{
    QVector<QVariant> preparedBindings(bindings.find(BindingType::JOIN).value());

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

QString Grammar::compileDelete(QueryBuilder &query) const
{
    const auto table  = wrapTable(query.getFrom());
    const auto wheres = compileWheres(query);

    return query.getJoins().isEmpty() ? compileDeleteWithoutJoins(query, table, wheres)
                                      : compileDeleteWithJoins(query, table, wheres);
}

QVector<QVariant> Grammar::prepareBindingsForDelete(const BindingsMap &bindings) const
{
    QVector<QVariant> preparedBindings;

    /* Flatten bindings map and exclude select bindings and than merge all remaining
       bindings from flatten bindings map. */
    const auto flatten = flatBindingsForUpdateDelete(bindings, {BindingType::SELECT});

    // std::copy() is ok, 'flatten' contains vector of references
    std::copy(flatten.cbegin(), flatten.cend(), std::back_inserter(preparedBindings));

    return preparedBindings;
}

std::unordered_map<QString, QVector<QVariant>>
Grammar::compileTruncate(const QueryBuilder &query) const
{
    return {{QStringLiteral("truncate table %1").arg(wrapTable(query.getFrom())), {}}};
}

const QVector<QString> &Grammar::getOperators() const
{
    /* I make it this way, I don't declare it as pure virtual intentionally, this gives
       me oportunity to instantiate the Grammar class eg. in tests. */
    static const QVector<QString> cachedOperators;

    return cachedOperators;
}

bool Grammar::shouldCompileAggregate(const std::optional<AggregateItem> &aggregate) const
{
    return aggregate.has_value() && !aggregate->function.isEmpty();
}

bool Grammar::shouldCompileColumns(const QueryBuilder &query) const
{
    /* If the query is actually performing an aggregating select, we will let
       compileAggregate() to handle the building of the select clauses, as it will need
       some more syntax that is best handled by that function to keep things neat. */
    return !query.getAggregate() && !query.getColumns().isEmpty();
}

bool Grammar::shouldCompileFrom(
        const std::variant<std::monostate, QString, Query::Expression> &from) const
{
    return !std::holds_alternative<std::monostate>(from) ||
            (std::holds_alternative<QString>(from) &&
             !std::get<QString>(from).isEmpty());
}

QStringList Grammar::compileComponents(const QueryBuilder &query) const
{
    QStringList sql;

    const auto &compileMap = getCompileMap();
    for (const auto &component : compileMap)
        if (component.isset)
            if (component.isset(query))
                sql.append(std::invoke(component.compileMethod, query));

    return sql;
}

QString Grammar::compileAggregate(const QueryBuilder &query) const
{
    const auto &aggregate = query.getAggregate();
    const auto &distinct = query.getDistinct();

    auto column = columnize(aggregate->columns);

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

    return QStringLiteral("select %1(%2) as %3").arg(aggregate->function, column,
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

    if (sql.size() > 0)
        return concatenateWhereClauses(query, sql);

    return {};
}

QStringList Grammar::compileWheresToVector(const QueryBuilder &query) const
{
    const auto &wheres = query.getWheres();

    QStringList compiledWheres;
    compiledWheres.reserve(wheres.size());

    for (const auto &where : wheres)
        compiledWheres << QStringLiteral("%1 %2")
                          .arg(where.condition,
                               std::invoke(getWhereMethod(where.type), where));

    return compiledWheres;
}

QString Grammar::concatenateWhereClauses(const QueryBuilder &query,
                                         const QStringList &sql) const
{
    // Is it a query instance of the JoinClause?
    const auto conjunction = dynamic_cast<const JoinClause *>(&query) == nullptr
                             ? QStringLiteral("where")
                             : QStringLiteral("on");

    return QStringLiteral("%1 %2").arg(conjunction,
                                       removeLeadingBoolean(sql.join(SPACE)));
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

    return QStringLiteral("having %1").arg(
                removeLeadingBoolean(compiledHavings.join(SPACE)));
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
        return QStringLiteral("%1 %2").arg(having.condition, having.sql);

    T_UNLIKELY
    default:
        throw Exceptions::RuntimeError(QStringLiteral("Unknown HavingType (%1).")
                                       .arg(static_cast<int>(having.type)));
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

    return QStringLiteral("order by %1").arg(compileOrdersToVector(query).join(COMMA));
}

QStringList Grammar::compileOrdersToVector(const QueryBuilder &query) const
{
    const auto &orders = query.getOrders();

    QStringList compiledOrders;
    compiledOrders.reserve(orders.size());

    for (const auto &order : orders)
        if (order.sql.isEmpty()) T_LIKELY
            compiledOrders << QStringLiteral("%1 %2")
                              .arg(wrap(order.column), order.direction.toLower());
        else T_UNLIKELY
            compiledOrders << order.sql;

    return compiledOrders;
}

QString Grammar::compileLimit(const QueryBuilder &query) const
{
    return QStringLiteral("limit %1").arg(query.getLimit());
}

QString Grammar::compileOffset(const QueryBuilder &query) const
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

QString Grammar::whereRaw(const WhereConditionItem &where) const
{
    return where.sql;
}

QString Grammar::whereExists(const WhereConditionItem &where) const
{
    return QStringLiteral("exists (%1)").arg(compileSelect(*where.nestedQuery));
}

QString Grammar::whereNotExists(const WhereConditionItem &where) const
{
    return QStringLiteral("not exists (%1)").arg(compileSelect(*where.nestedQuery));
}

QStringList
Grammar::compileInsertToVector(const QVector<QVariantMap> &values) const
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
Grammar::compileUpdateColumns(const QVector<UpdateItem> &values) const
{
    QStringList compiledAssignments;
    compiledAssignments.reserve(values.size());

    for (const auto &assignment : values)
        compiledAssignments << QStringLiteral("%1 = %2").arg(
                                   wrap(assignment.column),
                                   parameter(assignment.value));

    return compiledAssignments.join(COMMA);
}

QString
Grammar::compileUpdateWithoutJoins(const QueryBuilder &, const QString &table,
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
Grammar::compileDeleteWithoutJoins(const QueryBuilder &, const QString &table,
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

QString Grammar::concatenate(const QStringList &segments) const
{
    QString result = "";

    for (const auto &segment : segments) {
        if (segment.isEmpty())
            continue;

        result += QStringLiteral("%1 ").arg(segment);
    }

    return result.trimmed();
}

QString Grammar::removeLeadingBoolean(QString statement) const
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

    // RegExp not used for performance reasons
    /* Before and/or could not be whitespace, current implementation doesn't include
       whitespaces before. */
    if (statement.startsWith(QLatin1String("and ")))
        return statement.mid(firstChar(4));
    else if (statement.startsWith(QLatin1String("or ")))
        return statement.mid(firstChar(3));
    else
        return statement;
}

QVector<std::reference_wrapper<const QVariant>>
Grammar::flatBindingsForUpdateDelete(const BindingsMap &bindings,
                                     const QVector<BindingType> &exclude) const
{
    QVector<std::reference_wrapper<const QVariant>> cleanBindingsFlatten;

    for (auto itBindingVector = bindings.constBegin();
         itBindingVector != bindings.constEnd(); ++itBindingVector)

        if (exclude.contains(itBindingVector.key()))
            continue;
        else
            for (const auto &binding : itBindingVector.value())
                cleanBindingsFlatten.append(std::cref(binding));

    return cleanBindingsFlatten;
}

} // namespace Orm::Query::Grammars

TINYORM_END_COMMON_NAMESPACE
