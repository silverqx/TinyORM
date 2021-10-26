#pragma once
#ifndef ORM_HASCONNECTIONRESOLVER_HPP
#define ORM_HASCONNECTIONRESOLVER_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
    class ConnectionResolverInterface;

namespace Concerns
{

    /*! Connection resolver. */
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

} // namespace Concerns
} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_HASCONNECTIONRESOLVER_HPP
