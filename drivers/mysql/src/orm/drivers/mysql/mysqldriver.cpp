#include "orm/drivers/mysql/mysqldriver.hpp"

#include <orm/macros/likely.hpp>

#include "orm/drivers/exceptions/invalidargumenterror.hpp"
#include "orm/drivers/exceptions/sqltransactionerror.hpp"
#include "orm/drivers/mysql/mysqlconstants_p.hpp"
#include "orm/drivers/mysql/mysqldriver_p.hpp"
#include "orm/drivers/mysql/mysqlresult.hpp"
#include "orm/drivers/mysql/mysqlutils_p.hpp"
#include "orm/drivers/utils/type_p.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

using Orm::Drivers::MySql::Constants::QMYSQL;

using MySqlUtils = Orm::Drivers::MySql::MySqlUtilsPrivate;

namespace Orm::Drivers::MySql
{

/* public */

MySqlDriver::MySqlDriver()
    : SqlDriver(std::make_unique<MySqlDriverPrivate>())
{}

bool MySqlDriver::open(
        const QString &database, const QString &username, const QString &password,
        const QString &host, const int port, const QString &options)
{
    Q_D(MySqlDriver);

    // CUR drivers log warning or throw? or add reopen method or parameter? silverqx
    if (isOpen())
        close();

    try {
        // Allocate and initialize the MYSQL object for the mysql_real_connect()
        d->mysqlInit();

        // mysql_thread_init() is called automatically by mysql_init()

        // Set extra MySQL connection options
        const auto [optionFlags, unixSocket] = d->mysqlSetConnectionOptions(options);

        // Set the default character set for the mysql_real_connect() function
        d->mysqlSetCharacterSet(host, true);

        // Establish a connection to the MySQL server running on the host
        d->mysqlRealConnect(host, username.toUtf8(), password.toUtf8(), database.toUtf8(),
                            port, unixSocket.toUtf8(), optionFlags);

        // Set the default character set for SQL statements
        d->mysqlSetCharacterSet(host, false);

        // Select the default database
        d->mysqlSelectDb(database);

    } catch (...) {
        // Deallocate a connection handler (this must be called whatever exception occurs)
        d->mysqlClose();

        // Re-throw
        throw;
    }

    setOpen(true);

    return true;
}

void MySqlDriver::close() noexcept
{
    Q_D(MySqlDriver);

    // Nothing to do
    if (!isOpen())
        return;

    setOpen(false);

    // QString::clear() isn't marked as noexcept even if it's so use this move alternative
    d->databaseName = QString();

    // Deallocate the connection handler
    d->mysqlClose();
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

QString MySqlDriver::driverName() const noexcept
{
    return QMYSQL;
}

/* Transactions */

bool MySqlDriver::beginTransaction()
{
    Q_D(MySqlDriver);

    // Nothing to do
    if (!isOpen())
        throw Exceptions::LogicError(
                u"MySQL database connection isn't open in %1()."_s.arg(__tiny_func__));

    if (mysql_query(d->mysql, "START TRANSACTION") == 0)
        return true;

    throw Exceptions::SqlTransactionError(
                u"Unable to start transaction in %1()."_s.arg(__tiny_func__),
                MySqlUtils::prepareMySqlError(d->mysql));
}

bool MySqlDriver::commitTransaction()
{
    Q_D(MySqlDriver);

    // Nothing to do
    if (!isOpen())
        throw Exceptions::LogicError(
                u"MySQL database connection isn't open in %1()."_s.arg(__tiny_func__));

    if (mysql_query(d->mysql, "COMMIT") == 0)
        return true;

    throw Exceptions::SqlTransactionError(
                u"Unable to commit transaction in %1()."_s.arg(__tiny_func__),
                MySqlUtils::prepareMySqlError(d->mysql));
}

bool MySqlDriver::rollbackTransaction()
{
    Q_D(MySqlDriver);

    // Nothing to do
    if (!isOpen())
        throw Exceptions::LogicError(
                u"MySQL database connection isn't open in %1()."_s.arg(__tiny_func__));

    if (mysql_query(d->mysql, "ROLLBACK") == 0)
        return true;

    throw Exceptions::SqlTransactionError(
                u"Unable to rollback transaction in %1()."_s.arg(__tiny_func__),
                MySqlUtils::prepareMySqlError(d->mysql));
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
    if (const auto driverShared = driver.lock(); driverShared) T_LIKELY
        /* We need to upcast here, there is no other way, it also has to be
           std::weak_ptr(), it can't be done better. This upcast is kind of check,
           we can't pass down the SqlDriver to the MySqlResult.
           Even if it would be the shared_ptr<SqlDriver> we had to upcast the same way. */
        return std::make_unique<MySqlResult>(
                    std::dynamic_pointer_cast<MySqlDriver>(driverShared));

    else T_UNLIKELY
        throw Exceptions::InvalidArgumentError(
                u"The 'driver' argument can't be nullptr, it can't be expired in %1()."_s
                .arg(__tiny_func__));
}

} // namespace Orm::Drivers::MySql

TINYORM_END_COMMON_NAMESPACE
