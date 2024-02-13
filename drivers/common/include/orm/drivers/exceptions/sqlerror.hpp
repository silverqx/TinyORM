#pragma once
#ifndef ORM_DRIVERS_EXCEPTIONS_SQLERROR_HPP
#define ORM_DRIVERS_EXCEPTIONS_SQLERROR_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "orm/drivers/exceptions/runtimeerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers::Exceptions
{

    /*! TinyDrivers SqlError exception, wrapper for the database-specific error. */
    class TINYDRIVERS_EXPORT SqlError : public RuntimeError // clazy:exclude=copyable-polymorphic
    {
    public:
        /*! Deleted default constructor (needed if all drivers are disabled). */
        SqlError() = delete;

#ifdef TINYDRIVERS_MYSQL_DRIVER
        /*! Prepare MySQL errNo and error message. */
        struct MySqlErrorType
        {
            /*! Prepared database-specific error code. */
            QString errNo {};
            /*! Prepared database-specific error message. */
            QString errorMessage {};
        };

        /*! Converting constructor for MySQL. */
        SqlError(const char *message, MySqlErrorType &&mysqlError);
        /*! Converting constructor for MySQL. */
        SqlError(const QString &message, MySqlErrorType &&mysqlError);
#endif // TINYDRIVERS_MYSQL_DRIVER

        /*! Get the database-specific error code. */
        inline QString nativeErrorCode() const noexcept;
        /*! Get the database-specific error message. */
        inline QString databaseText() const noexcept;

    protected:
#ifdef TINYDRIVERS_MYSQL_DRIVER
        /*! Protected converting constructor for use by descendants to avoid an error
            message formatting. */
        SqlError(const QString &message, MySqlErrorType &&mysqlError, int /*unused*/);

        /*! Format the MySQL error message. */
        static QString formatMessage(const char *message,
                                     const MySqlErrorType &mysqlError);
#endif

        /*! The database error code. */
        QString m_errorCode;
        /*! The database error message. */
        QString m_databaseText;
    };

    /* public */

    /* Don't return const & for the following two methods to be API compatible with
       the Orm::Exceptions::SqlError. */

    QString SqlError::nativeErrorCode() const noexcept
    {
        return m_errorCode;
    }

    QString SqlError::databaseText() const noexcept
    {
        return m_databaseText;
    }

} // namespace Orm::Drivers::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_EXCEPTIONS_SQLERROR_HPP
