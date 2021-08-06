#pragma once
#ifndef CONNECTIONRESOLVERINTERFACE_H
#define CONNECTIONRESOLVERINTERFACE_H

#include <QString>

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{
    class ConnectionInterface;

    /*! Database connection resolver interface. */
    class ConnectionResolverInterface
    {
    public:
        /*! Pure virtual destructor. */
        virtual ~ConnectionResolverInterface() = 0;

        /*! Get a database connection instance. */
        virtual ConnectionInterface &connection(const QString &name = "") = 0;

        /*! Get the default connection name. */
        virtual const QString &getDefaultConnection() const = 0;

        /*! Set the default connection name. */
        virtual void setDefaultConnection(const QString &defaultConnection) = 0;
    };

    inline ConnectionResolverInterface::~ConnectionResolverInterface() = default;

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // CONNECTIONRESOLVERINTERFACE_H
