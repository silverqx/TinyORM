#ifndef GRAMMAR_H
#define GRAMMAR_H

#include "export.hpp"
#include "orm/ormtypes.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

    class SHAREDLIB_EXPORT Grammar
    {
    public:
        /*! Compile a select query into SQL. */
        QString compileSelect(QueryBuilder &query) const;
        /*! Compile an insert statement into SQL. */
        QString compileInsert(const QueryBuilder &query, const QVector<QVariantMap> &values) const;
        /*! Compile an insert ignore statement into SQL. */
        QString compileInsertOrIgnore(const QueryBuilder &query,
                                      const QVector<QVariantMap> &values) const;
        // TODO postgres, sequence silverqx
        /*! Compile an insert and get ID statement into SQL. */
        inline QString
        compileInsertGetId(const QueryBuilder &query, const QVector<QVariantMap> &values) const
        { return compileInsert(query, values); }
        /*! Compile an update statement into SQL. */
        QString compileUpdate(const QueryBuilder &query, const QVector<UpdateItem> &values) const;
        /*! Prepare the bindings for an update statement. */
        QVector<QVariant> prepareBindingsForUpdate(const BindingsMap &bindings,
                                                   const QVector<UpdateItem> &values) const;
        /*! Compile a delete statement into SQL. */
        QString compileDelete(const QueryBuilder &query) const;
        /*! Prepare the bindings for a delete statement. */
        QVector<QVariant> prepareBindingsForDelete(const BindingsMap &bindings) const;
        /*! Compile a truncate table statement into SQL. */
        QString compileTruncate(const QueryBuilder &query) const;

        /*! Get the format for database stored dates. */
        const QString &getDateFormat() const;

    protected:
        // TODO methods below should be abstracted to DatabaseGrammar silverqx
        /*! Convert an array of column names into a delimited string. */
        QString columnize(const QStringList &columns) const;
        QString columnize(const QStringList &columns, bool isTorrentsTable) const;
        // TODO add template constraint to QVariantMap and QVector<QVariant> for now silverqx
        /*! Create query parameter place-holders for an array. */
        template<typename Container>
        QString parametrize(const Container &values) const;
        /*! Get the appropriate query parameter place-holder for a value. */
        QString parameter(const QVariant &value) const;

        /*! Remove the leading boolean from a statement. */
        QString removeLeadingBoolean(QString statement) const;

    private:
        using ComponentsVector = QVector<QString>;
        enum struct ComponentType
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
        struct ComponentValue
        {
            std::function<QString(const QueryBuilder &)> compileMethod;
            std::function<bool(const QueryBuilder &)> isset;
        };

        /*! Compile the components necessary for a select clause. */
        ComponentsVector compileComponents(const QueryBuilder &query) const;

        /*! Compile the "select *" portion of the query. */
        QString compileColumns(const QueryBuilder &query) const;
        QString compileFrom(const QueryBuilder &query) const;
        /*! Compile the "join" portions of the query. */
        QString compileJoins(const QueryBuilder &query) const;
        QString compileWheres(const QueryBuilder &query) const;
        QVector<QString> compileWheresToVector(const QueryBuilder &query) const;
        QString concatenateWhereClauses(const QueryBuilder &query, const QVector<QString> &sql) const;
        QString compileGroups(const QueryBuilder &query) const;
        QString compileHavings(const QueryBuilder &query) const;
        QString compileHaving(const HavingConditionItem &having) const;

        /*! Map the ComponentType to a Grammar::compileXx() methods. */
        const QMap<ComponentType, ComponentValue> &
        getCompileMap() const;
        /*! Map the WhereType to a Grammar::whereXx() methods. */
        const std::function<QString(const WhereConditionItem &)> &
        getWhereMethod(WhereType whereType) const;

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

        QString compileOrders(const QueryBuilder &query) const;
        QVector<QString> compileOrdersToVector(const QueryBuilder &query) const;
        QString compileLimit(const QueryBuilder &query) const;
        QString compileOffset(const QueryBuilder &query) const;

        /*! Compile a insert values lists. */
        QVector<QString> compileInsertToVector(const QVector<QVariantMap> &values) const;
        QString compileInsert(const QueryBuilder &query, const QVector<QVariantMap> &values,
                              bool ignore) const;

        /*! Compile the columns for an update statement. */
        QString compileUpdateColumns(const QVector<UpdateItem> &values) const;
        /*! Compile an update statement without joins into SQL. */
        QString compileUpdateWithoutJoins(const QString &table, const QString &columns,
                                          const QString &wheres) const;
        /*! Compile an update statement with joins into SQL. */
        QString compileUpdateWithJoins(const QueryBuilder &query, const QString &table,
                                       const QString &columns, const QString &wheres) const;

        /*! Compile a delete statement without joins into SQL. */
        QString compileDeleteWithoutJoins(const QString &table, const QString &wheres) const;
        /*! Compile a delete statement with joins into SQL. */
        QString compileDeleteWithJoins(const QueryBuilder &query, const QString &table,
                                       const QString &wheres) const;
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // GRAMMAR_H
