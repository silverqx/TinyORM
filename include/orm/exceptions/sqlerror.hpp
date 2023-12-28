#pragma once
#ifndef ORM_EXCEPTIONS_SQLERROR_HPP
#define ORM_EXCEPTIONS_SQLERROR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/macros/sqldrivermappings.hpp"
#include TINY_INCLUDE_TSqlError

#include "orm/exceptions/runtimeerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Exceptions
{

    /*! TinyORM SqlError exception, wrapper for the TSqlError. */
    class SHAREDLIB_EXPORT SqlError : public RuntimeError // clazy:exclude=copyable-polymorphic
    {
    public:
        /*! const char * constructor. */
        SqlError(const char *message, const TSqlError &error);
        /*! QString constructor. */
        SqlError(const QString &message, const TSqlError &error);

        /*! const char * constructor. */
        SqlError(const char *message, TSqlError &&error);
        /*! QString constructor. */
        SqlError(const QString &message, TSqlError &&error);

        /*! Get the original Qt SQL error. */
        inline const TSqlError &getSqlError() const noexcept;

    protected:
        /*! Protected converting constructor for use by descendants to avoid an error
            message formatting. */
        SqlError(const QString &message, const TSqlError &error, int /*unused*/);

        /*! Format the Qt SQL error message. */
        static QString formatMessage(const char *message, const TSqlError &error);

        /*! The Qt SQL error instance. */
        TSqlError m_sqlError;
    };

    /* public */

    const TSqlError &SqlError::getSqlError() const noexcept
    {
        return m_sqlError;
    }

} // namespace Orm::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_EXCEPTIONS_SQLERROR_HPP
