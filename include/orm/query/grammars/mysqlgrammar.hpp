#ifndef MYSQLGRAMMAR_H
#define MYSQLGRAMMAR_H

#include "orm/query/grammars/grammar.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Query::Grammars
{

    class SHAREDLIB_EXPORT MySqlGrammar : public Grammar
    {
        Q_DISABLE_COPY(MySqlGrammar)

    public:
        MySqlGrammar() = default;

        /*! Compile an insert statement into SQL. */
        QString compileInsert(const QueryBuilder &query,
                              const QVector<QVariantMap> &values) const override;
        /*! Compile an insert ignore statement into SQL. */
        QString compileInsertOrIgnore(const QueryBuilder &query,
                                      const QVector<QVariantMap> &values) const override;

        /*! Compile the lock into SQL. */
        QString compileLock(const QueryBuilder &query) const override;

        /*! Get the grammar specific operators. */
        const QVector<QString> &getOperators() const override;

    protected:
        /*! Compile an update statement without joins into SQL. */
        QString
        compileUpdateWithoutJoins(
                const QueryBuilder &query, const QString &table,
                const QString &columns, const QString &wheres) const override;

        /*! Compile a delete statement without joins into SQL. */
        QString
        compileDeleteWithoutJoins(const QueryBuilder &query, const QString &table,
                                  const QString &wheres) const override;

        /*! Wrap a single string in keyword identifiers. */
        QString wrapValue(QString value) const override;

        /*! Map the ComponentType to a Grammar::compileXx() methods. */
        const QMap<SelectComponentType, SelectComponentValue> &
        getCompileMap() const override;
        /*! Map the WhereType to a Grammar::whereXx() methods. */
        const std::function<QString(const WhereConditionItem &)> &
        getWhereMethod(WhereType whereType) const override;
    };

} // namespace Orm::Query::Grammars
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // MYSQLGRAMMAR_H
