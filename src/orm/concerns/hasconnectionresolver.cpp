#include "orm/concerns/hasconnectionresolver.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Concerns
{

ConnectionResolverInterface *HasConnectionResolver::m_resolver = nullptr;

ConnectionResolverInterface *HasConnectionResolver::getConnectionResolver()
{
    return m_resolver;
}

void HasConnectionResolver::setConnectionResolver(ConnectionResolverInterface *resolver)
{
    m_resolver = resolver;
}

void HasConnectionResolver::unsetConnectionResolver()
{
    m_resolver = nullptr;
}

} // namespace Orm::Concerns
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
