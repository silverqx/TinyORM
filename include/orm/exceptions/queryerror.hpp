#pragma once
#ifndef ORM_EXCEPTIONS_QUERYERROR_HPP
#define ORM_EXCEPTIONS_QUERYERROR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QVariant>

#include "orm/exceptions/sqlerror.hpp"

class QSqlQuery;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Exceptions
{

    /*! TinyORM database query exception. TinyOrm library compiled against the TinyDrivers
        doesn't use this exception class. */
    class TINYORM_EXPORT QueryError : public SqlError // clazy:exclude=copyable-polymorphic
    {
    public:
        /*! const char * constructor. */
        QueryError(QString connectionName, const char *message, const QSqlQuery &query,
                   const QList<QVariant> &bindings = {});
        /*! QString constructor. */
        QueryError(QString connectionName, const QString &message, const QSqlQuery &query,
                   const QList<QVariant> &bindings = {});

        /*! Get the connection name for the query. */
        inline const QString &getConnectionName() const noexcept;
        /*! Get the SQL for the query. */
        inline const QString &getSql() const noexcept;
        /*! Get the bindings for the query. */
        inline const QList<QVariant> &getBindings() const noexcept;

    protected:
        /*! Format the Qt SQL error message. */
        static QString formatMessage(const QString &connectionName, const char *message,
                                     const QSqlQuery &query);

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

} // namespace Orm::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_EXCEPTIONS_QUERYERROR_HPP
