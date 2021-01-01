#include "orm/query/expression.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Query
{

Expression::Expression(const QVariant &value)
    : m_value(value)
{}

Expression::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
