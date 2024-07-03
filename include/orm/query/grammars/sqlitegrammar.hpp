#pragma once
#ifndef ORM_QUERY_GRAMMARS_SQLITEGRAMMAR_HPP
#define ORM_QUERY_GRAMMARS_SQLITEGRAMMAR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/query/grammars/grammar.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Query::Grammars
{

    /*! SQLite grammar. */
    class TINYORM_EXPORT SQLiteGrammar : public Grammar
    {
        Q_DISABLE_COPY_MOVE(SQLiteGrammar)

    public:
        /*! Default constructor. */
        SQLiteGrammar() = default;
        /*! Virtual destructor. */
        ~SQLiteGrammar() override = default;

        /*! Compile an insert ignore statement into SQL. */
        QString compileInsertOrIgnore(const QueryBuilder &query,
                                      const QList<QVariantMap> &values) const override;

        /*! Compile an update statement into SQL. */
        QString compileUpdate(QueryBuilder &query,
                              const QList<UpdateItem> &values) const override;
        /*! Compile an "upsert" statement into SQL. */
        QString compileUpsert(
                    QueryBuilder &query, const QList<QVariantMap> &values,
                    const QStringList &uniqueBy,
                    const QStringList &update) const override;

        /*! Compile a delete statement into SQL. */
        QString compileDelete(QueryBuilder &query) const override;

        /*! Compile a truncate table statement into SQL. Returns a map of
            the query string and bindings. */
        std::unordered_map<QString, QList<QVariant>>
        compileTruncate(const QueryBuilder &query) const override;

        /*! Compile the lock into SQL. */
        QString compileLock(const QueryBuilder &query) const override;

        /*! Get the grammar specific operators. */
        const std::unordered_set<QString> &getOperators() const override;

    protected:
        /*! Map the ComponentType to a Grammar::compileXx() methods. */
        const QList<SelectComponentValue> &getCompileMap() const override;
        /*! Map the WhereType to a Grammar::whereXx() methods. */
        const WhereMemFn &getWhereMethod(WhereType whereType) const override;

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
        QString dateBasedWhere(const QString &type,
                               const WhereConditionItem &where) const override;
        /*! Column for a date based where clause based on the type (uses strftime()). */
        QString dateBasedWhereColumn(const QString &type,
                                     const WhereConditionItem &where) const;

        /*! Compile the columns for an update statement. */
        QString compileUpdateColumns(const QList<UpdateItem> &values) const override;

    private:
        /*! Compile an update statement with joins or limit into SQL. */
        QString compileUpdateWithJoinsOrLimit(QueryBuilder &query,
                                              const QList<UpdateItem> &values) const;

        /*! Compile a delete statement with joins or limit into SQL. */
        QString compileDeleteWithJoinsOrLimit(QueryBuilder &query) const;
    };

} // namespace Orm::Query::Grammars

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_QUERY_GRAMMARS_SQLITEGRAMMAR_HPP
