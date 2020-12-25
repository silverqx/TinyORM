#ifndef JOINCLAUSE_H
#define JOINCLAUSE_H

#include "orm/query/querybuilder.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{
    class DatabaseConnection;
    class Grammar;
}

namespace Orm::Query
{
    class JoinClause final : public Builder
    {
    public:
        JoinClause(const Builder &query, const QString &type, const QString &table);

        JoinClause &on(const QString &first, const QString &comparison,
                       const QString &second, const QString &condition = "and");

        const QString &getType() const
        { return m_type; }
        const QString &getTable() const
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
