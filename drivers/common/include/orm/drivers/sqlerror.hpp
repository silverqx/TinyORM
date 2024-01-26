#pragma once
#ifndef ORM_DRIVERS_SQLERROR_HPP
#define ORM_DRIVERS_SQLERROR_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QString>

#include <orm/macros/commonnamespace.hpp>

#include "orm/drivers/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

    class TINYDRIVERS_EXPORT SqlError
    {
    public:
        enum ErrorType {
            NoError,
            ConnectionError,
            StatementError,
            TransactionError,
            UnknownError,
        };

        /*! Constructor (not explicit to allow setLastError(SqlError::NoError)). */
        inline SqlError(ErrorType errorType);
        /*! Constructor (not explicit to allow {}). */
        SqlError(QString driverText = {}, QString databaseText = {},
                 ErrorType errorType = NoError, QString errorCode = {});
        /*! Default destructor. */
        inline ~SqlError() = default;

        /*! Equality comparison operator for the SqlError. */
        bool operator==(const SqlError &other) const;

        /*! Swap the SqlError. */
        void swap(SqlError &other) noexcept;

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
        SqlError::ErrorType m_errorType;
        QString m_errorCode;
    };

    /* public */

    SqlError::SqlError(const ErrorType errorType)
        : m_errorType(NoError)
    {
        Q_ASSERT_X(errorType == NoError, "SqlError(ErrorType)",
                   "This constructor can be called with the ErrorType::NoError only.");
    }

    bool SqlError::isValid() const
    {
        return m_errorType != NoError;
    }

    QString SqlError::driverText() const
    {
        return m_driverText;
    }

    QString SqlError::databaseText() const
    {
        return m_databaseText;
    }

    SqlError::ErrorType SqlError::type() const
    {
        return m_errorType;
    }

    QString SqlError::nativeErrorCode() const
    {
        return m_errorCode;
    }

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#ifndef QT_NO_DEBUG_STREAM
TINYDRIVERS_EXPORT QDebug
operator<<(QDebug debug, const TINYORM_PREPEND_NAMESPACE(Orm::Drivers::SqlError) &error);
#endif

#endif // ORM_DRIVERS_SQLERROR_HPP
