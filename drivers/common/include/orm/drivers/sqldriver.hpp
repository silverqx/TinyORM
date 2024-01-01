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

    class SqlDriverPrivate;
    class SqlError;
    class SqlField;
    class SqlRecord;
    class SqlResult;

    /*! Database driver abstract class. */
    class TINYDRIVERS_EXPORT SqlDriver
    {
        Q_DISABLE_COPY_MOVE(SqlDriver)
        Q_DECLARE_PRIVATE(SqlDriver)

    protected:
        /*! Protected constructor. */
        explicit SqlDriver(std::unique_ptr<SqlDriverPrivate> &&dd);

    public:
        /*! Supported driver features enum. */
        enum DriverFeature {
            BatchOperations,
            BLOB,
            CancelQuery,
            EventNotifications,
            FinishQuery,
            LastInsertId,
            LowPrecisionNumbers,
            MultipleResultSets,
            NamedPlaceholders,
            PositionalPlaceholders,
            PreparedQueries,
            QuerySize,
            SimpleLocking,
            Transactions,
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
            /*! Unknown database driver. */
            UnknownDbms,
            /*! MySQL database driver. */
            MySqlServer,
            // Not implemented yet :/ (must be defined because eg. Orm::SqlQuery use it)
            /*! PostgreSQL database driver. */
            PostgreSQL,
            /*! SQLite database driver. */
            SQLite,
        };

        /*! Pure virtual destructor. */
        virtual ~SqlDriver() = 0;

        /*! Open the database connection using the given values. */
        virtual bool
        open(const QString &database, const QString &username, const QString &password,
             const QString &host, int port, const QString &options) = 0;
        /*! Close the current database connection. */
        virtual void close() = 0;

        /* Getters / Setters */
        /*! Determine whether the current driver supports the given feature. */
        virtual bool hasFeature(DriverFeature feature) const = 0;

        /*! Determine whether the connection is currently open. */
        virtual bool isOpen() const;
        /*! Determine if an error occurred while opening a new database connection. */
        bool isOpenError() const;

        /*! Get the underlying database handle (low-level handle, eg. MYSQL *). */
        virtual QVariant handle() const = 0;
        /*! Get the database driver name. */
        virtual QString driverName() const = 0;

        /*! Get the current database driver type. */
        DbmsType dbmsType() const;
        /*! Get information about the last error that occurred on the database. */
        SqlError lastError() const;

        /*! Get the default numerical precision policy for the current driver. */
        NumericalPrecisionPolicy defaultNumericalPrecisionPolicy() const;
        /*! Set the default numerical precision policy for the current driver. */
        void setDefaultNumericalPrecisionPolicy(NumericalPrecisionPolicy precision);

        /*! Get the default numerical precision policy for the current driver (alias). */
        inline NumericalPrecisionPolicy numericalPrecisionPolicy() const;
        /*! Set the default numerical precision policy for the current driver (alias). */
        inline void setNumericalPrecisionPolicy(NumericalPrecisionPolicy precision);

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
        virtual int maximumIdentifierLength(IdentifierType type) const;
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
        void setOpen(bool value);
        /*! Set a flag if an error while opening a new database connection. */
        void setOpenError(bool value);
        /*! Set the last database error. */
        bool setLastError(const SqlError &error);
        /*! Set an error that occurred during opening a new database connection. */
        bool setLastOpenError(const SqlError &error);

        /* Data members */
        /*! Smart pointer to the private implementation. */
        std::unique_ptr<SqlDriverPrivate> d_ptr;
    };

    /* public */

    // Must be defined to be API compatible with the QtSql module

    NumericalPrecisionPolicy SqlDriver::numericalPrecisionPolicy() const
    {
        return defaultNumericalPrecisionPolicy();
    }

    void SqlDriver::setNumericalPrecisionPolicy(const NumericalPrecisionPolicy precision)
    {
        setDefaultNumericalPrecisionPolicy(precision);
    }

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_SQLDRIVER_HPP
