#include "orm/query/expression.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Query
{

/*
   I don't need QString ctor because QString will be immediately converted
   to the QVariant in this constructor.
   m_value can not be QString because the Query::Expression still can hold eg. int,
   uint, QDateTime, and all other supported types, this is not fully true, it could be
   the QString, an explanation is below.
   BUT, m_value is always converted to the QString in Grammar and appended to the query
   string, anyway I have decided to leave it QVariant because a user can write DB::raw(3)
   and not DB::raw(QString::number(3)), it is all about nicer API at the expense
   of performance.
*/

// NOLINTNEXTLINE(modernize-pass-by-value)
Expression::Expression(const QVariant &value)
    : m_value(value)
{}

Expression::Expression(QVariant &&value)
    : m_value(std::move(value))
{}

Expression::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

} // namespace Orm::Query

TINYORM_END_COMMON_NAMESPACE
