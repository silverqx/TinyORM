#include "orm/query/querybuilder.hpp"

#include <QDebug>

#include <range/v3/view/remove_if.hpp>

#include "orm/databaseconnection.hpp"
#include "orm/exceptions/invalidargumenterror.hpp"
#include "orm/query/joinclause.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Utils::Helpers;

namespace Orm::Query
{

/* public */

Builder::Builder(DatabaseConnection &connection, const QueryGrammar &grammar)
    : m_connection(connection)
    , m_grammar(grammar)
{}

/* Retrieving results */

SqlQuery Builder::get(const QVector<Column> &columns)
{
    return onceWithColumns(columns, [this]
    {
        return runSelect();
    });
}

SqlQuery Builder::find(const QVariant &id, const QVector<Column> &columns)
{
    return where(ID, EQ, id).first(columns);
}

SqlQuery Builder::findOr(const QVariant &id, const QVector<Column> &columns,
                         const std::function<void()> &callback)
{
    auto query = find(id, columns);

    if (query.isValid())
        return query;

    // Optionally invoke the callback
    if (callback)
        std::invoke(callback);

    /* Return invalid SqlQuery if a record was not found. Don't return the SqlQuery()
       as an user can still obtain some info from the invalid SqlQuery. */
    return query;
}

SqlQuery Builder::first(const QVector<Column> &columns)
{
    auto query = take(1).get(columns);

    if (m_connection.pretending())
        return query;

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

    const auto query = first({column});

    if (m_connection.pretending())
        return {};

    return query.value(column_);
}

QVariant Builder::soleValue(const Column &column)
{
    // Expression support
    QString column_;

    if (std::holds_alternative<Expression>(column))
        column_ = std::get<Expression>(column).getValue().value<QString>();
    else
        column_ = std::get<QString>(column);

    const auto query = sole({column});

    if (m_connection.pretending())
        return {};

    return query.value(column_);
}

QVector<QVariant> Builder::pluck(const QString &column)
{
    /* First, we will need to select the results of the query accounting for the
       given column. Once we have the results, we will be able to take the results
       and get the exact data that was requested for the query. */
    auto query = get({column});

    const auto size = QueryUtils::queryResultSize(query);

    // Empty result
    if (size == 0)
        return {};

    /* If the column is qualified with a table or have an alias, we cannot use
       those directly in the "pluck" operations, we have to strip the table out or
       use the alias name instead. */
    const auto unqualifiedColumn = stripTableForPluck(column);

    QVector<QVariant> result;
    result.reserve(size);

    while (query.next())
        result << query.value(unqualifiedColumn);

    return result;
}

QString Builder::implode(const QString &column, const QString &glue)
{
    const auto itemsRaw = pluck(column);

    const auto items = itemsRaw | ranges::views::transform([](const auto &item)
    {
        return item.template value<QString>();
    })
            | ranges::to<QStringList>();

    return items.join(glue);
}

QString Builder::toSql()
{
    return m_grammar.compileSelect(*this);
}

namespace
{
    /*! Flat bindings map for an insert statement. */
    const auto flatValuesForInsert = [](const auto &values)
    {
        // All 'values' are const lvalues so no need to for the rvalue 'values'

        QVector<QVariant> flattenValues;

        for (const auto &insertMap : values)
            for (const auto &value : insertMap)
                flattenValues << value;

        return flattenValues;
    };
} // namespace

/* Insert, Update, Delete */

// TEST for insert silverqx
std::optional<SqlQuery>
Builder::insert(const QVector<QVariantMap> &values)
{
    if (values.isEmpty())
        return std::nullopt;

    // This method is for the multi-rows insert
    /* The logic described below is guaranteed by QVariantMap, keys are ordered
       by default.
       Here, we will sort the insert keys for every record so that each insert is
       in the same order for the record. We need to make sure this is the case
       so there are not any errors or problems when inserting these records. */

    return m_connection.insert(m_grammar.compileInsert(*this, values),
                               cleanBindings(flatValuesForInsert(values)));
}

std::optional<SqlQuery>
Builder::insert(const QVariantMap &values)
{
    return insert(QVector<QVariantMap> {values});
}

std::optional<SqlQuery>
Builder::insert(const QVector<QString> &columns,
                const QVector<QVector<QVariant>> &values)
{
    return insert(QueryUtils::zipForInsert(columns, values));
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

std::tuple<int, std::optional<QSqlQuery>>
Builder::insertOrIgnore(const QVector<QString> &columns,
                        const QVector<QVector<QVariant>> &values)
{
    return insertOrIgnore(QueryUtils::zipForInsert(columns, values));
}

std::tuple<int, QSqlQuery>
Builder::update(const QVector<UpdateItem> &values)
{
    return m_connection.update(
                m_grammar.compileUpdate(*this, values),
                cleanBindings(m_grammar.prepareBindingsForUpdate(getRawBindings(),
                                                                 values)));
}

namespace
{
    /*! Merge attributes and values for the updateOrInsert() method. */
    const auto mergeValuesForInsert = [](const QVector<WhereItem> &attributes,
                                         const QVector<UpdateItem> &values)
    {
        QVariantMap result;

        for (const auto &attribute : attributes)
            /* Can not contain expression in the column name, throws
               the std::bad_variant_access exception. */
            result.insert(std::get<QString>(attribute.column), attribute.value);

        for (const auto &value : values)
            result.insert(value.column, value.value);

        return result;
    };
} // namespace

std::tuple<int, std::optional<QSqlQuery>>
Builder::updateOrInsert(const QVector<WhereItem> &attributes,
                        const QVector<UpdateItem> &values)
{
    if (!where(attributes).exists())
        return {-1, insert(mergeValuesForInsert(attributes, values))};

    if (values.isEmpty())
        return {0, std::nullopt};

    return limit(1).update(values);
}

namespace
{
    /*! Flat bindings map for an upsert statement (proxy method for better naming). */
    const auto flatValuesForUpsert = [](auto &&values)
    {
        return flatValuesForInsert(std::forward<decltype (values)>(values));
    };
} // namespace

std::tuple<int, std::optional<QSqlQuery>>
Builder::upsert(const QVector<QVariantMap> &values, const QStringList &uniqueBy,
                const QStringList &update)
{
    // Nothing to do, no values to insert or update
    if (values.isEmpty())
        return {0, std::nullopt};

    // NOTE api different, don't call insert, it's useless silverqx
    // If the update is an empty vector then throw and don't insert
    if (update.isEmpty())
        throw Exceptions::InvalidArgumentError(
                QStringLiteral(
                    "The 'upsert' method doesn't support an empty update argument, "
                    "please use the 'insert' method instead in %1().")
                .arg(__tiny_func__));

    return m_connection.affectingStatement(
                m_grammar.compileUpsert(*this, values, uniqueBy, update),
                cleanBindings(flatValuesForUpsert(values)));
}

std::tuple<int, std::optional<QSqlQuery>>
Builder::upsert(const QVector<QVariantMap> &values, const QStringList &uniqueBy)
{
    // Update all columns
    // Columns are obtained only from a first QMap
    const auto update = values.constFirst().keys();

    return m_connection.affectingStatement(
                m_grammar.compileUpsert(*this, values, uniqueBy, update),
                cleanBindings(flatValuesForUpsert(values)));
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
    for (auto &&[sql, bindings] : m_grammar.compileTruncate(*this))
        /* Postgres doesn't execute truncate statement as prepared query:
           https://www.postgresql.org/docs/13/sql-prepare.html */
        if (m_connection.driverName() == QPSQL)
            m_connection.unprepared(sql);
        else
            m_connection.statement(sql, bindings);
}

/* Select */

QVariant Builder::aggregate(const QString &function,
                            const QVector<Column> &columns) const
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

bool Builder::exists()
{
    auto results = m_connection.select(m_grammar.compileExists(*this), getBindings());

    /* If the results have rows, we will get the row and see if the exists column is a
       boolean true. If there are no results for this query we will return false as
       there are no rows for this query at all, and we can return that info here. */
    if (!results.first())
        return false;

    return results.value(QStringLiteral("exists")).template value<bool>();
}

bool Builder::existsOr(const std::function<void()> &callback)
{
    if (exists())
        return true;

    std::invoke(callback);

    return false;
}

bool Builder::doesntExistOr(const std::function<void()> &callback)
{
    if (doesntExist())
        return true;

    std::invoke(callback);

    return false;
}

Builder &Builder::select(const QVector<Column> &columns)
{
    clearColumns();

    std::ranges::copy(columns, std::back_inserter(m_columns));

    return *this;
}

Builder &Builder::select(const Column &column)
{
    clearColumns();

    m_columns << column;

    return *this;
}

Builder &Builder::addSelect(const QVector<Column> &columns)
{
    std::ranges::copy(columns, std::back_inserter(m_columns));

    return *this;
}

Builder &Builder::addSelect(const Column &column)
{
    m_columns << column;

    return *this;
}

Builder &Builder::select(QVector<Column> &&columns)
{
    clearColumns();

    std::ranges::move(columns, std::back_inserter(m_columns));

    return *this;
}

Builder &Builder::select(Column &&column)
{
    clearColumns();

    m_columns << std::move(column);

    return *this;
}

Builder &Builder::addSelect(QVector<Column> &&columns)
{
    std::ranges::move(columns, std::back_inserter(m_columns));

    return *this;
}

Builder &Builder::addSelect(Column &&column)
{
    m_columns << std::move(column);

    return *this;
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

/* Nested where */

Builder &Builder::where(const std::function<void(Builder &)> &callback,
                        const QString &condition)
{
    // Ownership of the std::shared_ptr<QueryBuilder>
    const auto query = forNestedWhere();

    std::invoke(callback, *query);

    return addNestedWhereQuery(query, condition);
}

Builder &Builder::orWhere(const std::function<void(Builder &)> &callback)
{
    return where(callback, OR);
}

Builder &Builder::whereNot(const std::function<void(Builder &)> &callback,
                           const QString &condition)
{
    return where(callback, SPACE_IN.arg(condition, NOT));
}

Builder &Builder::orWhereNot(const std::function<void(Builder &)> &callback)
{
    return where(callback, SPACE_IN.arg(OR, NOT));
}

/* Array where */

Builder &Builder::where(const QVector<WhereItem> &values, const QString &condition,
                        const QString &defaultCondition)
{
    /* We will maintain the boolean we received when the method was called and pass it
       into the nested where.
       The parentheses in this query are ok:
       select * from xyz where (id = ?) */
    return addArrayOfWheres(values, condition, defaultCondition);
}

Builder &Builder::orWhere(const QVector<WhereItem> &values,
                          const QString &defaultCondition)
{
    return where(values, OR, defaultCondition);
}

Builder &Builder::whereNot(const QVector<WhereItem> &values, const QString &condition,
                           const QString &defaultCondition)
{
    return where(values, SPACE_IN.arg(condition, NOT),
                 // Avoid "and/... not" between all WhereItem-s
                 defaultCondition.isEmpty() ? condition : defaultCondition);
}

Builder &Builder::orWhereNot(const QVector<WhereItem> &values,
                             const QString &defaultCondition)
{
    return where(values, SPACE_IN.arg(OR, NOT),
                 // Avoid "or not" between all WhereItem-s
                 defaultCondition.isEmpty() ? OR : defaultCondition);
}

/* where column */

Builder &Builder::whereColumn(const QVector<WhereColumnItem> &values,
                              const QString &condition)
{
    return addArrayOfWheres(values, condition);
}

Builder &Builder::orWhereColumn(const QVector<WhereColumnItem> &values)
{
    return addArrayOfWheres(values, OR);
}

Builder &Builder::whereColumn(const Column &first, const QString &comparison,
                              const Column &second, const QString &condition)
{
#ifdef TINYORM_DEBUG
    throwIfInvalidOperator(comparison);
#endif

    m_wheres.append({.column = first, .comparison = comparison, .condition = condition,
                     .type = WhereType::COLUMN, .columnTwo = second});

    return *this;
}

Builder &Builder::orWhereColumn(const Column &first, const QString &comparison,
                                const Column &second)
{
    return whereColumn(first, comparison, second, OR);
}

Builder &Builder::whereColumnEq(const Column &first, const Column &second,
                                const QString &condition)
{
    return whereColumn(first, EQ, second, condition);
}

Builder &Builder::orWhereColumnEq(const Column &first, const Column &second)
{
    return whereColumn(first, EQ, second, OR);
}

/* where IN */

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
    return whereIn(column, values, OR);
}

Builder &Builder::whereNotIn(const Column &column, const QVector<QVariant> &values,
                             const QString &condition)
{
    return whereIn(column, values, condition, true);
}

Builder &Builder::orWhereNotIn(const Column &column, const QVector<QVariant> &values)
{
    return whereNotIn(column, values, OR);
}

/* where null */

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
    return whereNull(columns, OR);
}

Builder &Builder::whereNotNull(const QVector<Column> &columns, const QString &condition)
{
    return whereNull(columns, condition, true);
}

Builder &Builder::orWhereNotNull(const QVector<Column> &columns)
{
    return whereNotNull(columns, OR);
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

/* where between */

Builder &Builder::whereBetween(const Column &column, const WhereBetweenItem &values,
                               const QString &condition, const bool nope)
{
    m_wheres.append({.column = column, .condition = condition,
                     .type = WhereType::BETWEEN, .nope = nope,
                     .between = values});

    addBinding(cleanBindings(values), BindingType::WHERE);

    return *this;
}

Builder &Builder::orWhereBetween(const Column &column, const WhereBetweenItem &values)
{
    return whereBetween(column, values, OR);
}

Builder &Builder::whereNotBetween(const Column &column, const WhereBetweenItem &values,
                                  const QString &condition)
{
    return whereBetween(column, values, condition, true);
}

Builder &Builder::orWhereNotBetween(const Column &column, const WhereBetweenItem &values)
{
    return whereBetween(column, values, OR, true);
}

/* where between columns */

Builder &Builder::whereBetweenColumns(
        const Column &column, const WhereBetweenColumnsItem &betweenColumns,
        const QString &condition, const bool nope)
{
    m_wheres.append({.column = column, .condition = condition,
                     .type = WhereType::BETWEEN_COLUMNS, .nope = nope,
                     .betweenColumns = betweenColumns});

    return *this;
}

Builder &Builder::orWhereBetweenColumns(const Column &column,
                                        const WhereBetweenColumnsItem &betweenColumns)
{
    return whereBetweenColumns(column, betweenColumns, OR);
}

Builder &Builder::whereNotBetweenColumns(
        const Column &column, const WhereBetweenColumnsItem &betweenColumns,
        const QString &condition)
{
    return whereBetweenColumns(column, betweenColumns, condition, true);
}

Builder &Builder::orWhereNotBetweenColumns(const Column &column,
                                           const WhereBetweenColumnsItem &betweenColumns)
{
    return whereBetweenColumns(column, betweenColumns, OR, true);
}

/* where exists */

Builder &Builder::whereExists(
        const std::function<void(Builder &)> &callback, const QString &condition,
        const bool nope)
{
    // Ownership of the std::shared_ptr<QueryBuilder>
    const auto query = forSubQuery();

    /* Similar to the sub-select clause, we will create a new query instance so
       the developer may cleanly specify the entire exists query and we will
       compile the whole thing in the grammar and insert it into the SQL. */
    std::invoke(callback, *query);

    return addWhereExistsQuery(query, condition, nope);
}

Builder &Builder::orWhereExists(const std::function<void(Builder &)> &callback,
                                const bool nope)
{
    return whereExists(callback, OR, nope);
}

Builder &Builder::whereNotExists(const std::function<void(Builder &)> &callback,
                                 const QString &condition)
{
    return whereExists(callback, condition, true);
}

Builder &Builder::orWhereNotExists(const std::function<void(Builder &)> &callback)
{
    return whereExists(callback, OR, true);
}

/* where row values */

Builder &
Builder::whereRowValues(const QVector<Column> &columns, const QString &comparison,
                        const QVector<QVariant> &values, const QString &condition)
{
    if (columns.size() != values.size() || columns.isEmpty())
        throw Exceptions::InvalidArgumentError(
                QStringLiteral("The number of columns must match the number of values "
                               "and can not be empty in %1().")
                .arg(__tiny_func__));

    m_wheres.append({.comparison = comparison, .condition = condition,
                     .type = WhereType::ROW_VALUES,
                     .columns = columns, .values = values});

    addBinding(cleanBindings(values), BindingType::WHERE);

    return *this;
}

Builder &
Builder::orWhereRowValues(const QVector<Column> &columns, const QString &comparison,
                          const QVector<QVariant> &values)
{
    return whereRowValues(columns, comparison, values, OR);
}

Builder &
Builder::whereRowValuesEq(const QVector<Column> &columns, const QVector<QVariant> &values,
                          const QString &condition)
{
    return whereRowValues(columns, EQ, values, condition);
}

Builder &
Builder::orWhereRowValuesEq(const QVector<Column> &columns,
                            const QVector<QVariant> &values)
{
    return whereRowValues(columns, EQ, values, OR);
}

/* where dates */

Builder &Builder::whereDate(const Column &column, const QString &comparison,
                            QVariant value, const QString &condition)
{
    // Convert to the QString representation (support both QDate and QDateTime)
    if (const auto type = Helpers::qVariantTypeId(value);
        type == QMetaType::QDate
    )
        value = value.value<QDate>().toString(Qt::ISODate);

    else if (type == QMetaType::QDateTime)
        value = value.value<QDateTime>().date().toString(Qt::ISODate);

    return whereInternal(column, comparison, std::move(value), condition,
                         WhereType::DATE);
}

Builder &Builder::whereTime(const Column &column, const QString &comparison,
                            QVariant value, const QString &condition)
{
    // Convert to the QString representation (support both QTime and QDateTime)
    if (const auto type = Helpers::qVariantTypeId(value);
        type == QMetaType::QTime
    )
        value = value.value<QTime>().toString(Qt::ISODate);

    else if (type == QMetaType::QDateTime)
        value = value.value<QDateTime>().time().toString(Qt::ISODate);

    return whereInternal(column, comparison, std::move(value), condition,
                         WhereType::TIME);
}

Builder &Builder::whereDay(const Column &column, const QString &comparison,
                           QVariant value, const QString &condition)
{
    // Convert to the int representation (support both QDate and QDateTime)
    if (const auto type = Helpers::qVariantTypeId(value);
        type == QMetaType::QDate
    )
        value = value.value<QDate>().day();

    else if (type == QMetaType::QDateTime)
        value = value.value<QDateTime>().date().day();

    else
        value = value.value<int>();

    return whereInternal(column, comparison, std::move(value), condition,
                         WhereType::DAY);
}

Builder &Builder::whereMonth(const Column &column, const QString &comparison,
                             QVariant value, const QString &condition)
{
    // Convert to the int representation (support both QDate and QDateTime)
    if (const auto type = Helpers::qVariantTypeId(value);
        type == QMetaType::QDate
    )
        value = value.value<QDate>().month();

    else if (type == QMetaType::QDateTime)
        value = value.value<QDateTime>().date().month();

    else
        value = value.value<int>();

    return whereInternal(column, comparison, std::move(value), condition,
                         WhereType::MONTH);
}

Builder &Builder::whereYear(const Column &column, const QString &comparison,
                            QVariant value, const QString &condition)
{
    // Convert to the int representation (support both QDate and QDateTime)
    if (const auto type = Helpers::qVariantTypeId(value);
        type == QMetaType::QDate
    )
        value = value.value<QDate>().year();

    else if (type == QMetaType::QDateTime)
        value = value.value<QDateTime>().date().year();

    else
        value = value.value<int>();

    return whereInternal(column, comparison, std::move(value), condition,
                         WhereType::YEAR);
}

/* where raw */

Builder &Builder::whereRaw(const QString &sql, const QVector<QVariant> &bindings,
                           const QString &condition)
{
    m_wheres.append({.condition = condition, .type = WhereType::RAW, .sql = sql});

    addBinding(bindings, BindingType::WHERE);

    return *this;
}

Builder &Builder::orWhereRaw(const QString &sql, const QVector<QVariant> &bindings)
{
    return whereRaw(sql, bindings, OR);
}

/* Group by and having */

Builder &Builder::groupBy(const QVector<Column> &groups)
{
    if (groups.isEmpty())
        return *this;

    std::ranges::copy(groups, std::back_inserter(m_groups));

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
#ifdef TINYORM_DEBUG
    throwIfInvalidOperator(comparison);
#endif

    m_havings.append({column, value, comparison, condition, HavingType::BASIC});

    if (!value.canConvert<Expression>())
        addBinding(value, BindingType::HAVING);

    return *this;
}

Builder &Builder::orHaving(const Column &column, const QString &comparison,
                           const QVariant &value)
{
    return having(column, comparison, value, OR);
}

Builder &Builder::havingRaw(const QString &sql, const QVector<QVariant> &bindings,
                            const QString &condition)
{
    m_havings.append({.condition = condition, .type = HavingType::RAW, .sql = sql});

    addBinding(bindings, BindingType::HAVING);

    return *this;
}

// TODO stackoverflow, I think all of these kind of methods should be inline silverqx
Builder &Builder::orHavingRaw(const QString &sql, const QVector<QVariant> &bindings)
{
    return havingRaw(sql, bindings, OR);
}

/* Ordering */

Builder &Builder::orderBy(const Column &column, const QString &direction)
{
    const auto &directionLower = direction.toLower();

    if (directionLower != ASC && directionLower != DESC)
        throw Exceptions::InvalidArgumentError(
                QStringLiteral(
                    "Order direction must be \"asc\" or \"desc\", case is not important "
                    "in %1().)")
                .arg(__tiny_func__));

    m_orders.append({column, directionLower});

    return *this;
}

Builder &Builder::orderByDesc(const Column &column)
{
    return orderBy(column, DESC);
}

Builder &Builder::inRandomOrder(const QString &seed)
{
    return orderByRaw(m_grammar.compileRandom(seed));
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
    return orderBy(column, DESC);
}

Builder &Builder::oldest(const Column &column)
{
    return orderBy(column, ASC);
}

Builder &Builder::reorder()
{
    m_orders.clear();

    m_bindings[BindingType::ORDER].clear();

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

// NOTE api little different, added bool prependOrder parameter silverqx
Builder &Builder::forPageBeforeId(const int perPage, const QVariant &lastId,
                                  const QString &column, const bool prependOrder)
{
    m_orders = removeExistingOrdersFor(column);

    if (lastId.isValid() && !lastId.isNull())
        where(column, LT, lastId);

    if (prependOrder)
        m_orders.prepend({column, DESC});
    else
        orderBy(column, DESC);

    return limit(perPage);
}

// NOTE api little different, added bool prependOrder parameter silverqx
Builder &Builder::forPageAfterId(const int perPage, const QVariant &lastId,
                                 const QString &column, const bool prependOrder)
{
    m_orders = removeExistingOrdersFor(column);

    if (lastId.isValid() && !lastId.isNull())
        where(column, GT, lastId);

    if (prependOrder)
        m_orders.prepend({column, ASC});
    else
        orderBy(column, ASC);

    return limit(perPage);
}

/* Pessimistic Locking */

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

/* Debugging */

// NOTE api different, added the replaceBindings and simpleBindings parameters silverqx
void Builder::dump(const bool replaceBindings, const bool simpleBindings)
{
    auto queryString = toSql();
    auto bindings = getBindings();

    auto [queryStringReplaced, simpleBindingsList] =
            QueryUtils::replaceBindingsInSql(queryString, bindings, simpleBindings);

    qDebug().noquote() << (replaceBindings ? std::move(queryStringReplaced)
                                           : std::move(queryString));

    if (replaceBindings)
        return;

    /* Show bindings on own line if replace bindings is disabled.
       Simple bindings logs bindings without the type information. */
    if (simpleBindings)
        qDebug().noquote() << simpleBindingsList.join(COMMA);
    else
        qDebug() << std::move(bindings);
}

void Builder::dd(const bool replaceBindings, const bool simpleBindings)
{
    dump(replaceBindings, simpleBindings);

    exit(1); // NOLINT(concurrency-mt-unsafe)
}

/* Getters / Setters */

const QString &Builder::defaultKeyName() const
{
    return ID;
}

QVector<QVariant> Builder::getBindings() const
{
    QVector<QVariant> flattenBindings;

    for (const auto &bindings : std::as_const(m_bindings))
        for (const auto &binding : bindings)
            flattenBindings.append(binding);

    return flattenBindings;
}

Builder &Builder::addBinding(const QVariant &binding, const BindingType type)
{
#ifdef TINYORM_DEBUG
    // Check if m_bindings contain type
    checkBindingType(type);
#endif

    m_bindings[type].append(binding);

    return *this;
}

Builder &Builder::addBinding(QVariant &&binding, const BindingType type)
{
#ifdef TINYORM_DEBUG
    // Check if m_bindings contain type
    checkBindingType(type);
#endif

    m_bindings[type].append(std::move(binding));

    return *this;
}

Builder &Builder::addBinding(const QVector<QVariant> &bindings, const BindingType type)
{
#ifdef TINYORM_DEBUG
    // Check if m_bindings contain type
    checkBindingType(type);
#endif

    if (!bindings.isEmpty())
        std::ranges::copy(bindings, std::back_inserter(m_bindings[type]));

    return *this;
}

Builder &Builder::addBinding(QVector<QVariant> &&bindings, const BindingType type)
{
#ifdef TINYORM_DEBUG
    // Check if m_bindings contain type
    checkBindingType(type);
#endif

    if (!bindings.isEmpty())
        std::ranges::move(bindings, std::back_inserter(m_bindings[type]));

    return *this;
}

Builder &Builder::setBindings(QVector<QVariant> &&bindings, const BindingType type)
{
#ifdef TINYORM_DEBUG
    // Check if m_bindings contain type
    checkBindingType(type);
#endif

    auto &bindingsRef = m_bindings[type]; // clazy:exclude=detaching-member

    bindingsRef.reserve(bindings.size());
    bindingsRef = std::move(bindings);

    return *this;
}

/* Other methods */

std::shared_ptr<Builder> Builder::newQuery() const
{
    /* It has to be the shared pointer because it is returned to the user so instances
       counting is necessary, also saved internally eg. in the TinyBuilder::m_query. */
    return std::make_shared<Builder>(m_connection, m_grammar);
}

std::shared_ptr<Builder> Builder::forNestedWhere() const
{
    // Ownership of the std::shared_ptr
    auto query = newQuery();

    query->setFrom(m_from);

    return query;
}

Expression Builder::raw(const QVariant &value) const
{
    return m_connection.raw(value);
}

Builder &Builder::addNestedWhereQuery(const std::shared_ptr<Builder> &query,
                                      const QString &condition)
{
    if (query->m_wheres.isEmpty())
        return *this;

    m_wheres.append({.column = {}, .condition = condition, .type = WhereType::NESTED,
                     .nestedQuery = query});

    const auto &whereBindings =
            query->getRawBindings().find(BindingType::WHERE).value();

    addBinding(whereBindings, BindingType::WHERE);

    return *this;
}

Builder &Builder::addWhereExistsQuery(const std::shared_ptr<Builder> &query,
                                      const QString &condition, const bool nope)
{
    const auto type = nope ? WhereType::NOT_EXISTS : WhereType::EXISTS;

    m_wheres.append({.condition = condition, .type = type, .nestedQuery = query});

    addBinding(query->getBindings(), BindingType::WHERE);

    return *this;
}

Builder &Builder::mergeWheres(const QVector<WhereConditionItem> &wheres,
                              const QVector<QVariant> &bindings)
{
    m_wheres += wheres;

    m_bindings[BindingType::WHERE] += bindings;

    return *this;
}

Builder &Builder::mergeWheres(QVector<WhereConditionItem> &&wheres,
                              QVector<QVariant> &&bindings)
{
    std::ranges::move(wheres, std::back_inserter(m_wheres));

    std::ranges::move(bindings, std::back_inserter(m_bindings[BindingType::WHERE]));

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
            Q_UNREACHABLE();
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
            Q_UNREACHABLE();
        }

    return copy;
}

QString Builder::stripTableForPluck(const QString &column) const
{
    static const auto as = QStringLiteral(" as ");

    if (!column.contains(as))
        return m_grammar.unqualifyColumn(column);

    return m_grammar.getAliasFromFrom(column);
}

/* protected */

void Builder::throwIfInvalidOperator(const QString &comparison) const
{
    const auto comparison_ = comparison.toLower();

    if (getOperators().contains(comparison_) ||
        m_grammar.getOperators().contains(comparison_)
    )
        return;

    throw Exceptions::InvalidArgumentError(
                QStringLiteral("The '%1' operator is not valid for the '%2' database "
                               "in %3().")
                .arg(comparison_, getConnection().driverNamePrintable(), __tiny_func__));
}

QVector<QVariant> Builder::cleanBindings(const QVector<QVariant> &bindings) const
{
    QVector<QVariant> cleanedBindings;
    cleanedBindings.reserve(bindings.size());

    for (const auto &binding : bindings)
        if (!binding.canConvert<Expression>())
            cleanedBindings << binding;

    return cleanedBindings;
}

QVector<QVariant> Builder::cleanBindings(QVector<QVariant> &&bindings) const
{
    QVector<QVariant> cleanedBindings;
    cleanedBindings.reserve(bindings.size());

    for (auto &&binding : bindings)
        if (!binding.canConvert<Expression>())
            cleanedBindings << std::move(binding);

    return cleanedBindings;
}

QVector<QVariant> Builder::cleanBindings(const WhereBetweenItem &bindings) const
{
    QVector<QVariant> cleanedBindings;
    cleanedBindings.reserve(2);

    if (const auto &min = bindings.min;
        !min.canConvert<Expression>()
    )
        cleanedBindings << min;

    if (const auto &max = bindings.max;
        !max.canConvert<Expression>()
    )
        cleanedBindings << max;

    return cleanedBindings;
}

Builder &
Builder::addArrayOfWheres(const QVector<WhereItem> &values, const QString &condition,
                          const QString &defaultCondition)
{
    return where([&values, &condition, &defaultCondition](Builder &query)
    {
        for (const auto &where : values)
            query.where(where.column, where.comparison, where.value,
                        where.condition.isEmpty()
                        // Allow to pass a default condition for the QVector<WhereItem>
                        ? (defaultCondition.isEmpty() ? condition : defaultCondition)
                        : where.condition);

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

std::shared_ptr<JoinClause>
Builder::newJoinClause(const Builder &query, const QString &type,
                       const QString &table) const
{
    /* It has to be shared pointer, because it can not be passed down to joinInternal()
       in join() as incomplete type. */
    return std::make_shared<JoinClause>(query, type, table);
}

std::shared_ptr<JoinClause>
Builder::newJoinClause(const Builder &query, const QString &type,
                       Expression &&table) const
{
    return std::make_shared<JoinClause>(query, type, std::move(table));
}

Builder &Builder::clearColumns()
{
    m_columns.clear();

    m_bindings[BindingType::SELECT].clear();

    return *this;
}

SqlQuery
Builder::onceWithColumns(const QVector<Column> &columns,
                         const std::function<SqlQuery()> &callback)
{
    // Save orignal columns
    auto original = m_columns;

    if (original.isEmpty())
        m_columns = columns;

    auto result = std::invoke(callback);

    // After running the callback, the columns are reset to the original value
    m_columns = std::move(original);

    return result;
}

std::pair<QString, QVector<QVariant>>
Builder::createSub(const std::function<void(Builder &)> &callback) const
{
    // Ownership of the std::shared_ptr<QueryBuilder>
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
    auto queryFrom = std::get<QString>(query.m_from);

    if (queryDatabaseName != getConnection().getDatabaseName() &&
        !queryFrom.startsWith(queryDatabaseName) &&
        !queryFrom.contains(DOT)
    )
        query.from(DOT_IN.arg(queryDatabaseName, std::move(queryFrom)));

    return query;
}

void Builder::enforceOrderBy() const
{
    if (m_orders.isEmpty())
        throw Exceptions::RuntimeError(
                QStringLiteral("You must specify an orderBy clause when using "
                               "the '%1()' method in %2().")
                .arg(__func__, __tiny_func__));
}

QVector<OrderByItem> Builder::removeExistingOrdersFor(const QString &column) const
{
    return m_orders
            | ranges::views::remove_if([&column](const OrderByItem &order)
    {
        if (std::holds_alternative<Expression>(order.column))
            return false;

        return std::get<QString>(order.column) == column;
    })
            | ranges::to<QVector<OrderByItem>>();
}

/* Getters / Setters */

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

/* private */

SqlQuery Builder::runSelect()
{
    return m_connection.select(toSql(), getBindings());
}

Builder &Builder::joinInternal(
            std::shared_ptr<JoinClause> &&join, const QString &first,
            const QString &comparison, const QVariant &second, const bool where)
{
    if (where)
        join->where(first, comparison, second);
    else
        join->on(first, comparison, second.value<QString>());

    // Move ownership
    return joinInternal(std::move(join));
}

Builder &Builder::joinInternal(std::shared_ptr<JoinClause> &&join,
                               const std::function<void(JoinClause &)> &callback)
{
    std::invoke(callback, *join);

    // Move ownership
    return joinInternal(std::move(join));
}

Builder &Builder::joinInternal(std::shared_ptr<JoinClause> &&join)
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

    return join(Expression(QStringLiteral("(%1) as %2").arg(std::move(queryString),
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

    return join(Expression(QStringLiteral("(%1) as %2").arg(std::move(queryString),
                                                            m_grammar.wrapTable(as))),
                callback, type);
}

Builder &Builder::whereInternal(
        const Column &column, const QString &comparison, QVariant value,
        const QString &condition, const WhereType type)
{
#ifdef TINYORM_DEBUG
    throwIfInvalidOperator(comparison);
#endif

    m_wheres.append({.column = column, .value = value, .comparison = comparison,
                     .condition = condition, .type = type});

    if (!value.canConvert<Expression>())
        addBinding(std::move(value), BindingType::WHERE);

    return *this;
}

void Builder::checkBindingType(const BindingType type) const
{
    if (m_bindings.contains(type))
        return;

    // TODO add hash which maps BindingType to the QString silverqx
    throw Exceptions::InvalidArgumentError(
                QStringLiteral("Invalid binding type '%1' in %2().")
                .arg(static_cast<int>(type))
                .arg(__tiny_func__));
}

const QVector<QString> &Builder::getOperators()
{
    static const QVector<QString> cachedOperators {
        EQ, LT, GT, LE, GE, NE_, NE, QLatin1String("<=>"),
        LIKE, QLatin1String("like binary"), NLIKE, ILIKE,
        B_AND, B_OR, "^", "<<", ">>", "&~",
        QLatin1String("rlike"),  QLatin1String("not rlike"),
        QLatin1String("regexp"), QLatin1String("not regexp"),
        "~", "~*", "!~", "!~*", QLatin1String("similar to"),
        QLatin1String("not similar to"), QLatin1String("not ilike"), "~~*", "!~~*",
    };

    return cachedOperators;
}

} // namespace Orm::Query

TINYORM_END_COMMON_NAMESPACE
