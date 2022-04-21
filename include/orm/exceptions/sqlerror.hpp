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

    /*! TinyORM SqlError exception, wrapper for the QSqlError. */
    class SHAREDLIB_EXPORT SqlError : public RuntimeError
    {
    public:
        /*! const char * constructor. */
        SqlError(const char *message, const QSqlError &error);
        /*! QString constructor. */
        SqlError(const QString &message, const QSqlError &error);

        /*! Get the original Qt SQL error. */
        inline const QSqlError &getSqlError() const noexcept;

    protected:
        /*! Internal ctor for use from descendants to avoid an error message
            formatting. */
        SqlError(const QString &message, const QSqlError &error, int /*unused*/);

        /*! Format the Qt SQL error message. */
        QString formatMessage(const char *message, const QSqlError &error) const;

        /*! The Qt SQL error instance. */
        QSqlError m_sqlError;
    };

    const QSqlError &SqlError::getSqlError() const noexcept
    {
        return m_sqlError;
    }

} // namespace Orm::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_EXCEPTIONS_SQLERROR_HPP
