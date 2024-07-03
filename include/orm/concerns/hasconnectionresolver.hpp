#pragma once
#ifndef ORM_CONCERNS_HASCONNECTIONRESOLVER_HPP
#define ORM_CONCERNS_HASCONNECTIONRESOLVER_HPP

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
    class TINYORM_EXPORT HasConnectionResolver
    {
    public:
        /*! Get the connection resolver instance. */
        static ConnectionResolverInterface *getConnectionResolver() noexcept;

        /*! Set the connection resolver instance. */
        static void setConnectionResolver(ConnectionResolverInterface *resolver) noexcept;

        /*! Unset the connection resolver for models. */
        static void unsetConnectionResolver() noexcept;
    };

} // namespace Concerns
} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONCERNS_HASCONNECTIONRESOLVER_HPP
