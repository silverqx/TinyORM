#include "orm/query/querybuilder.hpp"

#include "orm/databaseconnection.hpp"
#include "orm/query/joinclause.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Query
{

Builder::Builder(ConnectionInterface &connection, const QueryGrammar &grammar)
    : m_connection(connection)
    , m_grammar(grammar)
{}

QSqlQuery
Builder::get(const QVector<Column> &columns)
{
    return onceWithColumns(columns, [this]
    {
        return runSelect();
    });
}

QSqlQuery
Builder::find(const QVariant &id, const QVector<Column> &columns)
{
    return where("id", "=", id).first(columns);
}

QSqlQuery Builder::first(const QVector<Column> &columns)
{
    auto query = take(1).get(columns);

    query.first();

    return query;
}

QVariant Builder::value(const Column &column)
{
    // Expression support
    QString column_;

    if (std::holds_alternative<Expression>(column))
        column_ = std::get<Expression>(column).getValue().value<QString>();
    else
        column_ = std::get<QString>(column);

    return first({column}).value(column_);
}

QVector<QVariant> Builder::pluck(const QString &column)
{
    /* First, we will need to select the results of the query accounting for the
       given column. Once we have the results, we will be able to take the results
       and get the exact data that was requested for the query. */
    auto query = get({column});

    const auto size = query.size();

    // Empty result
    if (size == 0)
        return {};

    /* If the columns are qualified with a table or have an alias, we cannot use
       those directly in the "pluck" operations since the results from the DB
       are only keyed by the column itself. We'll strip the table out here. */
    const auto unqualifiedColumn = stripTableForPluck(column);

    QVector<QVariant> result;
    if (size > 0)
        result.reserve(size);

    while (query.next())
        result << query.value(unqualifiedColumn);

    return result;
}

QString Builder::toSql()
{
    return m_grammar.compileSelect(*this);
}

std::optional<QSqlQuery>
Builder::insert(const QVariantMap &values)
{
    return insert(QVector<QVariantMap> {values});
}

namespace
{
    const auto flatValuesForInsert = [](const auto &values)
    {
        QVector<QVariant> flattenValues;
        for (const auto &insertMap : values)
            for (const auto &value : insertMap)
                flattenValues << value;

        return flattenValues;
    };
}

// TEST for insert silverqx
std::optional<QSqlQuery>
Builder::insert(const QVector<QVariantMap> &values)
{
    if (values.isEmpty())
        return std::nullopt;

    /* The logic described below is guaranteed by QVariantMap, keys are ordered
       by default.
       Here, we will sort the insert keys for every record so that each insert is
       in the same order for the record. We need to make sure this is the case
       so there are not any errors or problems when inserting these records. */

    return m_connection.insert(m_grammar.compileInsert(*this, values),
                               cleanBindings(flatValuesForInsert(values)));
}

// FEATURE dilemma primarykey, add support for Model::KeyType in QueryBuilder/TinyBuilder or should it be QVariant and runtime type check? 🤔 silverqx
quint64 Builder::insertGetId(const QVariantMap &values, const QString &sequence)
{
    const QVector<QVariantMap> valuesVector {values};

    auto query = m_connection.insert(
                     m_grammar.compileInsertGetId(*this, valuesVector, sequence),
                     cleanBindings(flatValuesForInsert(valuesVector)));

    // FEATURE dilemma primarykey, Model::KeyType vs QVariant, Processor::processInsertGetId() silverqx
    return query.lastInsertId().value<quint64>();
}

std::tuple<int, std::optional<QSqlQuery>>
Builder::insertOrIgnore(const QVector<QVariantMap> &values)
{
    if (values.isEmpty())
        return {0, std::nullopt};

    return m_connection.affectingStatement(
                m_grammar.compileInsertOrIgnore(*this, values),
                cleanBindings(flatValuesForInsert(values)));
}

std::tuple<int, std::optional<QSqlQuery>>
Builder::insertOrIgnore(const QVariantMap &values)
{
    return insertOrIgnore(QVector<QVariantMap> {values});
}

std::tuple<int, QSqlQuery>
Builder::update(const QVector<UpdateItem> &values)
{
    return m_connection.update(
                m_grammar.compileUpdate(*this, values),
                cleanBindings(m_grammar.prepareBindingsForUpdate(getRawBindings(),
                                                                 values)));
}

std::tuple<int, QSqlQuery> Builder::deleteRow()
{
    return remove();
}

std::tuple<int, QSqlQuery> Builder::remove()
{
    return m_connection.remove(
            m_grammar.compileDelete(*this),
            cleanBindings(m_grammar.prepareBindingsForDelete(getRawBindings())));
}

void Builder::truncate()
{
    for (const auto &[sql, bindings] : m_grammar.compileTruncate(*this))
        /* Postgres doesn't execute truncate statement as prepared query:
           https://www.postgresql.org/docs/13/sql-prepare.html */
        if (m_connection.driverName() == "QPSQL")
            m_connection.unprepared(sql);
        else
            m_connection.statement(sql, bindings);
}

QVariant Builder::aggregate(const QString &function, const QVector<Column> &columns)
{
    auto resultsQuery = cloneWithout({PropertyType::COLUMNS})
                        .cloneWithoutBindings({BindingType::SELECT})
                        .setAggregate(function, columns)
                        .get(columns);

    // Empty result
    if (!resultsQuery.first())
        return {};

    return resultsQuery.value(QStringLiteral("aggregate"));
}

Builder &Builder::select(const QVector<Column> &columns)
{
    clearColumns();

    std::ranges::copy(columns, std::back_inserter(m_columns));

    return *this;
}

Builder &Builder::select(const Column &column)
{
    return select(QVector<Column> {column});
}

Builder &Builder::addSelect(const QVector<Column> &columns)
{
    std::ranges::copy(columns, std::back_inserter(m_columns));

    return *this;
}

// FUTURE when appropriate, move inline definitions outside class, check all inline to see what to do silverqx
Builder &Builder::addSelect(const Column &column)
{
    return addSelect(QVector<Column> {column});
}

Builder &Builder::selectRaw(const QString &expression, const QVector<QVariant> &bindings)
{
    addSelect(Expression(expression));

    addBinding(bindings, BindingType::SELECT);

    return *this;
}

Builder &Builder::distinct()
{
    m_distinct = true;

    return *this;
}

Builder &Builder::distinct(const QStringList &columns)
{
    m_distinct = columns;

    return *this;
}

Builder &Builder::distinct(QStringList &&columns)
{
    m_distinct = std::move(columns);

    return *this;
}

Builder &Builder::from(const QString &table, const QString &as)
{
    m_from = as.isEmpty() ? table : QStringLiteral("%1 as %2").arg(table, as);

    return *this;
}

Builder &Builder::from(const Expression &table)
{
    m_from.emplace<Expression>(table);

    return *this;
}

Builder &Builder::from(Expression &&table)
{
    m_from.emplace<Expression>(std::move(table));

    return *this;
}

Builder &Builder::fromRaw(const QString &expression, const QVector<QVariant> &bindings)
{
    m_from.emplace<Expression>(expression);

    addBinding(bindings, BindingType::FROM);

    return *this;
}

Builder &Builder::where(const Column &column, const QString &comparison,
                        const QVariant &value, const QString &condition)
{
    // TODO constexpr, compile check for a invalid comparison operator silverqx
    invalidOperator(comparison);

    m_wheres.append({.column = column, .value = value, .comparison = comparison,
                     .condition = condition, .type = WhereType::BASIC});

    if (!value.canConvert<Expression>())
        // CUR check flattenBindings, I already have flatBindingsForUpdateDelete() algo silverqx
        addBinding(value, BindingType::WHERE);

    return *this;
}

Builder &Builder::orWhere(const Column &column, const QString &comparison,
                          const QVariant &value)
{
    return where(column, comparison, value, QStringLiteral("or"));
}

Builder &Builder::whereEq(const Column &column, const QVariant &value,
                          const QString &condition)
{
    return where(column, QStringLiteral("="), value, condition);
}

Builder &Builder::orWhereEq(const Column &column, const QVariant &value)
{
    return where(column, QStringLiteral("="), value, QStringLiteral("or"));
}

Builder &Builder::where(const std::function<void(Builder &)> &callback,
                        const QString &condition)
{
    // Ownership of the QSharedPointer<QueryBuilder>
    const auto query = forNestedWhere();

    std::invoke(callback, *query);

    return addNestedWhereQuery(query, condition);
}

Builder &Builder::orWhere(const std::function<void(Builder &)> &callback)
{
    return where(callback, QStringLiteral("or"));
}

Builder &Builder::where(const QVector<WhereItem> &values, const QString &condition)
{
    /* We will maintain the boolean we received when the method was called and pass it
       into the nested where.
       The parentheses in this query are ok:
       select * from xyz where (id = ?) */
    return addArrayOfWheres(values, condition);
}

Builder &Builder::orWhere(const QVector<WhereItem> &values)
{
    return where(values, QStringLiteral("or"));
}

Builder &Builder::whereColumn(const QVector<WhereColumnItem> &values,
                              const QString &condition)
{
    return addArrayOfWheres(values, condition);
}

Builder &Builder::orWhereColumn(const QVector<WhereColumnItem> &values)
{
    return addArrayOfWheres(values, QStringLiteral("or"));
}

Builder &Builder::whereColumn(const Column &first, const QString &comparison,
                              const Column &second, const QString &condition)
{
    // Compile check for a invalid comparison operator
    invalidOperator(comparison);

    m_wheres.append({.column = first, .comparison = comparison, .condition = condition,
                     .type = WhereType::COLUMN, .columnTwo = second});

    return *this;
}

Builder &Builder::orWhereColumn(const Column &first, const QString &comparison,
                                const Column &second)
{
    return whereColumn(first, comparison, second, QStringLiteral("or"));
}

Builder &Builder::whereColumnEq(const Column &first, const Column &second,
                                const QString &condition)
{
    return whereColumn(first, QStringLiteral("="), second, condition);
}

Builder &Builder::orWhereColumnEq(const Column &first, const Column &second)
{
    return whereColumn(first, QStringLiteral("="), second, QStringLiteral("or"));
}

Builder &Builder::whereIn(const Column &column, const QVector<QVariant> &values,
                          const QString &condition, const bool nope)
{
    const auto type = nope ? WhereType::NOT_IN : WhereType::IN_;

    m_wheres.append({.column = column, .condition = condition, .type = type,
                     .values = values});

    /* Finally we'll add a binding for each values unless that value is an expression
       in which case we will just skip over it since it will be the query as a raw
       string and not as a parameterized place-holder to be replaced by the DB driver. */
    addBinding(cleanBindings(values), BindingType::WHERE);

    return *this;
}

Builder &Builder::orWhereIn(const Column &column, const QVector<QVariant> &values)
{
    return whereIn(column, values, QStringLiteral("or"));
}

Builder &Builder::whereNotIn(const Column &column, const QVector<QVariant> &values,
                             const QString &condition)
{
    return whereIn(column, values, condition, true);
}

Builder &Builder::orWhereNotIn(const Column &column, const QVector<QVariant> &values)
{
    return whereNotIn(column, values, QStringLiteral("or"));
}

Builder &Builder::whereNull(const QVector<Column> &columns, const QString &condition,
                            const bool nope)
{
    const auto type = nope ? WhereType::NOT_NULL : WhereType::NULL_;

    for (const auto &column : columns)
        m_wheres.append({.column = column, .condition = condition, .type = type});

    return *this;
}

Builder &Builder::orWhereNull(const QVector<Column> &columns)
{
    return whereNull(columns, QStringLiteral("or"));
}

Builder &Builder::whereNotNull(const QVector<Column> &columns, const QString &condition)
{
    return whereNull(columns, condition, true);
}

Builder &Builder::orWhereNotNull(const QVector<Column> &columns)
{
    return whereNotNull(columns, QStringLiteral("or"));
}

Builder &Builder::whereNull(const Column &column, const QString &condition,
                            const bool nope)
{
    return whereNull(QVector<Column> {column}, condition, nope);
}

Builder &Builder::orWhereNull(const Column &column)
{
    return orWhereNull(QVector<Column> {column});
}

Builder &Builder::whereNotNull(const Column &column, const QString &condition)
{
    return whereNotNull(QVector<Column> {column}, condition);
}

Builder &Builder::orWhereNotNull(const Column &column)
{
    return orWhereNotNull(QVector<Column> {column});
}

Builder &Builder::whereRaw(const QString &sql, const QVector<QVariant> &bindings,
                           const QString &condition)
{
    m_wheres.append({.condition = condition, .type = WhereType::RAW, .sql = sql});

    addBinding(bindings, BindingType::WHERE);

    return *this;
}

Builder &Builder::orWhereRaw(const QString &sql, const QVector<QVariant> &bindings)
{
    return whereRaw(sql, bindings, QStringLiteral("or"));
}

Builder &Builder::groupBy(const QVector<Column> &groups)
{
    if (groups.isEmpty())
        return *this;

    std::copy(groups.cbegin(), groups.cend(), std::back_inserter(m_groups));

    return *this;
}

Builder &Builder::groupBy(const Column &group)
{
    return groupBy(QVector<Column> {group});
}

Builder &Builder::groupByRaw(const QString &sql, const QVector<QVariant> &bindings)
{
    m_groups.append(Expression(sql));

    addBinding(bindings, BindingType::GROUPBY);

    return *this;
}

Builder &Builder::having(const Column &column, const QString &comparison,
                         const QVariant &value, const QString &condition)
{
    // Compile check for a invalid comparison operator
    invalidOperator(comparison);

    m_havings.append({column, value, comparison, condition, HavingType::BASIC});

    if (!value.canConvert<Expression>())
        // CUR check flattenBindings, I already have flatBindingsForUpdateDelete() algo silverqx
        addBinding(value, BindingType::HAVING);

    return *this;
}

Builder &Builder::orHaving(const Column &column, const QString &comparison,
                           const QVariant &value)
{
    return having(column, comparison, value, QStringLiteral("or"));
}

Builder &Builder::havingRaw(const QString &sql, const QVector<QVariant> &bindings,
                            const QString &condition)
{
    m_havings.append({.condition = condition, .type = HavingType::RAW, .sql = sql});

    addBinding(bindings, BindingType::HAVING);

    return *this;
}

Builder &Builder::orHavingRaw(const QString &sql, const QVector<QVariant> &bindings)
{
    return havingRaw(sql, bindings, QStringLiteral("or"));
}

Builder &Builder::orderBy(const Column &column, const QString &direction)
{
    const auto &directionLower = direction.toLower();

    if (directionLower != QLatin1String("asc") &&
        directionLower != QLatin1String("desc")
    )
        throw RuntimeError("Order direction must be \"asc\" or \"desc\", "
                           "case is not important.");

    m_orders.append({column, directionLower});

    return *this;
}

Builder &Builder::orderByDesc(const Column &column)
{
    return orderBy(column, QStringLiteral("desc"));
}

Builder &Builder::orderByRaw(const QString &sql, const QVector<QVariant> &bindings)
{
    m_orders.append({.sql = sql});

    addBinding(bindings, BindingType::ORDER);

    return *this;
}

Builder &Builder::latest(const Column &column)
{
    /* Default value "created_at" is ok, because we are in the QueryBuilder,
       in the Model/TinyBuilder is this default argument processed by
       the TinyBuilder::getCreatedAtColumnForLatestOldest() method. */
    return orderBy(column, QStringLiteral("desc"));
}

Builder &Builder::oldest(const Column &column)
{
    return orderBy(column, QStringLiteral("asc"));
}

Builder &Builder::reorder()
{
    m_orders.clear();

    m_bindings.find(BindingType::ORDER)->clear();

    return *this;
}

Builder &Builder::reorder(const Column &column, const QString &direction)
{
    reorder();

    return orderBy(column, direction);
}

Builder &Builder::limit(const int value)
{
    /* I checked negative limit/offset, MySQL and PostgreSQL throws an error,
       SQLite accepts a negative value, but it has no effect and Microsoft SQL Server
       doesn't support negative values too, as is described here:
       https://bit.ly/3yrG7aF */
    Q_ASSERT(value >= 0);

    if (value >= 0)
        m_limit = value;

    return *this;
}

Builder &Builder::take(const int value)
{
    return limit(value);
}

Builder &Builder::offset(const int value)
{
    Q_ASSERT(value >= 0);

    m_offset = std::max(0, value);

    return *this;
}

Builder &Builder::skip(const int value)
{
    return offset(value);
}

Builder &Builder::forPage(const int page, const int perPage)
{
    return offset((page - 1) * perPage).limit(perPage);
}

Builder &Builder::lockForUpdate()
{
    return lock(true);
}

Builder &Builder::sharedLock()
{
    return lock(false);
}

Builder &Builder::lock(const bool value)
{
    m_lock = value;

    // FEATURE read/write connection silverqx
//    if (! is_null($this->lock))
//        useWritePdo();

    return *this;
}

Builder &Builder::lock(const char *value)
{
    /* I need this overload because if I pass 'char *' string to the lock(), the compiler
       selects lock(bool) overload, this behavior is described here:
       https://stackoverflow.com/questions/14770252/string-literal-matches-bool-overload-instead-of-stdstring */
    m_lock = QString(value);

    return *this;
}

Builder &Builder::lock(const QString &value)
{
    m_lock = value;

    return *this;
}

Builder &Builder::lock(QString &&value)
{
    m_lock = std::move(value);

    return *this;
}

QVector<QVariant> Builder::getBindings() const
{
    QVector<QVariant> flattenBindings;

    std::for_each(m_bindings.cbegin(), m_bindings.cend(),
                  [&flattenBindings](const auto &bindings)
    {
        for (const auto &binding : bindings)
            flattenBindings.append(binding);
    });

    return flattenBindings;
}

// TODO next revisit QSharedPointer, after few weeks I'm pretty sure that this can/should be std::unique_pre, like in the TinyBuilder, I need to check if more instances need to save this pointer at once, if don't then I have to change it silverqx
QSharedPointer<Builder> Builder::newQuery() const
{
    return QSharedPointer<Builder>::create(m_connection, m_grammar);
}

QSharedPointer<Builder> Builder::forNestedWhere() const
{
    // Ownership of the QSharedPointer
    const auto query = newQuery();

    query->setFrom(m_from);

    return query;
}

Expression Builder::raw(const QVariant &value) const
{
    return m_connection.raw(value);
}

// TODO now, (still need to be revisited) it can be reference, shared owner will be callee, and copy will be made during m_wheres.append() silverqx
Builder &Builder::addNestedWhereQuery(const QSharedPointer<Builder> &query,
                                      const QString &condition)
{
    if (!(query->m_wheres.size() > 0))
        return *this;

    m_wheres.append({.column = {}, .condition = condition, .type = WhereType::NESTED,
                     .nestedQuery = query});

    const auto &whereBindings =
            query->getRawBindings().find(BindingType::WHERE).value();

    addBinding(whereBindings, BindingType::WHERE);

    return *this;
}

Builder Builder::cloneWithout(const std::unordered_set<PropertyType> &properties) const
{
    auto copy = *this;

    for (const auto property : properties)
        switch (property) {
        case PropertyType::COLUMNS:
            copy.m_columns.clear();
            break;

        default:
            break;
        }

    return copy;
}

Builder Builder::cloneWithoutBindings(
        const std::unordered_set<BindingType> &except) const
{
    auto copy = *this;

    for (const auto bindingType : except)
        switch (bindingType) {
        case BindingType::SELECT:
            copy.m_bindings[BindingType::SELECT].clear();
            break;

        default:
            break;
        }

    return copy;
}

bool Builder::invalidOperator(const QString &comparison) const
{
    const auto comparison_ = comparison.toLower();

    return !m_operators.contains(comparison_) &&
            !m_grammar.getOperators().contains(comparison_);
}

Builder &Builder::addBinding(const QVariant &binding, const BindingType type)
{
    if (!m_bindings.contains(type))
        // TODO add hash which maps BindingType to the QString silverqx
        throw RuntimeError(QStringLiteral("Invalid binding type: %1")
                           .arg(static_cast<int>(type)));

    m_bindings[type].append(binding);

    return *this;
}

Builder &Builder::addBinding(const QVector<QVariant> &bindings, const BindingType type)
{
    // TODO duplicate check, unify silverqx
    if (!m_bindings.contains(type))
        // TODO add hash which maps BindingType to the QString silverqx
        throw RuntimeError(QStringLiteral("Invalid binding type: %1")
                           .arg(static_cast<int>(type)));

    if (!bindings.isEmpty())
        std::ranges::copy(bindings, std::back_inserter(m_bindings[type]));

    return *this;
}

Builder &Builder::addBinding(QVector<QVariant> &&bindings, const BindingType type)
{
    if (!m_bindings.contains(type))
        throw RuntimeError(QStringLiteral("Invalid binding type: %1")
                           .arg(static_cast<int>(type)));

    if (!bindings.isEmpty())
        std::ranges::move(bindings, std::back_inserter(m_bindings[type]));

    return *this;
}

// TODO investigate extended lifetime of reference in cleanBindings(), important case 🤔 silverqx
QVector<QVariant> Builder::cleanBindings(const QVector<QVariant> &bindings) const
{
    // TODO investigate const, move, reserve() vs ctor(size), nice example of move semantics 😏 silverqx
    // FUTURE rewrite with ranges transform, for fun silverqx
    QVector<QVariant> cleanedBindings;
    cleanedBindings.reserve(bindings.size());

    for (const auto &binding : bindings)
        if (!binding.canConvert<Expression>())
            cleanedBindings.append(binding);

    return cleanedBindings;
}

Builder &
Builder::addArrayOfWheres(const QVector<WhereItem> &values, const QString &condition)
{
    return where([&values, &condition](Builder &query)
    {
        for (const auto &where : values)
            query.where(where.column, where.comparison, where.value,
                        where.condition.isEmpty() ? condition : where.condition);

    }, condition);
}

Builder &
Builder::addArrayOfWheres(const QVector<WhereColumnItem> &values,
                          const QString &condition)
{
    // WARN condition also affects condition in QVector, I don't like it silverqx
    return where([&values, &condition](Builder &query)
    {
        for (const auto &where : values)
            query.whereColumn(where.first, where.comparison, where.second,
                              where.condition.isEmpty() ? condition : where.condition);

    }, condition);
}

QSharedPointer<JoinClause>
Builder::newJoinClause(const Builder &query, const QString &type,
                       const QString &table) const
{
    /* It has to be shared pointer, because it can not be passed down to joinInternal()
       in join() as incomplete type. */
    return QSharedPointer<JoinClause>::create(query, type, table);
}

QSharedPointer<JoinClause>
Builder::newJoinClause(const Builder &query, const QString &type,
                       Expression &&table) const
{
    return QSharedPointer<JoinClause>::create(query, type, std::move(table));
}

Builder &Builder::clearColumns()
{
    m_columns.clear();

    m_bindings[BindingType::SELECT].clear();

    return *this;
}

QSqlQuery
Builder::onceWithColumns(
            const QVector<Column> &columns,
            const std::function<QSqlQuery()> &callback)
{
    // Save orignal columns
    const auto original = m_columns;

    if (original.isEmpty())
        m_columns = columns;

    const auto result = std::invoke(callback);

    // After running the callback, the columns are reset to the original value
    m_columns = original;

    return result;
}

std::pair<QString, QVector<QVariant>>
Builder::createSub(const std::function<void(Builder &)> &callback) const
{
    // Ownership of the QSharedPointer<QueryBuilder>
    const auto query = forSubQuery();

    std::invoke(callback, *query);

    prependDatabaseNameIfCrossDatabaseQuery(*query);

    return {query->toSql(), query->getBindings()};
}

std::pair<QString, QVector<QVariant>>
Builder::createSub(Builder &query) const
{
    prependDatabaseNameIfCrossDatabaseQuery(query);

    return {query.toSql(), query.getBindings()};
}

std::pair<QString, QVector<QVariant>>
Builder::createSub(const QString &query) const
{
    return {query, {}};
}

std::pair<QString, QVector<QVariant>>
Builder::createSub(QString &&query) const
{
    return {std::move(query), {}};
}

Builder &Builder::prependDatabaseNameIfCrossDatabaseQuery(Builder &query) const
{
    const auto &queryDatabaseName = query.getConnection().getDatabaseName();
    const auto queryFrom = std::get<QString>(query.m_from);

    if (queryDatabaseName != getConnection().getDatabaseName() &&
        !queryFrom.startsWith(queryDatabaseName) &&
        !queryFrom.contains(QChar('.'))
    )
        query.from(QStringLiteral("%1.%2").arg(queryDatabaseName, queryFrom));

    return query;
}

QString Builder::stripTableForPluck(const QString &column) const
{
    const auto as = QStringLiteral(" as ");

    if (!column.contains(as))
        return m_grammar.unqualifyColumn(column);

    return column.split(as).last().trimmed();
}

Builder &Builder::setAggregate(const QString &function, const QVector<Column> &columns)
{
// TODO clang13 doesn't support in_place construction of aggregates in std::optional.emplace() 😲, gcc and msvc are ok silverqx
#ifdef __clang__
    m_aggregate = {function, columns};
#else
    m_aggregate.emplace(function, columns);
#endif

    if (m_groups.isEmpty()) {
        m_orders.clear();

        m_bindings[BindingType::ORDER].clear();
    }

    return *this;
}

QSqlQuery Builder::runSelect()
{
    return m_connection.select(toSql(), getBindings());
}

Builder &Builder::joinInternal(
            QSharedPointer<JoinClause> &&join, const QString &first,
            const QString &comparison, const QVariant &second, const bool where)
{
    if (where)
        join->where(first, comparison, second);
    else
        join->on(first, comparison, second.value<QString>());

    // Move ownership
    return joinInternal(std::move(join));
}

Builder &Builder::joinInternal(QSharedPointer<JoinClause> &&join,
                               const std::function<void(JoinClause &)> &callback)
{
    std::invoke(callback, *join);

    // Move ownership
    return joinInternal(std::move(join));
}

Builder &Builder::joinInternal(QSharedPointer<JoinClause> &&join)
{
    // For convenience, I want to append first and afterwards add bindings
    const auto &joinRef = *join;

    // Move ownership
    m_joins.append(std::move(join));

    addBinding(joinRef.getBindings(), BindingType::JOIN);

    return *this;
}

Builder &Builder::joinSubInternal(
            std::pair<QString, QVector<QVariant>> &&subQuery, const QString &as,
            const QString &first, const QString &comparison, const QVariant &second,
            const QString &type, const bool where)
{
    auto &[queryString, bindings] = subQuery;

    addBinding(std::move(bindings), BindingType::JOIN);

    return join(Expression(QStringLiteral("(%1) as %2").arg(queryString,
                                                            m_grammar.wrapTable(as))),
                first, comparison, second, type, where);
}

Builder &Builder::joinSubInternal(
        std::pair<QString, QVector<QVariant>> &&subQuery, const QString &as,
        const std::function<void(JoinClause &)> &callback,
        const QString &type)
{
    auto &[queryString, bindings] = subQuery;

    addBinding(std::move(bindings), BindingType::JOIN);

    return join(Expression(QStringLiteral("(%1) as %2").arg(queryString,
                                                            m_grammar.wrapTable(as))),
                callback, type);
}

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
