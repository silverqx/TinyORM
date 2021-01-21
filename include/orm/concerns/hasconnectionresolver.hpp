#ifndef HASCONNECTIONRESOLVER_H
#define HASCONNECTIONRESOLVER_H

#include "export.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{
    class ConnectionResolverInterface;

namespace Concerns
{

    class SHAREDLIB_EXPORT HasConnectionResolver
    {
    public:
        /*! Get the connection resolver instance. */
        static ConnectionResolverInterface *getConnectionResolver();

        /*! Set the connection resolver instance. */
        static void setConnectionResolver(ConnectionResolverInterface *resolver);

        /*! Unset the connection resolver for models. */
        static void unsetConnectionResolver();

    protected:
        // WARNING if the m_resolver is inline static, that it is nullptr even after initialization from DatabaseManager ctor, examine why this happen and how it works silverqx
        /*! The connection resolver instance. */
        static ConnectionResolverInterface *m_resolver;
    };

} // namespace Orm::Concerns
} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // HASCONNECTIONRESOLVER_H
