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
        /*! The connection resolver instance. */
        inline static ConnectionResolverInterface *m_resolver = nullptr;
    };

} // namespace Orm::Concerns
} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // HASCONNECTIONRESOLVER_H
