#include "orm/concerns/hasconnectionresolver.hpp"

#include <QtGlobal>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Concerns
{

/* Hide the g_resolver pointer like in the guardedmodel.cpp to avoid different memory
   addreses for the exe and dll, look also the note in the guardedmodel.cpp.
   It also doesn't have to be tagged as the "thread_local" because it points
   to the DatabaseManager and only one instance of the DatabaseManager is allowed
   per whole application, so it will exist only in one thread. */

namespace
{
    /*! The connection resolver instance, atomic. */
    std::atomic<ConnectionResolverInterface *> &g_resolver() noexcept
    {
        static std::atomic<ConnectionResolverInterface *> cached = nullptr;

        return cached;
    }
} // namespace

/* public */

ConnectionResolverInterface *HasConnectionResolver::getConnectionResolver() noexcept
{
    Q_ASSERT(g_resolver() != nullptr);

    return g_resolver();
}

void HasConnectionResolver::setConnectionResolver(
        ConnectionResolverInterface *resolver) noexcept
{
    g_resolver() = resolver;
}

void HasConnectionResolver::unsetConnectionResolver() noexcept
{
    g_resolver() = nullptr;
}

} // namespace Orm::Concerns

TINYORM_END_COMMON_NAMESPACE
