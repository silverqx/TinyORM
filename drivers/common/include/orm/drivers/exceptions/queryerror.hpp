#pragma once
#ifndef ORM_DRIVERS_EXCEPTIONS_QUERYERROR_HPP
#define ORM_DRIVERS_EXCEPTIONS_QUERYERROR_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QVariant>

#include "orm/drivers/exceptions/sqlerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers::Exceptions
{

    /*! TinyDrivers database query exception. */
    class TINYDRIVERS_EXPORT QueryError : public SqlError // clazy:exclude=copyable-polymorphic
    {
    public:
        /*! Deleted default constructor (needed if all drivers are disabled). */
        QueryError() = delete;

#ifdef TINYDRIVERS_MYSQL_DRIVER
        /*! const char * constructor. */
        QueryError(const QString &connectionName, const char *message,
                   MySqlErrorType &&mysqlError, const QString &query,
                   const QList<QVariant> &bindings = {});
        /*! QString constructor. */
        QueryError(const QString &connectionName, const QString &message,
                   MySqlErrorType &&mysqlError, const QString &query,
                   const QList<QVariant> &bindings = {});
#endif

        /*! Get the connection name for the query. */
        inline const QString &getConnectionName() const noexcept;
        /*! Get the SQL for the query. */
        inline const QString &getSql() const noexcept;
        /*! Get the bindings for the query. */
        inline const QList<QVariant> &getBindings() const noexcept;

    protected:
#ifdef TINYDRIVERS_MYSQL_DRIVER
        /*! Format the MySQL error message. */
        static QString
        formatMessage(const QString &connectionName, const char *message,
                      const MySqlErrorType &mysqlError, const QString &query,
                      const QList<QVariant> &bindings);
#endif

        /*! The database connection name. */
        QString m_connectionName;
        /*! The SQL for the query. */
        QString m_sql;
        /*! The bindings for the query. */
        QList<QVariant> m_bindings;
    };

    /* public */

    const QString &QueryError::getConnectionName() const noexcept
    {
        return m_connectionName;
    }

    const QString &QueryError::getSql() const noexcept
    {
        return m_sql;
    }

    const QList<QVariant> &QueryError::getBindings() const noexcept
    {
        return m_bindings;
    }

} // namespace Orm::Drivers::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_EXCEPTIONS_QUERYERROR_HPP
