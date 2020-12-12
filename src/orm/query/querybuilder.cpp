#include "querybuilder.h"

#include "orm/databaseconnection.h"
#include "orm/ormerror.h"
#include "orm/query/joinclause.h"

#ifdef MANGO_COMMON_NAMESPACE
namespace MANGO_COMMON_NAMESPACE
{
#endif
namespace Orm::Query
{

Builder::Builder(const DatabaseConnection &db, const Grammar &grammar)
    : m_db(db)
    , m_grammar(grammar)
{}

std::tuple<bool, QSqlQuery>
Builder::get(const QStringList &columns)
{
    if ((false == ((columns.size() == 1) && (columns.at(0) == "*")))
        && !columns.empty())
        m_columns = columns;

    return runSelect();
}

std::tuple<bool, QSqlQuery> Builder::first(const QStringList &columns)
{
    auto [ok, query] = take(1).get(columns);

    if (!ok || !query.isSelect() || !query.isActive())
        return {false, query};

    return {query.first(), query};
}

QVariant Builder::value(const QString &column)
{
    auto [firstOk, query] = first({column});
    if (!firstOk)
        return {};

    return query.value(column);
}

std::tuple<bool, QSqlQuery>
Builder::find(const QVariant &id, const QStringList &columns)
{
    return where("id", "=", id).first(columns);
}

QString Builder::toSql() const
{
    return m_grammar.compileSelect(*this);
}

std::tuple<bool, std::optional<QSqlQuery>>
Builder::insert(const QVariantMap &values) const
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

std::tuple<bool, std::optional<QSqlQuery>>
Builder::insert(const QVector<QVariantMap> &values) const
{
    /* Since every insert gets treated like a batch insert, we will make sure the
       bindings are structured in a way that is convenient when building these
       inserts statements by verifying these elements are actually an array. */
    if (values.isEmpty())
        return {true, std::nullopt};

    /* The logic described below is guaranteed by QVariantMap, keys are ordered
       by default.
       Here, we will sort the insert keys for every record so that each insert is
       in the same order for the record. We need to make sure this is the case
       so there are not any errors or problems when inserting these records. */

    // WARNING cleanBindings() is missing silverqx
    return m_db.insert(m_grammar.compileInsert(*this, values),
                       flatValuesForInsert(values));
}

std::tuple<int, std::optional<QSqlQuery>>
Builder::insertOrIgnore(const QVector<QVariantMap> &values) const
{
    if (values.isEmpty())
        return {0, std::nullopt};

    return m_db.affectingStatement(m_grammar.compileInsertOrIgnore(*this, values),
                                   flatValuesForInsert(values));
}

std::tuple<int, std::optional<QSqlQuery>>
Builder::insertOrIgnore(const QVariantMap &values) const
{
    return insertOrIgnore(QVector<QVariantMap> {values});
}

quint64 Builder::insertGetId(const QVariantMap &values) const
{
    const QVector<QVariantMap> valuesVector {values};

    auto [ok, query] = m_db.insert(
            m_grammar.compileInsertGetId(*this, valuesVector),
            flatValuesForInsert(valuesVector));

    if (!ok)
        return 0;

    return query.lastInsertId().toULongLong();
}

std::tuple<int, QSqlQuery>
Builder::update(const QVector<UpdateItem> &values) const
{
    return m_db.update(m_grammar.compileUpdate(*this, values),
                       cleanBindings(m_grammar.prepareBindingsForUpdate(getRawBindings(),
                                                                        values)));
}

std::tuple<int, QSqlQuery> Builder::remove() const
{
    return m_db.remove(m_grammar.compileDelete(*this),
                       cleanBindings(m_grammar.prepareBindingsForDelete(getRawBindings())));
}

std::tuple<int, QSqlQuery> Builder::remove(const quint64 id)
{
    /* If an ID is passed to the method, we will set the where clause to check the
       ID to let developers to simply and quickly remove a single row from this
       database without manually specifying the "where" clauses on the query. */
    where(m_from + ".id", "=", id, "and");

    return remove();
}

std::tuple<bool, QSqlQuery> Builder::truncate() const
{
    return m_db.statement(m_grammar.compileTruncate(*this));
}

Builder &Builder::where(const QString &column, const QString &comparison,
                        const QVariant &value, const QString &condition)
{
    // Compile check for a invalid comparison operator
    invalidOperator(comparison);

    m_wheres.append({column, value, comparison, condition, WhereType::BASIC});
    addBinding(value, BindingType::WHERE);

    return *this;
}

Builder &Builder::orWhere(const QString &column, const QString &comparison,
                          const QVariant &value)
{
    return where(column, comparison, value, "or");
}

Builder &Builder::whereEq(const QString &column, const QVariant &value,
                          const QString &condition)
{
    return where(column, "=", value, condition);
}

Builder &Builder::orWhereEq(const QString &column, const QVariant &value)
{
    return where(column, "=", value, "or");
}

Builder &Builder::where(const std::function<void(Builder &)> &callback,
                        const QString &condition)
{
    const auto query = forNestedWhere();
    std::invoke(callback, *query);

    return addNestedWhereQuery(query, condition);
}

Builder &Builder::orWhere(const std::function<void (Builder &)> &callback)
{
    return where(callback, "or");
}

Builder &Builder::where(const QVector<WhereItem> &values, const QString &condition)
{
    /* If the column is an array, we will assume it is the QVector of WhereItem-s
       and can add them each as a where clause. We will maintain the boolean we
       received when the method was called and pass it into the nested where. */
    return addArrayOfWheres(values, condition);
}

Builder &Builder::whereColumn(const QString &first, const QString &comparison,
                              const QString &second, const QString &condition)
{
    // Compile check for a invalid comparison operator
    invalidOperator(comparison);

    m_wheres.append({first, second, comparison, condition, WhereType::COLUMN});

    return *this;
}

Builder &Builder::orWhereColumn(const QString &first, const QString &comparison,
                                const QString &second)
{
    return whereColumn(first, comparison, second, "or");
}

Builder &Builder::whereIn(const QString &column, const QVector<QVariant> &values,
                          const QString &condition, const bool nope)
{
    const auto type = nope ? WhereType::NOT_IN : WhereType::IN_;

    m_wheres.append({.column = column, .condition = condition, .type = type, .values = values});

    /* Finally we'll add a binding for each values unless that value is an expression
       in which case we will just skip over it since it will be the query as a raw
       string and not as a parameterized place-holder to be replaced by the DB driver. */
    addBinding(cleanBindings(values), BindingType::WHERE);

    return *this;
}

Builder &Builder::orWhereIn(const QString &column, const QVector<QVariant> &values)
{
    return whereIn(column, values, "or");
}

Builder &Builder::whereNotIn(const QString &column, const QVector<QVariant> &values,
                             const QString &condition)
{
    return whereIn(column, values, condition, true);
}

Builder &Builder::orWhereNotIn(const QString &column, const QVector<QVariant> &values)
{
    return whereNotIn(column, values, "or");
}

Builder &Builder::whereNull(const QStringList &columns, const QString &condition,
                            const bool nope)
{
    const auto type = nope ? WhereType::NOT_NULL : WhereType::NULL_;

    for (const auto &column : columns)
        m_wheres.append({.column = column, .condition = condition, .type = type});

    return *this;
}

Builder &Builder::whereNull(const QString &column, const QString &condition,
                            const bool nope)
{
    return whereNull(QStringList(column), condition, nope);
}

Builder &Builder::orWhereNull(const QStringList &columns)
{
    return whereNull(columns, QStringLiteral("or"));
}

Builder &Builder::orWhereNull(const QString &column)
{
    return orWhereNull(QStringList(column));
}

Builder &Builder::whereNotNull(const QStringList &columns, const QString &condition)
{
    return whereNull(columns, condition, true);
}

Builder &Builder::whereNotNull(const QString &column, const QString &condition)
{
    return whereNotNull(QStringList(column), condition);
}

Builder &Builder::orWhereNotNull(const QStringList &columns)
{
    return whereNotNull(columns, QStringLiteral("or"));
}

Builder &Builder::orWhereNotNull(const QString &column)
{
    return orWhereNotNull(QStringList(column));
}

Builder &Builder::groupBy(const QStringList &groups)
{
    if (groups.isEmpty())
        return *this;

    std::copy(groups.cbegin(), groups.cend(), std::back_inserter(m_groups));

    return *this;
}

Builder &Builder::having(const QString &column, const QString &comparison,
                         const QVariant &value, const QString &condition)
{
    // Compile check for a invalid comparison operator
    invalidOperator(comparison);

    m_havings.append({column, value, comparison, condition, HavingType::BASIC});
    addBinding(value, BindingType::HAVING);

    return *this;
}

Builder &Builder::orHaving(const QString &column, const QString &comparison,
                           const QVariant &value)
{
    return having(column, comparison, value, "or");
}

Builder &Builder::join(const QString &table, const QString &first,
                       const QString &comparison, const QString &second,
                       const QString &type, const bool where)
{
    const auto join = newJoinClause(*this, type, table);

    enum struct JoinType { WHERE, ON };
    const auto method = where ? JoinType::WHERE : JoinType::ON;
    switch (method) {
    case JoinType::WHERE:
        join->where(first, comparison, second);
        break;
    case JoinType::ON:
        join->on(first, comparison, second);
        break;
    }

    m_joins.append(join);
    addBinding(join->getBindings(), BindingType::JOIN);

    return *this;
}

Builder &Builder::join(const QString &table, const std::function<void(JoinClause &)> &callback,
                       const QString &type)
{
    const auto join = newJoinClause(*this, type, table);

    std::invoke(callback, *join);

    m_joins.append(join);
    addBinding(join->getBindings(), BindingType::JOIN);

    return *this;
}

Builder &Builder::leftJoin(const QString &table,
                           const std::function<void (JoinClause &)> &callback)
{
    return join(table, callback, "left");
}

Builder &Builder::rightJoin(const QString &table,
                            const std::function<void (JoinClause &)> &callback)
{
    return join(table, callback, "right");
}

Builder &Builder::crossJoin(const QString &table,
                            const std::function<void (JoinClause &)> &callback)
{
    return join(table, callback, "cross");
}

Builder &Builder::joinWhere(const QString &table, const QString &first,
                            const QString &comparison, const QString &second,
                            const QString &type)
{
    return join(table, first, comparison, second, type, true);
}

Builder &Builder::leftJoin(const QString &table, const QString &first,
                           const QString &comparison, const QString &second)
{
    return join(table, first, comparison, second, "left");
}

Builder &Builder::leftJoinWhere(const QString &table, const QString &first,
                                const QString &comparison, const QString &second)
{
    return join(table, first, comparison, second, "left");
}

Builder &Builder::rightJoin(const QString &table, const QString &first,
                            const QString &comparison, const QString &second)
{
    return join(table, first, comparison, second, "right");
}

Builder &Builder::rightJoinWhere(const QString &table, const QString &first,
                                 const QString &comparison, const QString &second)
{
    return joinWhere(table, first, comparison, second, "right");
}

Builder &Builder::crossJoin(const QString &table, const QString &first,
                            const QString &comparison, const QString &second)
{
    return join(table, first, comparison, second, "cross");
}

Builder &Builder::orderBy(const QString &column, const QString &direction)
{
    const auto &directionLower = direction.toLower();
    if ((directionLower != "asc") && (directionLower != "desc"))
        throw OrmError("Order direction must be \"asc\" or \"desc\", case is not important.");

    m_orders.append({column, directionLower});

    return *this;
}

Builder &Builder::orderByDesc(const QString &column)
{
    return orderBy(column, "desc");
}

Builder &Builder::latest(const QString &column)
{
    return orderBy(column, "desc");
}

Builder &Builder::oldest(const QString &column)
{
    return orderBy(column, "asc");
}

Builder &Builder::reorder()
{
    m_orders.clear();
    m_bindings.find(BindingType::ORDER)->clear();
    return *this;
}

Builder &Builder::reorder(const QString &column, const QString &direction)
{
    reorder();
    return orderBy(column, direction);
}

Builder &Builder::limit(const int value)
{
    Q_ASSERT(value >= 0);
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

QSharedPointer<Builder> Builder::newQuery() const
{
    return QSharedPointer<Builder>::create(m_db, m_grammar);
}

QSharedPointer<Builder> Builder::forNestedWhere() const
{
    const auto query = newQuery();
    query->from(m_from);

    return query;
}

Expression Builder::raw(const QVariant &value) const
{
    return m_db.raw(value);
}

bool Builder::invalidOperator(const QString &comparison) const
{
    const auto contains = m_operators.contains(comparison);
    Q_ASSERT(contains);
    return contains == false;
}

Builder &Builder::addBinding(const QVariant &binding, const BindingType type)
{
    if (!m_bindings.contains(type))
        // TODO add hash to map BindingType to QString silverqx
        throw OrmError(QStringLiteral("Invalid binding type: %1").arg(static_cast<int>(type)));

    m_bindings[type].append(binding);

    return *this;
}

Builder &Builder::addBinding(const QVector<QVariant> &bindings, const BindingType type)
{
    // TODO duplicate check, unify silverqx
    if (!m_bindings.contains(type))
        // TODO add hash to map BindingType to QString silverqx
        throw OrmError(QStringLiteral("Invalid binding type: %1").arg(static_cast<int>(type)));

    std::copy(bindings.cbegin(), bindings.cend(), std::back_inserter(m_bindings[type]));

    return *this;
}

// TODO investigate extended lifetime of reference in cleanBindings(), important case 🤔 silverqx
QVector<QVariant> Builder::cleanBindings(const QVector<QVariant> &bindings) const
{
    // TODO investigate const, move, reserve() vs ctor(size), nice example of move semantics 😏 silverqx
    QVector<QVariant> cleanedBindings;
    cleanedBindings.reserve(bindings.size());
    for (auto &binding : bindings)
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
Builder::addArrayOfWheres(const QVector<WhereColumnItem> &values, const QString &condition)
{
    return where([&values, &condition](Builder &query)
    {
        for (const auto &where : values)
            query.whereColumn(where.first, where.comparison, where.second,
                              where.condition.isEmpty() ? condition : where.condition);

    }, condition);
}

QSharedPointer<JoinClause>
Builder::newJoinClause(const Builder &query, const QString &type, const QString &table) const
{
    return QSharedPointer<JoinClause>::create(query, type, table);
}

Builder &Builder::addNestedWhereQuery(const QSharedPointer<Builder> query,
                                      const QString &condition)
{
    if (!(query->m_wheres.size() > 0))
        return *this;

    m_wheres.append({.condition = condition, .type = WhereType::NESTED,
                     .nestedQuery = query});

    const auto &whereBindings =
            query->getRawBindings().find(BindingType::WHERE).value();

    if (whereBindings.size() > 0)
        addBinding(whereBindings, BindingType::WHERE);

    return *this;
}

std::tuple<bool, QSqlQuery> Builder::runSelect() const
{
    return m_db.select(toSql(), getBindings());
}

} // namespace Orm
#ifdef MANGO_COMMON_NAMESPACE
} // namespace MANGO_COMMON_NAMESPACE
#endif
