#ifndef GRAMMAR_H
#define GRAMMAR_H

#include "orm/basegrammar.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Query::Grammars
{

    // FEATURE savepoints in grammar silverqx
    class SHAREDLIB_EXPORT Grammar : public BaseGrammar
    {
        Q_DISABLE_COPY(Grammar)

    public:
        Grammar() = default;

        /*! Compile a select query into SQL. */
        QString compileSelect(QueryBuilder &query) const;

        /*! Compile an insert statement into SQL. */
        virtual QString
        compileInsert(const QueryBuilder &query,
                      const QVector<QVariantMap> &values) const;
        /*! Compile an insert ignore statement into SQL. */
        virtual QString
        compileInsertOrIgnore(const QueryBuilder &query,
                              const QVector<QVariantMap> &values) const;
        // FEATURE postgres, sequence silverqx
        /*! Compile an insert and get ID statement into SQL. */
        inline QString
        compileInsertGetId(const QueryBuilder &query,
                           const QVector<QVariantMap> &values) const
        { return compileInsert(query, values); }

        /*! Compile an update statement into SQL. */
        virtual QString
        compileUpdate(QueryBuilder &query, const QVector<UpdateItem> &values) const;
        /*! Prepare the bindings for an update statement. */
        QVector<QVariant>
        prepareBindingsForUpdate(const BindingsMap &bindings,
                                 const QVector<UpdateItem> &values) const;

        /*! Compile a delete statement into SQL. */
        virtual QString compileDelete(QueryBuilder &query) const;
        /*! Prepare the bindings for a delete statement. */
        QVector<QVariant>
        prepareBindingsForDelete(const BindingsMap &bindings) const;

        /*! Compile a truncate table statement into SQL. Returns a map of
            the query string and bindings. */
        virtual std::unordered_map<QString, QVector<QVariant>>
        compileTruncate(const QueryBuilder &query) const;

        /*! Get the grammar specific operators. */
        virtual const QVector<QString> &getOperators() const;

    protected:
        /*! The components necessary for a select clause. */
        using SelectComponentsVector = QVector<QString>;
        /*! Select component types. */
        enum struct SelectComponentType
        {
             AGGREGATE,
             COLUMNS,
             FROM,
             JOINS,
             WHERES,
             GROUPS,
             HAVINGS,
             ORDERS,
             LIMIT,
             OFFSET,
             LOCK,
        };
        /*! The select component compile method and whether the component was set. */
        struct SelectComponentValue
        {
            /*! The component's compile method. */
            std::function<QString(const QueryBuilder &)> compileMethod;
            /*! Determine whether the component is set and is not empty. */
            std::function<bool(const QueryBuilder &)> isset;
        };

        /*! Map the ComponentType to a Grammar::compileXx() methods. */
        virtual const QMap<SelectComponentType, SelectComponentValue> &
        getCompileMap() const = 0;
        /*! Map the WhereType to a Grammar::whereXx() methods. */
        virtual const std::function<QString(const WhereConditionItem &)> &
        getWhereMethod(WhereType whereType) const = 0;

        /*! Compile the components necessary for a select clause. */
        SelectComponentsVector compileComponents(const QueryBuilder &query) const;

        /*! Compile the "select *" portion of the query. */
        QString compileColumns(const QueryBuilder &query) const;
        /*! Compile the "from" portion of the query. */
        QString compileFrom(const QueryBuilder &query) const;
        /*! Compile the "join" portions of the query. */
        QString compileJoins(const QueryBuilder &query) const;
        /*! Compile the "where" portions of the query. */
        QString compileWheres(const QueryBuilder &query) const;
        /*! Get the vector of all the where clauses for the query. */
        QVector<QString> compileWheresToVector(const QueryBuilder &query) const;
        /*! Format the where clause statements into one string. */
        QString concatenateWhereClauses(const QueryBuilder &query,
                                        const QVector<QString> &sql) const;
        /*! Compile the "group by" portions of the query. */
        QString compileGroups(const QueryBuilder &query) const;
        /*! Compile the "having" portions of the query. */
        QString compileHavings(const QueryBuilder &query) const;
        /*! Compile a single having clause. */
        QString compileHaving(const HavingConditionItem &having) const;

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

        /*! Compile the "order by" portions of the query. */
        QString compileOrders(const QueryBuilder &query) const;
        /*! Compile the query orders to the vector. */
        QVector<QString> compileOrdersToVector(const QueryBuilder &query) const;
        /*! Compile the "limit" portions of the query. */
        QString compileLimit(const QueryBuilder &query) const;
        /*! Compile the "offset" portions of the query. */
        QString compileOffset(const QueryBuilder &query) const;

        /*! Compile the lock into SQL. */
        virtual QString compileLock(const QueryBuilder &query) const;

        /*! Compile a insert values lists. */
        QVector<QString>
        compileInsertToVector(const QVector<QVariantMap> &values) const;

        /*! Compile the columns for an update statement. */
        virtual QString
        compileUpdateColumns(const QVector<UpdateItem> &values) const;
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

        /*! Remove the leading boolean from a statement. */
        QString removeLeadingBoolean(QString statement) const;
    };

} // namespace Orm::Query::Grammars
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // GRAMMAR_H
