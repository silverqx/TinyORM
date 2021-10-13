#pragma once
#ifndef JOINCLAUSE_HPP
#define JOINCLAUSE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/query/querybuilder.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Query
{

    /*! Join clause. */
    class SHAREDLIB_EXPORT JoinClause final : public Builder
    {
    public:
        /*! Join table type. */
        using JoinTable = FromClause;

        /*! Constructor with the table as QString. */
        JoinClause(const Builder &query, const QString &type, const QString &table);
        /*! Constructor with the table as Expression. */
        JoinClause(const Builder &query, const QString &type, const Expression &table);
        /*! Constructor with the table as Expression. */
        JoinClause(const Builder &query, const QString &type, Expression &&table);
        /*! Constructor with the table as Expression. */
        JoinClause(const Builder &query, const QString &type, const JoinTable &table);

        /*! Add an "on" clause to the join. */
        JoinClause &on(const QString &first, const QString &comparison,
                       const QString &second, const QString &condition = AND);
        /*! Add an "or on" clause to the join. */
        JoinClause &orOn(const QString &first, const QString &comparison,
                         const QString &second);

        /*! Get the type of join being performed. */
        const QString &getType() const;
        /*! Get the table the join clause is joining to. */
        const std::variant<std::monostate, QString, Expression> &
        getTable() const;

        /*! Get a new instance of the join clause builder. */
        QSharedPointer<Builder> newQuery() const override;

    protected:
        /*! Create a new query instance for a sub-query. */
        QSharedPointer<Builder> forSubQuery() const override;

    private:
        /*! The type of join being performed. */
        const QString m_type;
        /*! The table the join clause is joining to. */
        const JoinTable m_table;
    };

    inline const QString &
    JoinClause::getType() const
    {
        return m_type;
    }

    inline const std::variant<std::monostate, QString, Expression> &
    JoinClause::getTable() const
    {
        return m_table;
    }

} // namespace Orm::Query
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // JOINCLAUSE_HPP
