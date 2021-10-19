#include "orm/concerns/hasconnectionresolver.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Concerns
{

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

TINYORM_END_COMMON_NAMESPACE
