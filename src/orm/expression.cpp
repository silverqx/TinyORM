#include "expression.h"

#ifdef MANGO_COMMON_NAMESPACE
namespace MANGO_COMMON_NAMESPACE
{
#endif
namespace Orm
{

Expression::Expression(const QVariant &value)
    : m_value(value)
{}

Expression::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

} // namespace Orm
#ifdef MANGO_COMMON_NAMESPACE
} // namespace MANGO_COMMON_NAMESPACE
#endif
