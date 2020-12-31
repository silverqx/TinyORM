#include "orm/grammar.hpp"

#include <QRegularExpression>

#include "orm/databaseconnection.hpp"
#include "orm/query/joinclause.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

using JoinClause = Query::JoinClause;

namespace
{
    // TODO duplicate in moviedetailservice.cpp silverqx
    /*! Join container by delimiter and exclude empty or null values. */
    const auto joinContainer =
            [](const auto &container, const QString &delimiter) -> QString
    {
        QString result = "";
        int count = 0;
        for (const auto &value : container) {
            if (value.isEmpty() || value.isNull())
                continue;
            result += value + delimiter;
            ++count;
        }
        if (count > 0)
            result.chop(delimiter.size());

        return result;
    };
}

QString Grammar::compileSelect(const QueryBuilder &query) const
{
    return joinContainer(compileComponents(query), QStringLiteral(" ")).trimmed();
}

QString Grammar::compileInsert(const QueryBuilder &query, const QVector<QVariantMap> &values) const
{
    return compileInsert(query, values, false);
}

QString Grammar::compileInsertOrIgnore(const QueryBuilder &query,
                                       const QVector<QVariantMap> &values) const
{
    return compileInsert(query, values, true);
}

QString Grammar::compileUpdate(const QueryBuilder &query, const QVector<UpdateItem> &values) const
{
    const auto table   = query.getTable();
    const auto columns = compileUpdateColumns(values);
    const auto wheres  = compileWheres(query);

    return query.getJoins().isEmpty() ? compileUpdateWithoutJoins(table, columns, wheres)
                                      : compileUpdateWithJoins(query, table, columns, wheres);
}

namespace
{
    /*! Flat bindings map and exclude select and join bindings. */
    const auto flatBindingsForUpdate = [](BindingsMap &bindings)
    {
        QVector<BindingType> bindingsToReject {BindingType::SELECT, BindingType::JOIN};
        QVector<std::reference_wrapper<const QVariant>> cleanBindingsFlatten;

        for (auto itBindingVector = bindings.constBegin(); itBindingVector != bindings.constEnd();
             ++itBindingVector)
        {
            if (bindingsToReject.contains(itBindingVector.key()))
                continue;
            for (const auto &binding : itBindingVector.value())
                cleanBindingsFlatten.append(std::cref(binding));
        }

        return cleanBindingsFlatten;
    };

    /*! Merge a 'from' vector into a 'to' vector. */
    const auto mergeVector = [](auto &to, const auto &from)
    {
        std::copy(from.cbegin(), from.cend(), std::back_inserter(to));
    };

    /*! Merge update values bindings. */
    const auto mergeValuesForUpdate = [](auto &to, const auto &from)
    {
        std::for_each(from.cbegin(), from.cend(), [&to](const auto &updateItem)
        {
            to.append(updateItem.value);
        });
    };
}

QVector<QVariant> Grammar::prepareBindingsForUpdate(const BindingsMap &bindings,
                                                    const QVector<UpdateItem> &values) const
{
    QVector<QVariant> preparedBindings;

    // Merge join bindings from bindings map
    mergeVector(preparedBindings, bindings.find(BindingType::JOIN).value());
    // Merge update values bindings
    mergeValuesForUpdate(preparedBindings, values);

    // Flatten bindings map and exclude select and join bindings and than merge
    // all remaining bindings from flatten bindings map.
    mergeVector(preparedBindings,
                flatBindingsForUpdate(const_cast<BindingsMap &>(bindings)));

    return preparedBindings;
}

QString Grammar::compileDelete(const QueryBuilder &query) const
{
    const auto table  = query.getTable();
    const auto wheres = compileWheres(query);

    return query.getJoins().isEmpty() ? compileDeleteWithoutJoins(table, wheres)
                                      : compileDeleteWithJoins(query, table, wheres);
}

namespace
{
    /*! Flat bindings map and exclude select bindings. */
    const auto flatBindingsForDelete = [](BindingsMap &bindings)
    {
        QVector<BindingType> bindingsToReject {BindingType::SELECT};
        QVector<std::reference_wrapper<const QVariant>> cleanBindingsFlatten;

        for (auto itBindingVector = bindings.constBegin(); itBindingVector != bindings.constEnd();
             ++itBindingVector)
        {
            if (bindingsToReject.contains(itBindingVector.key()))
                continue;
            for (const auto &binding : itBindingVector.value())
                cleanBindingsFlatten.append(std::cref(binding));
        }

        return cleanBindingsFlatten;
    };
}

QVector<QVariant> Grammar::prepareBindingsForDelete(const BindingsMap &bindings) const
{
    QVector<QVariant> preparedBindings;

    // Flatten bindings map and exclude select bindings and than merge
    // all remaining bindings from flatten bindings map.
    mergeVector(preparedBindings,
                flatBindingsForDelete(const_cast<BindingsMap &>(bindings)));

    return preparedBindings;
}

QString Grammar::compileTruncate(const QueryBuilder &query) const
{
    return QStringLiteral("truncate table %1").arg(query.getFrom());
}

Grammar::ComponentsVector
Grammar::compileComponents(const QueryBuilder &query) const
{
    ComponentsVector sql;

    const auto &compileMap = getCompileMap();
    for (const auto &component : compileMap)
        if (component.isset)
            if (component.isset(query))
                sql.append(std::invoke(component.compileMethod, query));

    return sql;
}

QString Grammar::compileColumns(const QueryBuilder &query) const
{
    QString select;

    if (query.getDistinct())
        select += "select distinct ";
    else
        select += "select ";

    return select + columnize(query.getColumns(),
                              (query.getFrom() == "torrents")
                              && (query.getConnection().getDatabaseName() == "q_media_test_orm"));
}

QString Grammar::compileFrom(const QueryBuilder &query) const
{
    return QStringLiteral("from %1").arg(query.getFrom());
}

QString Grammar::compileJoins(const QueryBuilder &query) const
{
    QVector<QString> sql;
    for (const auto &join : query.getJoins()) {
        sql << QStringLiteral("%1 join %2 %3").arg(
                   join->getType(),
                   join->getTable(),
                   compileWheres(*join));
    }

    return joinContainer(sql, QStringLiteral(" "));
}

QString Grammar::compileWheres(const QueryBuilder &query) const
{
    const auto sql = compileWheresToVector(query);
    if (sql.size() > 0)
        return concatenateWhereClauses(query, sql);

    return {};
}

QVector<QString> Grammar::compileWheresToVector(const QueryBuilder &query) const
{
    QVector<QString> compiledWheres;
    for (const auto &where : query.getWheres())
        compiledWheres << QStringLiteral("%1 %2")
                          .arg(where.condition,
                               std::invoke(getWhereMethod(where.type), where));
    return compiledWheres;
}

QString Grammar::concatenateWhereClauses(const QueryBuilder &query, const QVector<QString> &sql) const
{
    // Is query instance of JoinClause?
    const auto conjunction = dynamic_cast<const JoinClause *>(&query) == nullptr
                             ? QStringLiteral("where")
                             : QStringLiteral("on");

    return QStringLiteral("%1 %2").arg(
                conjunction,
                removeLeadingBoolean(joinContainer(sql, QStringLiteral(" "))));
}

QString Grammar::compileGroups(const QueryBuilder &query) const
{
    return QStringLiteral("group by %1").arg(columnize(query.getGroups()));
}

QString Grammar::compileHavings(const QueryBuilder &query) const
{
    QVector<QString> compiledHavings;
    for (const auto &having : query.getHavings())
        compiledHavings << compileHaving(having);

    return QStringLiteral("having %1").arg(
                removeLeadingBoolean(joinContainer(compiledHavings, QStringLiteral(" "))));
}

QString Grammar::compileHaving(const HavingConditionItem &having) const
{
    switch (having.type) {
    case HavingType::BASIC:
        return QStringLiteral("%1 %2 %3 ?").arg(having.condition, having.column,
                                                having.comparison);
    default:
        return {};
    }
}

const QMap<Grammar::ComponentType, Grammar::ComponentValue> &
Grammar::getCompileMap() const
{
    using std::placeholders::_1;
    // Needed, because some compileXx() methods are overloaded
    const auto getBind = [this](const auto &&func)
    {
        return std::bind(std::forward<decltype (func)>(func), this, _1);
    };

    // Pointers to a where member methods by whereType, yes yes c++ 😂
    static const QMap<ComponentType, ComponentValue> cached {
//        {ComponentType::AGGREGATE, {}},
        {ComponentType::COLUMNS,   {getBind(&Grammar::compileColumns),
                        [](const auto &query) { return !query.getColumns().isEmpty(); }}},
        {ComponentType::FROM,      {getBind(&Grammar::compileFrom),
                        [](const auto &query) { return !query.getFrom().isEmpty(); }}},
        {ComponentType::JOINS,     {getBind(&Grammar::compileJoins),
                        [](const auto &query) { return !query.getJoins().isEmpty(); }}},
        {ComponentType::WHERES,    {getBind(&Grammar::compileWheres),
                        [](const auto &query) { return !query.getWheres().isEmpty(); }}},
        {ComponentType::GROUPS,    {getBind(&Grammar::compileGroups),
                        [](const auto &query) { return !query.getGroups().isEmpty(); }}},
        {ComponentType::HAVINGS,   {getBind(&Grammar::compileHavings),
                        [](const auto &query) { return !query.getHavings().isEmpty(); }}},
        {ComponentType::ORDERS,    {getBind(&Grammar::compileOrders),
                        [](const auto &query) { return !query.getOrders().isEmpty(); }}},
        {ComponentType::LIMIT,     {getBind(&Grammar::compileLimit),
                        [](const auto &query) { return query.getLimit() > -1; }}},
        {ComponentType::OFFSET,    {getBind(&Grammar::compileOffset),
                        [](const auto &query) { return query.getOffset() > -1; }}},
//        {ComponentType::LOCK,      {}},
    };

    // TODO correct way to return const & for cached (static) local variable for QHash/QMap, check all 👿🤔 silverqx
    return cached;
}

const std::function<QString(const WhereConditionItem &)> &
Grammar::getWhereMethod(const WhereType whereType) const
{
    using std::placeholders::_1;
    const auto getBind = [this](const auto &&func)
    {
        return std::bind(std::forward<decltype (func)>(func), this, _1);
    };

    // Pointers to a where member methods by whereType, yes yes c++ 😂
    // An order has to be the same as in enum struct WhereType
    // TODO future, QHash would has faster lookup, I should choose QHash silverx
    static const QVector<std::function<QString(const WhereConditionItem &)>> cached {
        getBind(&Grammar::whereBasic),
        getBind(&Grammar::whereNested),
        getBind(&Grammar::whereColumn),
        getBind(&Grammar::whereIn),
        getBind(&Grammar::whereNotIn),
        getBind(&Grammar::whereNull),
        getBind(&Grammar::whereNotNull),
    };
    static const auto size = cached.size();

    // Check if whereType is in the range, just for sure 😏
    const auto type = static_cast<int>(whereType);
    Q_ASSERT((0 <= type) && (type < size));

    return cached.at(type);
}

QString Grammar::whereBasic(const WhereConditionItem &where) const
{
    return QStringLiteral("%1 %2 ?").arg(where.column,
                                         where.comparison);
}

QString Grammar::whereNested(const WhereConditionItem &where) const
{
    /* Here we will calculate what portion of the string we need to remove. If this
       is a join clause query, we need to remove the "on" portion of the SQL and
       if it is a normal query we need to take the leading "where" of queries. */
    const auto offset = 6;

    return QStringLiteral("(%1)").arg(compileWheres(*where.nestedQuery)
                                      .mid(offset));
}

QString Grammar::whereColumn(const WhereConditionItem &where) const
{
    /* In this where type where.column contains first column and where,value contains
       second column. */
    return QStringLiteral("%1 %2 %3").arg(where.column,
                                          where.comparison,
                                          where.value.toString());
}

QString Grammar::whereIn(const WhereConditionItem &where) const
{
    if (where.values.isEmpty())
        return QStringLiteral("0 = 1");

    return QStringLiteral("%1 in (%2)").arg(where.column,
                                            parametrize(where.values));
}

QString Grammar::whereNotIn(const WhereConditionItem &where) const
{
    if (where.values.isEmpty())
        return QStringLiteral("1 = 1");

    return QStringLiteral("%1 not in (%2)").arg(where.column,
                                                parametrize(where.values));
}

QString Grammar::whereNull(const WhereConditionItem &where) const
{
    return QStringLiteral("%1 is null").arg(where.column);
}

QString Grammar::whereNotNull(const WhereConditionItem &where) const
{
    return QStringLiteral("%1 is not null").arg(where.column);
}

QString Grammar::compileOrders(const QueryBuilder &query) const
{
    return QStringLiteral("order by %1").arg(
                joinContainer(compileOrdersToVector(query), QStringLiteral(", ")));
}

QVector<QString> Grammar::compileOrdersToVector(const QueryBuilder &query) const
{
    QVector<QString> compiledOrders;
    for (const auto &order : query.getOrders())
        compiledOrders << QStringLiteral("%1 %2")
                          .arg(order.column, order.direction.toLower());
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

QVector<QString> Grammar::compileInsertToVector(const QVector<QVariantMap> &values) const
{
    /* We need to build a list of parameter place-holders of values that are bound
       to the query. Each insert should have the exact same amount of parameter
       bindings so we will loop through the record and parameterize them all. */
    QVector<QString> compiledParameters;
    for (const auto &valuesMap : values)
        compiledParameters << QStringLiteral("(%1)").arg(parametrize(valuesMap));

    return compiledParameters;
}


QString Grammar::columnize(const QStringList &columns) const
{
    return joinContainer(columns, QStringLiteral(", "));
}

QString Grammar::columnize(const QStringList &columns, const bool isTorrentsTable) const
{
    /* Qt don't know how to iterate the result with json column, so I have to manually manage
       columns in the select clause. */
    if (isTorrentsTable && (columns.size() == 1) && (columns.at(0) == "*")) {
//        static const QString cached {
//            "id, name, progress, eta, size, seeds, total_seeds, leechers, "
//            "total_leechers, remaining, added_on, hash, status, "
//            "movie_detail_index, savepath"
//        };
        static const QString cached {
            "id, name, size, progress, added_on, hash"
        };
        return cached;
    }

    return columnize(columns);
}

template<typename Container>
QString Grammar::parametrize(const Container &values) const
{
    QVector<QString> compiledParameters;
    for (const auto &value : values)
        compiledParameters << parameter(value);

    // TODO move all common QStringLiteral() to the common file silverqx
    return joinContainer(compiledParameters, QStringLiteral(", "));
}

QString Grammar::parameter(const QVariant &value) const
{
    // TODO rethink expressions, how to work with them and pass them to the query builder 🤔 silverqx
    return value.canConvert<Expression>()
            ? value.value<Expression>().getValue().toString()
            : QStringLiteral("?");
}

QString Grammar::removeLeadingBoolean(QString statement) const
{
    return statement.replace(
                QRegularExpression(QStringLiteral("^(and |or )"),
                                   QRegularExpression::CaseInsensitiveOption),
                QStringLiteral(""));
}

QString Grammar::compileInsert(const QueryBuilder &query, const QVector<QVariantMap> &values,
                               const bool ignore) const
{
    Q_ASSERT(values.size() > 0);

    return QStringLiteral("insert%1 into %2 (%3) values %4").arg(
                ignore ? QStringLiteral(" ignore") : QStringLiteral(""),
                query.getTable(),
                // Columns are obtained only from a first QMap
                columnize(values.at(0).keys()),
                joinContainer(compileInsertToVector(values),
                              QStringLiteral(", ")));
}

QString Grammar::compileUpdateColumns(const QVector<UpdateItem> &values) const
{
    QVector<QString> compiledAssignments;
    for (const auto &assignment : values)
        compiledAssignments << QStringLiteral("%1 = %2").arg(
                                   assignment.column,
                                   parameter(assignment.value));

    return joinContainer(compiledAssignments, QStringLiteral(", "));
}

QString Grammar::compileUpdateWithoutJoins(const QString &table, const QString &columns,
                                           const QString &wheres) const
{
    return QStringLiteral("update %1 set %2 %3").arg(table, columns, wheres);
}

QString Grammar::compileUpdateWithJoins(const QueryBuilder &query, const QString &table,
                                        const QString &columns, const QString &wheres) const
{
    const auto joins = compileJoins(query);

    return QStringLiteral("update %1 %2 set %3 %4").arg(table, joins, columns, wheres);
}

QString Grammar::compileDeleteWithoutJoins(const QString &table, const QString &wheres) const
{
    return QStringLiteral("delete from %1 %2").arg(table, wheres);
}

QString Grammar::compileDeleteWithJoins(const QueryBuilder &query, const QString &table,
                                        const QString &wheres) const
{
    const auto alias = table.split(QStringLiteral(" as ")).last().trimmed();

    const auto joins = compileJoins(query);

    /* Alias has to be after the delete keyword and aliased table definition after the
       from keyword. */
    return QStringLiteral("delete %1 from %2 %3 %4").arg(alias, table, joins, wheres);
}

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
