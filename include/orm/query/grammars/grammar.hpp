#pragma once
#ifndef ORM_QUERY_GRAMMARS_GRAMMAR_HPP
#define ORM_QUERY_GRAMMARS_GRAMMAR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <optional>
#include <unordered_set>

#include "orm/basegrammar.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Query::Grammars
{

    // FEATURE savepoints in grammar silverqx
    /*! Sql grammar base class. */
    class TINYORM_EXPORT Grammar : public BaseGrammar
    {
        Q_DISABLE_COPY_MOVE(Grammar)

    public:
        /*! Default constructor. */
        Grammar() = default;
        /*! Pure virtual destructor. */
        inline ~Grammar() override = 0;

        /*! Compile a select query into SQL. */
        QString compileSelect(QueryBuilder &query) const;

        /*! Compile an exists statement into SQL. */
        QString compileExists(QueryBuilder &query) const;

        /*! Compile an insert statement into SQL. */
        virtual QString
        compileInsert(const QueryBuilder &query,
                      const QList<QVariantMap> &values) const;
        /*! Compile an insert ignore statement into SQL. */
        virtual QString
        compileInsertOrIgnore(const QueryBuilder &query,
                              const QList<QVariantMap> &values) const;
        /*! Compile an insert and get ID statement into SQL. */
        inline virtual QString
        compileInsertGetId(const QueryBuilder &query,
                           const QList<QVariantMap> &values,
                           const QString &sequence) const;

        /*! Compile an update statement into SQL. */
        virtual QString
        compileUpdate(QueryBuilder &query, const QList<UpdateItem> &values) const;
        /*! Prepare the bindings for an update statement. */
        static QList<QVariant>
        prepareBindingsForUpdate(const BindingsMap &bindings,
                                 const QList<UpdateItem> &values);

        /*! Compile an "upsert" statement into SQL. */
        virtual QString
        compileUpsert(QueryBuilder &query, const QList<QVariantMap> &values,
                      const QStringList &uniqueBy, const QStringList &update) const;

        /*! Compile a delete statement into SQL. */
        virtual QString compileDelete(QueryBuilder &query) const;
        /*! Prepare the bindings for a delete statement. */
        static QList<QVariant> prepareBindingsForDelete(const BindingsMap &bindings);

        /*! Compile a truncate table statement into SQL. Returns a map of
            the query string and bindings. */
        virtual std::unordered_map<QString, QList<QVariant>>
        compileTruncate(const QueryBuilder &query) const;

        /*! Compile the random statement into SQL. */
        virtual QString compileRandom(const QString &seed) const;

        /*! Get the grammar specific operators. */
        virtual const std::unordered_set<QString> &getOperators() const;

    protected:
        /*! The select component compile method and whether the component was set. */
        struct SelectComponentValue
        {
            /*! The component's compile method. */
            std::function<QString(const Grammar &, const QueryBuilder &)> compileMethod;
            /*! Determine whether the component is set and is not empty. */
            std::function<bool(const QueryBuilder &)> isset;
        };
        /*! Alias type for the whereXx() methods. */
        using WhereMemFn = std::function<QString(const Grammar &grammar,
                                                 const WhereConditionItem &)>;

        /*! Map the ComponentType to a Grammar::compileXx() methods. */
        virtual const QList<SelectComponentValue> &getCompileMap() const = 0;
        /*! Map the WhereType to a Grammar::whereXx() methods. */
        virtual const WhereMemFn &getWhereMethod(WhereType whereType) const = 0;

        /*! Determine whether the 'aggregate' component should be compiled. */
        static bool shouldCompileAggregate(const std::optional<AggregateItem> &aggregate);
        /*! Determine whether the 'columns' component should be compiled. */
        static bool shouldCompileColumns(const QueryBuilder &query);
        /*! Determine whether the 'from' component should be compiled. */
        static bool shouldCompileFrom(const std::variant<std::monostate, QString,
                                      Query::Expression> &from);

        /*! Compile the components necessary for a select clause. */
        QStringList compileComponents(const QueryBuilder &query) const;

        /*! Compile an aggregated select clause. */
        QString compileAggregate(const QueryBuilder &query) const;
        /*! Compile the "select *" portion of the query. */
        virtual QString compileColumns(const QueryBuilder &query) const;

        /*! Compile the "from" portion of the query. */
        QString compileFrom(const QueryBuilder &query) const;

        /*! Compile the "where" portions of the query. */
        QString compileWheres(const QueryBuilder &query) const;
        /*! Get the vector of all the where clauses for the query. */
        QStringList compileWheresToVector(const QueryBuilder &query) const;
        /*! Format the where clause statements into one string. */
        static QString concatenateWhereClauses(const QueryBuilder &query,
                                               const QStringList &sql);

        /*! Compile the "join" portions of the query. */
        QString compileJoins(const QueryBuilder &query) const;

        /*! Compile the "group by" portions of the query. */
        QString compileGroups(const QueryBuilder &query) const;

        /*! Compile the "having" portions of the query. */
        QString compileHavings(const QueryBuilder &query) const;
        /*! Compile a single having clause. */
        QString compileHaving(const HavingConditionItem &having) const;
        /*! Compile a basic having clause. */
        QString compileBasicHaving(const HavingConditionItem &having) const;

        /*! Compile the "order by" portions of the query. */
        QString compileOrders(const QueryBuilder &query) const;
        /*! Compile the query orders to the vector. */
        QStringList compileOrdersToVector(const QueryBuilder &query) const;
        /*! Compile the "limit" portions of the query. */
        QString compileLimit(const QueryBuilder &query) const;
        /*! Compile the "offset" portions of the query. */
        QString compileOffset(const QueryBuilder &query) const;

        /*! Compile the lock into SQL. */
        virtual QString compileLock(const QueryBuilder &query) const;

        /*! Compile a basic where clause. */
        QString whereBasic(const WhereConditionItem &where) const;
        /*! Compile a nested where clause. */
        QString whereNested(const WhereConditionItem &where) const;
        /*! Compile a where clause comparing two columns. */
        QString whereColumn(const WhereConditionItem &where) const;
        /*! Compile a "where in" clause. */
        QString whereIn(const WhereConditionItem &where) const;
        /*! Compile a "where not in" clause. */
        QString whereNotIn(const WhereConditionItem &where) const;
        /*! Compile a "where null" clause. */
        QString whereNull(const WhereConditionItem &where) const;
        /*! Compile a "where not null" clause. */
        QString whereNotNull(const WhereConditionItem &where) const;
        /*! Compile a raw where clause. */
        QString whereRaw(const WhereConditionItem &where) const;
        /*! Compile a "where exists" clause. */
        QString whereExists(const WhereConditionItem &where) const;
        /*! Compile a "where not exists" clause. */
        QString whereNotExists(const WhereConditionItem &where) const;
        /*! Compile a where row values condition. */
        QString whereRowValues(const WhereConditionItem &where) const;
        /*! Compile a "between" where clause. */
        QString whereBetween(const WhereConditionItem &where) const;
        /*! Compile a "between" where clause using columns. */
        QString whereBetweenColumns(const WhereConditionItem &where) const;

        /*! Compile a "where date" clause. */
        QString whereDate(const WhereConditionItem &where) const;
        /*! Compile a "where time" clause. */
        QString whereTime(const WhereConditionItem &where) const;
        /*! Compile a "where day" clause. */
        QString whereDay(const WhereConditionItem &where) const;
        /*! Compile a "where month" clause. */
        QString whereMonth(const WhereConditionItem &where) const;
        /*! Compile a "where year" clause. */
        QString whereYear(const WhereConditionItem &where) const;
        /*! Compile a date based where clause. */
        virtual QString
        dateBasedWhere(const QString &type, const WhereConditionItem &where) const;

        /*! Compile a insert values lists. */
        QStringList compileInsertToVector(const QList<QVariantMap> &values) const;

        /*! Compile the columns for an update statement. */
        virtual QString
        compileUpdateColumns(const QList<UpdateItem> &values) const;
        /*! Compile an update statement without joins into SQL. */
        virtual QString
        compileUpdateWithoutJoins(const QueryBuilder &query, const QString &table,
                                  const QString &columns, const QString &wheres) const;
        /*! Compile an update statement with joins into SQL. */
        QString
        compileUpdateWithJoins(const QueryBuilder &query, const QString &table,
                               const QString &columns, const QString &wheres) const;

        /*! Compile a delete statement without joins into SQL. */
        virtual QString
        compileDeleteWithoutJoins(const QueryBuilder &query, const QString &table,
                                  const QString &wheres) const;
        /*! Compile a delete statement with joins into SQL. */
        QString
        compileDeleteWithJoins(const QueryBuilder &query, const QString &table,
                               const QString &wheres) const;

        /*! Concatenate an array of segments, removing empties. */
        static QString concatenate(const QStringList &segments);
        /*! Remove the leading boolean from a statement. */
        static QString removeLeadingBoolean(QString &&statement);

        /*! Flat bindings map and exclude given binding types. */
        static QList<std::reference_wrapper<const QVariant>>
        flatBindingsForUpdateDelete(const BindingsMap &bindings,
                                    const QList<BindingType> &exclude);

        /*! Compute the reserve size for the BindingsMap. */
        static QList<QVariant>::size_type
        computeReserveForBindingsMap(const BindingsMap &bindings,
                                     const QList<BindingType> &exclude = {});
    };

    /* public */

    Grammar::~Grammar() = default;

    QString Grammar::compileInsertGetId(
            const QueryBuilder &query, const QList<QVariantMap> &values,
            const QString &/*unused*/) const
    {
        return compileInsert(query, values);
    }

} // namespace Orm::Query::Grammars

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_QUERY_GRAMMARS_GRAMMAR_HPP
