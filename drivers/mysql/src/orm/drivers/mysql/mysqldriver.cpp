#include "orm/drivers/mysql/mysqldriver.hpp"

#include <orm/macros/likely.hpp>

#include "orm/drivers/exceptions/invalidargumenterror.hpp"
#include "orm/drivers/mysql/mysqldriver_p.hpp"
#include "orm/drivers/mysql/mysqlresult.hpp"
#include "orm/drivers/mysql/mysqlutils_p.hpp"
#include "orm/drivers/utils/type_p.hpp"

Q_DECLARE_METATYPE(MYSQL_RES *)
Q_DECLARE_METATYPE(MYSQL *)
Q_DECLARE_METATYPE(MYSQL_STMT *)

// BUG drivers defined at bad place also wrap all occurrences in #ifdef silverqx
// MYSQL_TYPE_JSON was added in MySQL 5.7.9
#if defined(MYSQL_VERSION_ID) && MYSQL_VERSION_ID < 50709
#  define MYSQL_TYPE_JSON 245
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

using MySqlUtils = Orm::Drivers::MySql::MySqlUtilsPrivate;

namespace Orm::Drivers::MySql
{

/* public */

MySqlDriver::MySqlDriver()
    : SqlDriver(std::make_unique<MySqlDriverPrivate>(this))
{}

bool MySqlDriver::open(
        const QString &database, const QString &username, const QString &password,
        const QString &host, const int port, const QString &options)
{
    Q_D(MySqlDriver);

    // CUR drivers log warning? silverqx
    if (isOpen())
        close();

    // Allocate and initialize the MYSQL object for the mysql_real_connect()
    if (!d->mysqlInit())
        return false;

    // Set extra MySQL connection options
    const auto [optionFlags, unixSocket] = d->mysqlSetConnectionOptions(options);

    // Set the default character set for the mysql_real_connect() function
    if (!d->mysqlSetCharacterSet(host, true))
        return false;

    // Establish a connection to the MySQL server running on the host
    if (!d->mysqlRealConnect(host, username.toUtf8(), password.toUtf8(),
                             database.toUtf8(), port, unixSocket.toUtf8(), optionFlags)
    )
        return false;

    // Set the default character set for SQL statements
    if (!d->mysqlSetCharacterSet(host, false))
        return false;

    // Select the default database
    if (!d->mysqlSelectDb(database))
        return false;

    // CUR drivers called to late? silverqx
#if QT_CONFIG(thread)
    mysql_thread_init();
#endif

    setOpen(true);
    setOpenError(false);

    return true;
}

void MySqlDriver::close() noexcept
{
    Q_D(MySqlDriver);

    // Nothing to do
    if (!isOpen())
        return;

    setOpenError(false);
    setOpen(false);

    // QString::clear() isn't marked noexcept even it is so use this move alternative
    d->databaseName = QString();
    d->mysql = nullptr;

#if QT_CONFIG(thread)
    mysql_thread_end();
#endif
    mysql_close(d->mysql);
}

/* Getters / Setters */

bool MySqlDriver::hasFeature(const DriverFeature feature) const noexcept
{
    Q_D(const MySqlDriver);

    switch (feature) {
    case Transactions:
        return d->supportsTransactions();

    case BLOB:
    case LastInsertId:
    case LowPrecisionNumbers:
    case PositionalPlaceholders:
    case PreparedQueries:
    case QuerySize:
    case Unicode:
        return true;

    case BatchOperations:
    case CancelQuery:
    case EventNotifications:
    case FinishQuery:
    case MultipleResultSets:
    case NamedPlaceholders:
    case SimpleLocking:
        return false;

    default:
        Q_UNREACHABLE();
    }

    return false;
}

QVariant MySqlDriver::handle() const noexcept
{
    Q_D(const MySqlDriver);
    return QVariant::fromValue(d->mysql);
}

/* Transactions */

bool MySqlDriver::beginTransaction()
{
    Q_D(MySqlDriver);

    // Nothing to do
    if (!isOpen()) {
        qWarning("MySqlDriver::beginTransaction: Database not open");
        return false;
    }

    if (mysql_query(d->mysql, "START TRANSACTION") == 0)
        return true;

    return setLastError(MySqlUtils::createError(
                            u"Unable to start transaction"_s,
                            SqlError::TransactionError, d->mysql));
}

bool MySqlDriver::commitTransaction()
{
    Q_D(MySqlDriver);

    // Nothing to do
    if (!isOpen()) {
        qWarning("MySqlDriver::commitTransaction: Database not open");
        return false;
    }

    if (mysql_query(d->mysql, "COMMIT") == 0)
        return true;

    return setLastError(MySqlUtils::createError(
                            u"Unable to commit transaction"_s,
                            SqlError::TransactionError, d->mysql));
}

bool MySqlDriver::rollbackTransaction()
{
    Q_D(MySqlDriver);

    // Nothing to do
    if (!isOpen()) {
        qWarning("MySqlDriver::rollbackTransaction: Database not open");
        return false;
    }

    if (mysql_query(d->mysql, "ROLLBACK") == 0)
        return true;

    return setLastError(MySqlUtils::createError(
                            u"Unable to rollback transaction"_s,
                            SqlError::TransactionError, d->mysql));
}

/* Others */

bool MySqlDriver::isIdentifierEscaped(const QString &identifier,
                                      const IdentifierType /*unused*/) const
{
    return identifier.size() > 2 &&
           identifier.startsWith('`'_L1) &&
           identifier.endsWith('`'_L1);
}

std::unique_ptr<SqlResult>
MySqlDriver::createResult(const std::weak_ptr<SqlDriver> &driver) const
{
    if (const auto driverShared = driver.lock();
        driverShared
    ) T_LIKELY
        /* We need to upcast here, there is no other way, it also has to be
           std::weak_ptr(), it can't be done any better. This upcast is kind of check,
           we can't pass down the SqlDriver to the MySqlResult.
           Even if it would be the shared_ptr<SqlDriver> we had to upcast the same way. */
        return std::make_unique<MySqlResult>(
                    std::dynamic_pointer_cast<MySqlDriver>(driverShared));

    else T_UNLIKELY
        throw Exceptions::InvalidArgumentError(
                u"The driver must be valid, it can't be expired in %1()."_s
                .arg(__tiny_func__));
}

} // namespace Orm::Drivers::MySql

TINYORM_END_COMMON_NAMESPACE
