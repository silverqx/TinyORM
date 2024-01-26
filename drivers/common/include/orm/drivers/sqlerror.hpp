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
        inline SqlError(ErrorType errorType) noexcept;
        /*! Constructor (not explicit to allow {}). */
        SqlError(QString driverText = {}, QString databaseText = {},
                 ErrorType errorType = NoError, QString errorCode = {}) noexcept;
        /*! Default destructor. */
        inline ~SqlError() = default;

        /*! Copy constructor. */
        inline SqlError(const SqlError &) = default;
        /*! Copy assignment operator. */
        inline SqlError &operator=(const SqlError &) = default;

        /*! Move constructor. */
        inline SqlError(SqlError &&) noexcept = default;
        /*! Move assignment operator. */
        inline SqlError &operator=(SqlError &&) noexcept = default;

        /*! Equality comparison operator for the SqlError. */
        bool operator==(const SqlError &other) const noexcept;

        /*! Swap the SqlError. */
        void swap(SqlError &other) noexcept;

        inline bool isValid() const noexcept;

        inline QString driverText() const noexcept;
        inline QString databaseText() const noexcept;
        inline ErrorType type() const noexcept;
        inline QString nativeErrorCode() const noexcept;

        QString text() const;

    private:
        /* Data members */
        QString m_driverText;
        QString m_databaseText;
        SqlError::ErrorType m_errorType;
        QString m_errorCode;
    };

    /* public */

    SqlError::SqlError(const ErrorType errorType) noexcept
        : m_errorType(NoError)
    {
        Q_ASSERT_X(errorType == NoError, "SqlError(ErrorType)",
                   "This constructor can be called with the ErrorType::NoError only.");
    }

    bool SqlError::isValid() const noexcept
    {
        return m_errorType != NoError;
    }

    QString SqlError::driverText() const noexcept
    {
        return m_driverText;
    }

    QString SqlError::databaseText() const noexcept
    {
        return m_databaseText;
    }

    SqlError::ErrorType SqlError::type() const noexcept
    {
        return m_errorType;
    }

    QString SqlError::nativeErrorCode() const noexcept
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
