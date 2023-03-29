#include "orm/query/joinclause.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Query
{

// NOLINTNEXTLINE(modernize-pass-by-value)
JoinClause::JoinClause(const Builder &query, const QString &type, const QString &table)
    : Builder(query.getConnectionShared(), query.getGrammarShared())
    , m_type(type)
    , m_table(table)
{}

// NOLINTNEXTLINE(modernize-pass-by-value)
JoinClause::JoinClause(const Builder &query, const QString &type, const Expression &table)
    : Builder(query.getConnectionShared(), query.getGrammarShared())
    , m_type(type)
    , m_table(table)
{}

// NOLINTNEXTLINE(modernize-pass-by-value)
JoinClause::JoinClause(const Builder &query, const QString &type, Expression &&table)
    : Builder(query.getConnectionShared(), query.getGrammarShared())
    , m_type(type)
    , m_table(std::move(table))
{}

// NOLINTNEXTLINE(modernize-pass-by-value)
JoinClause::JoinClause(const Builder &query, const QString &type, const JoinTable &table)
    : Builder(query.getConnectionShared(), query.getGrammarShared())
    , m_type(type)
    , m_table(table)
{}

JoinClause &JoinClause::on(const QString &first, const QString &comparison,
                           const QString &second, const QString &condition)
{
    /* On clauses can be chained, e.g.

       join.on("contacts.user_id", "=", "users.id")
           .on("contacts.info_id", "=", "info.id")

       will produce the following SQL:

       on `contacts`.`user_id` = `users`.`id` and `contacts`.`info_id` = `info`.`id` */

    whereColumn(first, comparison, second, condition);

    return *this;
}

// NOTE api different silverqx
JoinClause &
JoinClause::orOn(const QString &first, const QString &comparison,
                 const QString &second)
{
    return on(first, comparison, second, OR);
}

std::shared_ptr<Builder> JoinClause::newQuery() const
{
    return std::make_shared<JoinClause>(*this, m_type, m_table);
}

std::shared_ptr<Builder> JoinClause::forSubQuery() const
{
    return Builder::newQuery();
}

} // namespace Orm::Query

TINYORM_END_COMMON_NAMESPACE
