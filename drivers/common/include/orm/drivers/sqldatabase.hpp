#ifndef ORM_DRIVERS_SQLDATABASE_HPP
#define ORM_DRIVERS_SQLDATABASE_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "orm/drivers/driverstypes.hpp"
#include "orm/drivers/sqldatabasemanager.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

    class SqlDriverError;

    /*! Database connection. */
    class TINYDRIVERS_EXPORT SqlDatabase : public SqlDatabaseManager
    {
        // To access SqlDatabase() default constructor by the ConnectionsHash
        friend QHash<QString, SqlDatabase>;

        // To access the private constructors and d smart pointer (private impl.)
        friend SqlDatabaseManager;
        // To access the d smart pointer (private implementation)
        friend class SqlDatabasePrivate;

        /*! Default constructor (needed by the SqlDatabaseManager::cloneDatabase()). */
        inline SqlDatabase() = default;

        /*! Protected constructor. */
        explicit SqlDatabase(const QString &driver);
        /*! Protected constructor. */
        explicit SqlDatabase(std::unique_ptr<SqlDriver> &&driver);

    public:
        /*! Default destructor. */
        ~SqlDatabase();

        /*! Copy constructor. */
        inline SqlDatabase(const SqlDatabase &) = default;
        /*! Copy assignment operator. */
        inline SqlDatabase &operator=(const SqlDatabase &) = default;

        /*! Move constructor. */
        inline SqlDatabase(SqlDatabase &&) noexcept = default;
        /*! Move assignment operator. */
        inline SqlDatabase &operator=(SqlDatabase &&) noexcept = default;

        /* Database connection */
        /*! Open the database connection using the current connection values. */
        bool open();
        /*! Open the database connection using the given username and password.
            The password is not stored but passed directly to the database driver. */
        bool open(const QString &username, const QString &password);
        /*! Close the current connection (invalidates all SqlQuery1 instances). */
        void close();

        /* Getters / Setters */
        /*! Determine whether the connection is currently open. */
        bool isOpen() const;
        /*! Determine if an error occurred while opening a new database connection. */
        bool isOpenError() const;

        /*! Determine whether the connection has a valid database driver. */
        bool isValid() const;
        /*! Get information about the last error that occurred on the database. */
        SqlDriverError lastError() const;

        /*! Get the database driver name for the current connection. */
        QString driverName() const;
        /*! Get the current connection name. */
        QString connectionName() const;

        /*! Get the hostname. */
        QString hostName() const;
        /*! Set the hostname. */
        void setHostName(const QString &value);

        /*! Get the port. */
        int port() const;
        /*! Set the port. */
        void setPort(int p);

        /*! Get the database name. */
        QString databaseName() const;
        /*! Set the database name. */
        void setDatabaseName(const QString &database);

        /*! Get the username. */
        QString userName() const;
        /*! Set the username. */
        void setUserName(const QString &username);

        /*! Get the password. */
        QString password() const;
        /*! Set the password. */
        void setPassword(const QString &password);

        /*! Get the connection options. */
        QString connectOptions() const;
        /*! Set the connection options. */
        void setConnectOptions(const QString &options = {});

        /*! Get the current numerical precision policy. */
        NumericalPrecisionPolicy numericalPrecisionPolicy() const;
        /*! Set the numerical precision policy for the current connection. */
        void setNumericalPrecisionPolicy(NumericalPrecisionPolicy precision);

        /*! Get the database driver used to access the database connection. */
        std::weak_ptr<SqlDriver> driver() const noexcept;

        /*! Disable the different thread check during SqlDatabase::database() call. */
        static void disableThreadCheck() noexcept;

        /* Transactions */
        /*! Start a new database transaction. */
        bool transaction();
        /*! Commit the active database transaction. */
        bool commit();
        /*! Rollback the active database transaction. */
        bool rollback();

    private:
        /*! Smart pointer to the private implementation. */
        std::shared_ptr<SqlDatabasePrivate> d = nullptr;
    };

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#ifndef QT_NO_DEBUG_STREAM
TINYDRIVERS_EXPORT QDebug
operator<<(QDebug debug,
           const TINYORM_PREPEND_NAMESPACE(Orm::Drivers::SqlDatabase) &connection);
#endif

#endif // ORM_DRIVERS_SQLDATABASE_HPP
