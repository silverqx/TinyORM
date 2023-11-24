#pragma once
#ifndef ORM_DRIVERS_SQLDRIVERERROR_HPP
#define ORM_DRIVERS_SQLDRIVERERROR_HPP

#include <QString>

#include <orm/macros/commonnamespace.hpp>
#include <orm/macros/export.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

    class SHAREDLIB_EXPORT SqlDriverError
    {
    public:
        enum ErrorType {
            NoError,
            ConnectionError,
            StatementError,
            TransactionError,
            UnknownError,
        };

        /*! Constructor. */
        SqlDriverError(
                const QString &driverMessage = {}, const QString &databaseMessage = {},
                ErrorType errorType = NoError, const QString &errorCode = {});
        /*! Default destructor. */
        inline ~SqlDriverError() = default;

        /*! Equality comparison operator for the SqlField. */
        bool operator==(const SqlDriverError &other) const;

        inline bool isValid() const;

        inline QString driverText() const;
        inline QString databaseText() const;
        inline ErrorType type() const;
        inline QString nativeErrorCode() const;

        QString text() const;

    private:
        /* Data members */
        QString m_driverText;
        QString m_databaseText;
        SqlDriverError::ErrorType m_errorType;
        QString m_errorCode;
    };

    /* public */

    bool SqlDriverError::isValid() const
    {
        return m_errorType != NoError;
    }

    QString SqlDriverError::driverText() const
    {
        return m_driverText;
    }

    QString SqlDriverError::databaseText() const
    {
        return m_databaseText;
    }

    SqlDriverError::ErrorType SqlDriverError::type() const
    {
        return m_errorType;
    }

    QString SqlDriverError::nativeErrorCode() const
    {
        return m_errorCode;
    }

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#ifndef QT_NO_DEBUG_STREAM
#  ifdef TINYORM_COMMON_NAMESPACE
SHAREDLIB_EXPORT QDebug
operator<<(QDebug debug,
           const TINYORM_COMMON_NAMESPACE::Orm::Drivers::SqlDriverError &error);
#  else
SHAREDLIB_EXPORT QDebug
operator<<(QDebug debug, const Orm::Drivers::SqlDriverError &error);
#  endif
#endif

#endif // ORM_DRIVERS_SQLDRIVERERROR_HPP
