#pragma once
#ifndef ORM_DRIVERS_SQLDATABASE_HPP
#define ORM_DRIVERS_SQLDATABASE_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "orm/drivers/driverstypes.hpp"
#include "orm/drivers/sqldatabasemanager.hpp"
#include "orm/drivers/utils/notnull.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

    class DummySqlError;
    class SqlDatabasePrivate;
    class SqlRecord;

    /*! Database connection. */
    class TINYDRIVERS_EXPORT SqlDatabase : public SqlDatabaseManager
    {
        // To access the private constructors and d smart pointer (private impl.)
        friend SqlDatabaseManager;
        // To access the d smart pointer (private implementation)
        friend SqlDatabasePrivate;

        /*! Alias for the NotNull. */
        template<typename T>
        using NotNull = Orm::Drivers::Utils::NotNull<T>;

        /*! Private constructor. */
        SqlDatabase(const QString &driver, const QString &connection);
        /*! Private constructor. */
        SqlDatabase(QString &&driver, const QString &connection);
        /*! Private constructor. */
        explicit SqlDatabase(std::unique_ptr<SqlDriver> &&driver);

    public:
        /*! Default destructor. */
        ~SqlDatabase() noexcept;

        /*! Copy constructor. */
        SqlDatabase(const SqlDatabase &) = default;
        /*! Copy assignment operator. */
        SqlDatabase &operator=(const SqlDatabase &) = default;

        /*! Move constructor. */
        SqlDatabase(SqlDatabase &&) noexcept = default;
        /*! Move assignment operator. */
        SqlDatabase &operator=(SqlDatabase &&) noexcept = default;

        /* Database connection */
        /*! Open the database connection using the current connection values. */
        bool open();
        /*! Open the database connection using the given username and password.
            The password is not stored but passed directly to the database driver. */
        bool open(const QString &username, const QString &password);
        /*! Close the current connection (invalidates all SqlQuery instances). */
        void close() noexcept;

        /* Getters / Setters */
        /*! Determine whether the connection is currently open. */
        bool isOpen() const noexcept;
        /*! Determine if an error occurred while opening a new database connection. */
        bool isOpenError() const noexcept;

        /*! Determine whether the connection has a valid database driver. */
        bool isValid() const noexcept;
        /*! Get information about the last error that occurred on the database. */
        DummySqlError lastError() const noexcept Q_DECL_CONST_FUNCTION;

        /*! Get the database driver name for the current connection. */
        QString driverName() const noexcept;
        /*! Get the current connection name. */
        QString connectionName() const noexcept;

        /*! Get the hostname. */
        QString hostName() const noexcept;
        /*! Set the hostname. */
        void setHostName(const QString &hostname) noexcept;
        /*! Set the hostname. */
        void setHostName(QString &&hostname) noexcept;

        /*! Get the port. */
        int port() const noexcept;
        /*! Set the port. */
        void setPort(int port) noexcept;

        /*! Get the database name. */
        QString databaseName() const noexcept;
        /*! Set the database name. */
        void setDatabaseName(const QString &database) noexcept;
        /*! Set the database name. */
        void setDatabaseName(QString &&database) noexcept;

        /*! Get the username. */
        QString userName() const noexcept;
        /*! Set the username. */
        void setUserName(const QString &username) noexcept;
        /*! Set the username. */
        void setUserName(QString &&username) noexcept;

        /*! Get the password. */
        QString password() const noexcept;
        /*! Set the password. */
        void setPassword(const QString &password) noexcept;
        /*! Set the password. */
        void setPassword(QString &&password) noexcept;

        /*! Get the connection options. */
        QString connectOptions() const noexcept;
        /*! Set the connection options. */
        void setConnectOptions(const QString &options) noexcept;
        /*! Set the connection options. */
        void setConnectOptions(QString &&options = {}) noexcept;

        /*! Get the current numerical precision policy. */
        NumericalPrecisionPolicy numericalPrecisionPolicy() const noexcept;
        /*! Set the numerical precision policy for the current connection. */
        void setNumericalPrecisionPolicy(NumericalPrecisionPolicy precision) noexcept;

        /*! Get the SQL database driver used to access the database connection (const). */
        const SqlDriver *driver() const noexcept;
        /*! Get the SQL database driver used to access the database connection (const). */
        std::weak_ptr<const SqlDriver> driverWeak() const noexcept;

        /*! Determine if the same thread check during the database() call is enabled. */
        static bool isThreadCheckEnabled() noexcept;
        /*! Disable the same thread check during SqlDatabase::database() call. */
        static void disableThreadCheck() noexcept;

        /* Transactions */
        /*! Start a new database transaction. */
        bool transaction();
        /*! Commit the active database transaction. */
        bool commit();
        /*! Rollback the active database transaction. */
        bool rollback();

        /* Others */
        /*! Get a SqlRecord containing the field information for the given table. */
        SqlRecord record(const QString &table, bool withDefaultValues = true) const;

    private:
        /*! Set the connection name. */
        void setConnectionName(const QString &connection) noexcept;

        /*! Smart pointer to the private implementation. */
        NotNull<std::shared_ptr<SqlDatabasePrivate>> d;
    };

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#ifndef QT_NO_DEBUG_STREAM
TINYDRIVERS_EXPORT QDebug
operator<<(QDebug debug,
           const TINYORM_PREPEND_NAMESPACE(Orm::Drivers::SqlDatabase) &connection);
#endif

#endif // ORM_DRIVERS_SQLDATABASE_HPP
