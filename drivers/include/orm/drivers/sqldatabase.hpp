#ifndef ORM_DRIVERS_SQLDATABASE_HPP
#define ORM_DRIVERS_SQLDATABASE_HPP

#include <QtSql/qtsqlglobal.h>

#include "orm/drivers/sqldatabasemanager.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

    class SqlDriverError;

    class SHAREDLIB_EXPORT SqlDatabase : public SqlDatabaseManager
    {
        // To access SqlDatabase() default constructor by the ConnectionDict
        friend QHash<QString, SqlDatabase>;

        // To access the private Default constructor and d smart pointer (private impl.)
        friend SqlDatabaseManager;
        // To access the d smart pointer (private implementation)
        friend class SqlDatabasePrivate;

        /*! Default constructor (needed by the ConnectionDict (QHash)). */
        inline SqlDatabase() = default;

    protected:
        explicit SqlDatabase(const QString &driver);
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
        bool open();
        bool open(const QString &username, const QString &password);
        void close();

        /* Getters / Setters */
        bool isOpen() const;
        bool isOpenError() const;

        bool isValid() const;
        SqlDriverError lastError() const;

        QString driverName() const;
        QString connectionName() const;

        QString hostName() const;
        void setHostName(const QString &value);

        int port() const;
        void setPort(int p);

        QString databaseName() const;
        void setDatabaseName(const QString &database);

        QString userName() const;
        void setUserName(const QString &username);

        QString password() const;
        void setPassword(const QString &password);

        QString connectOptions() const;
        void setConnectOptions(const QString &options = QString());

        QSql::NumericalPrecisionPolicy numericalPrecisionPolicy() const;
        void setNumericalPrecisionPolicy(QSql::NumericalPrecisionPolicy precision);

        std::weak_ptr<SqlDriver> driver() const noexcept;

        /* Transactions */
        bool transaction();
        bool commit();
        bool rollback();

    private:
        std::shared_ptr<SqlDatabasePrivate> d;
    };

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#ifndef QT_NO_DEBUG_STREAM
#  ifdef TINYORM_COMMON_NAMESPACE
SHAREDLIB_EXPORT QDebug
operator<<(QDebug debug,
           const TINYORM_COMMON_NAMESPACE::Orm::Drivers::SqlDatabase &connection);
#  else
SHAREDLIB_EXPORT QDebug
operator<<(QDebug debug, const Orm::Drivers::SqlDatabase &connection);
#  endif
#endif

#endif // ORM_DRIVERS_SQLDATABASE_HPP
