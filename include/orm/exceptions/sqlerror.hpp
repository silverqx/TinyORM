#pragma once
#ifndef ORM_SQLERROR_HPP
#define ORM_SQLERROR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QtSql/QSqlError>

#include "orm/exceptions/runtimeerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Exceptions
{

    /*! SqlError exception, wrapper for the QSqlError. */
    class SHAREDLIB_EXPORT SqlError : public RuntimeError
    {
    public:
        /*! const char * constructor. */
        SqlError(const char *message, const QSqlError &error);
        /*! QString constructor. */
        SqlError(const QString &message, const QSqlError &error);

        /*! Get the original Qt SQL error. */
        const QSqlError &getSqlError() const;

    protected:
        /*! Internal ctor for use from descendants to avoid an error message formatting. */
        SqlError(const QString &message, const QSqlError &error, int);

        /*! Format the Qt SQL error message. */
        QString formatMessage(const char *message, const QSqlError &error) const;

        /*! The Qt SQL error instance. */
        const QSqlError m_sqlError;
    };

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SQLERROR_HPP
