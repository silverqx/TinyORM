#pragma once
#ifndef ORM_DRIVERS_DUMMYSQLERROR_HPP
#define ORM_DRIVERS_DUMMYSQLERROR_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QString>

#include <orm/macros/commonnamespace.hpp>

#include "orm/drivers/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

    /*! An empty dummy counterpart wrapper for Qt's SqlError (does nothing).
        TinyDrivers doesn't use this error class and throws exceptions instead.
        This class is used in some API-s like SqlQuery::lastError() to be API compatible
        with QtSql and it also helps to avoid #ifdef-s in user/client code. */
    class TINYDRIVERS_EXPORT DummySqlError
    {
    public:
        /*! An error type type. */
        enum ErrorType {
            NoError,
            ConnectionError,
            StatementError,
            TransactionError,
            UnknownError,
        };

        /*! Constructor. */
        inline DummySqlError( // NOLINT(google-explicit-constructor)
                const QString &driverText = {}, const QString &databaseText = {}, // NOLINT(google-explicit-constructor)
                ErrorType errorType = NoError, const QString &errorCode = {}) noexcept;
        /*! Default destructor. */
        inline ~DummySqlError() = default;

        /*! Copy constructor. */
        inline DummySqlError(const DummySqlError &) = default;
        /*! Copy assignment operator. */
        inline DummySqlError &operator=(const DummySqlError &) = default;

        /*! Move constructor. */
        inline DummySqlError(DummySqlError &&) noexcept = default;
        /*! Move assignment operator. */
        inline DummySqlError &operator=(DummySqlError &&) noexcept = default;

        /*! Equality comparison operator for the SqlError. */
        inline bool operator==(const DummySqlError &other) const noexcept;

        /*! Swap the SqlError. */
        inline void swap(DummySqlError &other) noexcept;

        /* Getters */
        /*! Determine whether an error is set (always returns false). */
        inline bool isValid() noexcept;
        /*! Get the drivers-specific error message (always returns an empty string). */
        inline QString driverText() const noexcept;
        /*! Get the database-specific error message (always returns an empty string). */
        inline QString databaseText() const noexcept;
        /*! Get an error type (always returns NoError aka. 0). */
        inline ErrorType type() const noexcept;
        /*! Get the database-specific error code (always returns an empty string). */
        inline QString nativeErrorCode() const noexcept;
        /*! Get the driverText() and databaseText() concatenated into a single string
            (always returns an empty string). */
        inline QString text() const noexcept;

    private:
        /*! Driver-specific error text. */
        QString m_driverText {};
        /*! Database-specific error text. */
        QString m_databaseText {};
        /*! An error type. */
        [[maybe_unused]]
        DummySqlError::ErrorType m_errorType = NoError;
        /*! Database-specific error code. */
        QString m_errorCode {};
    };

    /* public */

    DummySqlError::DummySqlError(
            const QString &/*unused*/, const QString &/*unused*/,
            const ErrorType /*unused*/, const QString &/*unused*/) noexcept
    {}

    bool DummySqlError::operator==(const DummySqlError &/*unused*/) const noexcept
    {
        return true;
    }

    void DummySqlError::swap(DummySqlError &/*unused*/) noexcept
    {}

    /* Getters */

    bool DummySqlError::isValid() noexcept // NOLINT(readability-convert-member-functions-to-static)
    {
        return false;
    }

    QString DummySqlError::driverText() const noexcept
    {
        return m_driverText;
    }

    QString DummySqlError::databaseText() const noexcept
    {
        return m_databaseText;
    }

    DummySqlError::ErrorType DummySqlError::type() const noexcept
    {
        return NoError;
    }

    QString DummySqlError::nativeErrorCode() const noexcept
    {
        return m_errorCode;
    }

    QString DummySqlError::text() const noexcept // NOLINT(readability-convert-member-functions-to-static)
    {
        return {};
    }

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#ifndef QT_NO_DEBUG_STREAM
TINYDRIVERS_EXPORT QDebug
operator<<(QDebug debug,
           const TINYORM_PREPEND_NAMESPACE(Orm::Drivers::DummySqlError) &error);
#endif

#endif // ORM_DRIVERS_DUMMYSQLERROR_HPP
