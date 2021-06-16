#pragma once
#ifndef JOINCLAUSE_H
#define JOINCLAUSE_H

#include "orm/query/querybuilder.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Query
{

    class SHAREDLIB_EXPORT JoinClause final : public Builder
    {
    public:
        JoinClause(const Builder &query, const QString &type, const QString &table);

        /*! Add an "on" clause to the join. */
        JoinClause &on(const QString &first, const QString &comparison,
                       const QString &second, const QString &condition = "and");
        /*! Add an "or on" clause to the join. */
        JoinClause &orOn(const QString &first, const QString &comparison,
                         const QString &second);

        /*! Get the type of join being performed. */
        inline const QString &getType() const
        { return m_type; }
        /*! Get the table the join clause is joining to. */
        inline const QString &getTable() const
        { return m_table; }

    private:
        /*! The type of join being performed. */
        const QString m_type;
        /*! The table the join clause is joining to. */
        const QString m_table;
    };

} // namespace Orm::Query
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // JOINCLAUSE_H
