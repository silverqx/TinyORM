#pragma once
#ifndef ORM_EXCEPTIONS_SQLERROR_HPP
#define ORM_EXCEPTIONS_SQLERROR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QtSql/QSqlError>

#include "orm/exceptions/runtimeerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Exceptions
{

    /*! TinyORM SqlError exception, wrapper for the QSqlError. TinyOrm library compiled
        against the TinyDrivers doesn't use this exception class. */
    class TINYORM_EXPORT SqlError : public RuntimeError // clazy:exclude=copyable-polymorphic
    {
    public:
        /*! const char * constructor. */
        SqlError(const char *message, const QSqlError &error);
        /*! QString constructor. */
        SqlError(const QString &message, const QSqlError &error);

        /*! const char * constructor. */
        SqlError(const char *message, QSqlError &&error);
        /*! QString constructor. */
        SqlError(const QString &message, QSqlError &&error);

        /*! Get the original Qt SQL error. */
        inline const QSqlError &getSqlError() const noexcept;

        /*! Get the database-specific error code (shortcut method). */
        inline QString nativeErrorCode() const noexcept;
        /*! Get the database-specific error message (shortcut method). */
        inline QString databaseText() const noexcept;

    protected:
        /*! Protected converting constructor for use by descendants to avoid an error
            message formatting. */
        SqlError(const QString &message, const QSqlError &error, int /*unused*/);

        /*! Format the Qt SQL error message. */
        static QString formatMessage(const char *message, const QSqlError &error);

        /*! The Qt SQL error instance. */
        QSqlError m_sqlError;
    };

    /* public */

    const QSqlError &SqlError::getSqlError() const noexcept
    {
        return m_sqlError;
    }

    QString SqlError::nativeErrorCode() const noexcept
    {
        return m_sqlError.nativeErrorCode();
    }

    QString SqlError::databaseText() const noexcept
    {
        return m_sqlError.databaseText();
    }

} // namespace Orm::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_EXCEPTIONS_SQLERROR_HPP
