#pragma once
#ifndef ORM_CONNECTIONRESOLVERINTERFACE_HPP
#define ORM_CONNECTIONRESOLVERINTERFACE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>

#include "orm/macros/commonnamespace.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
    class DatabaseConnection;

    /*! Database connection resolver interface. */
    class ConnectionResolverInterface
    {
        Q_DISABLE_COPY_MOVE(ConnectionResolverInterface)

    public:
        /*! Default constructor. */
        ConnectionResolverInterface() = default;
        /*! Pure virtual destructor. */
        inline virtual ~ConnectionResolverInterface() = 0;

        /*! Get a database connection instance. */
        virtual DatabaseConnection &connection(const QString &name = "") = 0; // NOLINT(google-default-arguments)

        /*! Get the default connection name. */
        virtual const QString &getDefaultConnection() const = 0;

        /*! Set the default connection name. */
        virtual void setDefaultConnection(const QString &defaultConnection) = 0;

        /*! Reset the default connection name to a default value. */
        virtual void resetDefaultConnection() = 0;
    };

    /* public */

    ConnectionResolverInterface::~ConnectionResolverInterface() = default;

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONNECTIONRESOLVERINTERFACE_HPP
