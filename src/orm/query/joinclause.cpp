#include "joinclause.h"

#ifdef MANGO_COMMON_NAMESPACE
namespace MANGO_COMMON_NAMESPACE
{
#endif
namespace Orm::Query
{

JoinClause::JoinClause(const Builder &query, const QString &type, const QString &table)
    : Builder(query.getConnection(), query.getGrammar())
    , m_type(type)
    , m_table(table)
{}

JoinClause &JoinClause::on(const QString &first, const QString &comparison,
                           const QString &second, const QString &condition)
{
    whereColumn(first, comparison, second, condition);
    return *this;
}

} // namespace Orm
#ifdef MANGO_COMMON_NAMESPACE
} // namespace MANGO_COMMON_NAMESPACE
#endif
