#pragma once
#ifndef ORM_DRIVERS_SQLDRIVERERROR_HPP
#define ORM_DRIVERS_SQLDRIVERERROR_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QString>

#include <orm/macros/commonnamespace.hpp>

#include "orm/drivers/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

    class TINYDRIVERS_EXPORT SqlDriverError
    {
    public:
        enum ErrorType {
            NoError,
            ConnectionError,
            StatementError,
            TransactionError,
            UnknownError,
        };

        /*! Constructor (not explicit to allow {}). */
        SqlDriverError(
                const QString &driverMessage = {}, const QString &databaseMessage = {},
                ErrorType errorType = NoError, const QString &errorCode = {});
        /*! Default destructor. */
        inline ~SqlDriverError() = default;

        /*! Equality comparison operator for the SqlDriverError. */
        bool operator==(const SqlDriverError &other) const;

        /*! Swap the SqlDriverError. */
        void swap(SqlDriverError &other) noexcept;

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
TINYDRIVERS_EXPORT QDebug
operator<<(QDebug debug,
           const TINYORM_PREPEND_NAMESPACE(Orm::Drivers::SqlDriverError) &error);
#endif

#endif // ORM_DRIVERS_SQLDRIVERERROR_HPP
