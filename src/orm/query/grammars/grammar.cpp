#include "orm/query/grammars/grammar.hpp"

#include <QRegularExpression>

#include "orm/databaseconnection.hpp"
#include "orm/query/joinclause.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Query::Grammars
{

QString Grammar::compileSelect(QueryBuilder &query) const
{
    /* If the query does not have any columns set, we'll set the columns to the
       * character to just get all of the columns from the database. Then we
       can build the query and concatenate all the pieces together as one. */
    const auto original = query.getColumns();

    if (original.isEmpty())
        query.setColumns({"*"});

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
                compileInsertToVector(values).join(", "));
}

QString Grammar::compileInsertOrIgnore(const QueryBuilder &,
                                       const QVector<QVariantMap> &) const
{
    throw RuntimeError("This database engine does not support inserting while "
                       "ignoring errors.");
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

namespace
{
    /*! Flat bindings map and exclude select and join bindings. */
    const auto flatBindingsForUpdate = [](BindingsMap &bindings)
    {
        QVector<BindingType> bindingsToReject {BindingType::SELECT, BindingType::JOIN};
        QVector<std::reference_wrapper<const QVariant>> cleanBindingsFlatten;

        for (auto itBindingVector = bindings.constBegin();
             itBindingVector != bindings.constEnd(); ++itBindingVector)
        {
            if (bindingsToReject.contains(itBindingVector.key()))
                continue;
            for (const auto &binding : itBindingVector.value())
                cleanBindingsFlatten.append(std::cref(binding));
        }

        return cleanBindingsFlatten;
    };

    // TODO revisit, is merge really needed, because this is not merge fuckin silverqx
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

QVector<QVariant>
Grammar::prepareBindingsForUpdate(const BindingsMap &bindings,
                                  const QVector<UpdateItem> &values) const
{
    QVector<QVariant> preparedBindings;

    // TODO I don't know why I'm using mergeVector here, when the preparedBindings can be initialized by ctor, revisit all this code, because it is the one of the first code I wrote and I'm much better in c++ now silverqx
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

QString Grammar::compileDelete(QueryBuilder &query) const
{
    const auto table  = wrapTable(query.getFrom());
    const auto wheres = compileWheres(query);

    return query.getJoins().isEmpty() ? compileDeleteWithoutJoins(query, table, wheres)
                                      : compileDeleteWithJoins(query, table, wheres);
}

namespace
{
    /*! Flat bindings map and exclude select bindings. */
    const auto flatBindingsForDelete = [](BindingsMap &bindings)
    {
        QVector<BindingType> bindingsToReject {BindingType::SELECT};
        QVector<std::reference_wrapper<const QVariant>> cleanBindingsFlatten;

        for (auto itBindingVector = bindings.constBegin();
             itBindingVector != bindings.constEnd();
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

QString Grammar::compileColumns(const QueryBuilder &query) const
{
    QString select;

    if (query.getDistinct())
        select += "select distinct ";
    else
        select += "select ";

    // BUG Qt and mysql json column silverqx
    // Contains json column
    const auto isTorrentsTable =
            (query.getFrom() == "torrents")
            && (query.getConnection().getDatabaseName() == "q_media_test_orm");

    return select + columnize(query.getColumns(), isTorrentsTable);
}

QString Grammar::compileFrom(const QueryBuilder &query) const
{
    return QStringLiteral("from %1").arg(wrapTable(query.getFrom()));
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

    return sql.join(' ');
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
                                       removeLeadingBoolean(sql.join(' ')));
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
                removeLeadingBoolean(compiledHavings.join(' ')));
}

QString Grammar::compileHaving(const HavingConditionItem &having) const
{
    switch (having.type) {
    case HavingType::BASIC:
        return QStringLiteral("%1 %2 %3 ?").arg(having.condition, wrap(having.column),
                                                having.comparison);
    default:
        throw RuntimeError(QStringLiteral("Unknown HavingType (%1).")
                           .arg(static_cast<int>(having.type)));
    }
}

QString Grammar::whereBasic(const WhereConditionItem &where) const
{
    // BUG whereBasic look Eloquent silverqx
    return QStringLiteral("%1 %2 ?").arg(wrap(where.column),
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
    return QStringLiteral("%1 %2 %3").arg(wrap(where.column),
                                          where.comparison,
                                          // TODO add data member to the WhereConditionItem for second column silverqx
                                          wrap(where.value.value<QString>()));
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

QString Grammar::compileOrders(const QueryBuilder &query) const
{
    if (query.getOrders().isEmpty())
        return "";

    return QStringLiteral("order by %1").arg(compileOrdersToVector(query).join(", "));
}

QStringList Grammar::compileOrdersToVector(const QueryBuilder &query) const
{
    const auto &orders = query.getOrders();

    QStringList compiledOrders;
    compiledOrders.reserve(orders.size());

    for (const auto &order : orders)
        compiledOrders << QStringLiteral("%1 %2")
                          .arg(wrap(order.column), order.direction.toLower());

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

    return "";
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
        compiledParameters << QStringLiteral("(%1)").arg(parametrize(valuesMap));

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

    return compiledAssignments.join(", ");
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

        result += segment + QChar(' ');
    }

    return result.trimmed();
}

QString Grammar::removeLeadingBoolean(QString statement) const
{
    return statement.replace(
                QRegularExpression(QStringLiteral("^(and |or )"),
                                   QRegularExpression::CaseInsensitiveOption),
                "");
}

} // namespace Orm::Query::Grammars
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
