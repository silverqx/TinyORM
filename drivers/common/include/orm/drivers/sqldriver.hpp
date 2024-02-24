#pragma once
#ifndef ORM_DRIVERS_SQLDRIVER_HPP
#define ORM_DRIVERS_SQLDRIVER_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QVariant>

#include <thread>

#include <orm/macros/commonnamespace.hpp>

#include "orm/drivers/driverstypes.hpp"
#include "orm/drivers/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

    class DummySqlError;
    class SqlDriverPrivate;
    class SqlResult;

    /*! Database driver abstract class. */
    class TINYDRIVERS_EXPORT SqlDriver
    {
        Q_DISABLE_COPY_MOVE(SqlDriver)
        Q_DECLARE_PRIVATE(SqlDriver)

    protected:
        /*! Protected constructor. */
        explicit SqlDriver(std::unique_ptr<SqlDriverPrivate> &&dd) noexcept;

    public:
        /* Enum-s tagged with "for API compatibility" were removed (no plan to use them),
           but were added back to be API compatible with QSqlDriver::DriverFeature. */

        /*! Supported driver features enum. */
        enum DriverFeature {
            /*! Supports batched operations (unused, for API compatibility). */
            BatchOperations,
            /*! Supports BLOB-s fields (Binary Large Objects). */
            BLOB,
            /*! Supports cancelling a running query (unused, for API compatibility). */
            CancelQuery,
            /*! Supports database event notifications (unused, for API compatibility). */
            EventNotifications,
            /*! Does low-level resource cleanup when QSqlQuery::finish() is called. */
            FinishQuery,
            /*! Supports returning an ID after the previous INSERT or UPDATE statement. */
            LastInsertId,
            /*! Supports fetching numerical values with low precision. */
            LowPrecisionNumbers,
            /*! Supports accessing multiple result sets returned from batched statements
                or stored procedures. */
            MultipleResultSets,
            /*! Supports the use of named placeholders. */
            NamedPlaceholders,
            /*! Supports the use of positional placeholders. */
            PositionalPlaceholders,
            /*! Supports prepared query execution (prepared statements). */
            PreparedQueries,
            /*! Whether the database is capable of reporting the size of a query.
                The QSqlQuery::size() will return -1 if a database doesn't support it
                (SQLite doesn't support this). */
            QuerySize,
            /*! Disallows a write lock on a table while other queries have a read lock
                on it. */
            SimpleLocking,
            /*! Supports SQL transactions. */
            Transactions,
            /*! Supports Unicode strings if the database server does. */
            Unicode,
        };

        /*! Identifier type enum. */
        enum IdentifierType {
            /*! Field/column name. */
            FieldName,
            /*! Table name. */
            TableName,
        };

        /*! Database driver type enum. */
        enum DbmsType {
            /*! MySQL database driver. */
            MySqlServer,
            // Not implemented yet :/ (must be defined because eg. Orm::SqlQuery uses it)
            /*! PostgreSQL database driver. */
            PostgreSQL,
            /*! SQLite database driver. */
            SQLite,
        };

        /*! Pure virtual destructor. */
        virtual ~SqlDriver() = 0;

        /*! Open the database connection using the given connection values. */
        virtual bool
        open(const QString &database, const QString &username, const QString &password,
             const QString &host, int port, const QString &options) = 0;
        /*! Reopen the database connection using the given connection values. */
        virtual bool
        reopen(const QString &database, const QString &username, const QString &password,
               const QString &host, int port, const QString &options) = 0;
        /*! Close the current database connection. */
        virtual void close() noexcept = 0;

        /* Getters / Setters */
        /*! Determine whether the current driver supports the given feature. */
        virtual bool hasFeature(DriverFeature feature) const noexcept = 0;

        /*! Determine whether the connection is currently open. */
        virtual bool isOpen() const noexcept;
        /*! Determine if an error occurred while opening a new database connection. */
        bool isOpenError() const noexcept;

        /*! Get the underlying database handle (low-level handle, eg. MYSQL *). */
        virtual QVariant handle() const noexcept = 0;
        /*! Get the database driver name. */
        virtual QString driverName() const noexcept = 0;

        /*! Get the current database driver type. */
        DbmsType dbmsType() const noexcept;
        /*! Get information about the last error that occurred on the database. */
        DummySqlError lastError() const noexcept;

        /*! Get the default numerical precision policy for the current driver. */
        NumericalPrecisionPolicy defaultNumericalPrecisionPolicy() const noexcept;
        /*! Set the default numerical precision policy for the current driver. */
        void
        setDefaultNumericalPrecisionPolicy(NumericalPrecisionPolicy precision) noexcept;

        /*! Get the default numerical precision policy for the current driver (alias). */
        inline NumericalPrecisionPolicy numericalPrecisionPolicy() const noexcept;
        /*! Set the default numerical precision policy for the current driver (alias). */
        inline void
        setNumericalPrecisionPolicy(NumericalPrecisionPolicy precision) noexcept;

        /*! Get the thread ID in which the driver was instantiated. */
        std::thread::id threadId() const noexcept;

        /* Transactions */
        /*! Start a new database transaction. */
        virtual bool beginTransaction() = 0;
        /*! Commit the active database transaction. */
        virtual bool commitTransaction() = 0;
        /*! Rollback the active database transaction. */
        virtual bool rollbackTransaction() = 0;

        /* Others */
        /*! Get the maximum identifier length for the current driver. */
        virtual int maximumIdentifierLength(IdentifierType type) const noexcept;
        /*! Determine whether the given identifier is escaped/quoted. */
        virtual bool isIdentifierEscaped(const QString &identifier,
                                         IdentifierType type) const;
        /*! Strip delimiters/quotes from the given identifier. */
        virtual QString stripDelimiters(const QString &identifier,
                                        IdentifierType type) const;

        /*! Factory method to create an empty database result. */
        virtual std::unique_ptr<SqlResult>
        createResult(const std::weak_ptr<SqlDriver> &driver) const = 0;

    protected:
        /* Setters */
        /*! Set a flag whether the connection is open. */
        void setOpen(bool value) noexcept;

        /* Data members */
        /*! Smart pointer to the private implementation. */
        std::unique_ptr<SqlDriverPrivate> d_ptr;
    };

    /* public */

    // Must be defined to be API compatible with the QtSql module

    NumericalPrecisionPolicy SqlDriver::numericalPrecisionPolicy() const noexcept
    {
        return defaultNumericalPrecisionPolicy();
    }

    void SqlDriver::setNumericalPrecisionPolicy(
                const NumericalPrecisionPolicy precision) noexcept
    {
        setDefaultNumericalPrecisionPolicy(precision);
    }

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_SQLDRIVER_HPP
