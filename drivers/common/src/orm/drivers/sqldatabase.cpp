#include "orm/drivers/sqldatabase.hpp"

#ifndef QT_NO_DEBUG_STREAM
#  include <QDebug>
#endif

#include <orm/macros/likely.hpp>

#include "orm/drivers/dummysqlerror.hpp"
#include "orm/drivers/sqldatabase_p.hpp"
#include "orm/drivers/sqldriver.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

/* private */

SqlDatabase::SqlDatabase(const QString &driver)
    : d(SqlDatabasePrivate::createSqlDatabasePrivate(driver))
{}

SqlDatabase::SqlDatabase(QString &&driver)
    : d(SqlDatabasePrivate::createSqlDatabasePrivate(std::move(driver)))
{}

SqlDatabase::SqlDatabase(std::unique_ptr<SqlDriver> &&driver)
    : d(SqlDatabasePrivate::createSqlDatabasePrivate(std::move(driver)))
{}

/* public */

SqlDatabase::~SqlDatabase() noexcept
{
    // CUR drivers finish multi-thread (but SqlDatabase can be used only from thread where it was created) silverqx
    if (d && d.use_count() == 1)
        close();
}

/* Database connection */

bool SqlDatabase::open()
{
    return d->driver()
             .open(d->databaseName, d->username, d->password, d->hostName, d->port,
                   d->connectionOptions);
}

bool SqlDatabase::open(const QString &username, const QString &password)
{
    /* This method doesn't store the given password. Instead, the password is passed
       directly to the driver to open the connection and is then discarded (more secure).
       Because of this, only the username setter is called, all other setters must be
       called before the open() method. */

    setUserName(username);

    return d->driver()
             .open(d->databaseName, username, password, d->hostName, d->port,
                   d->connectionOptions);
}

void SqlDatabase::close() noexcept
{
    if (!isValid())
        return;

    // Used the driverPtr() to have the close() method noexcept
    auto *driver = d->driverPtr();

    if (driver == nullptr)
        return;

    driver->close();
}

/* Getters / Setters */

bool SqlDatabase::isOpen() const noexcept
{
    // Used the driverPtr() to have the isOpen() method noexcept
    const auto *const driver = d->driverPtr();

    return driver != nullptr && driver->isOpen();
}

bool SqlDatabase::isOpenError() const noexcept // NOLINT(readability-convert-member-functions-to-static)
{
    return false;
}

bool SqlDatabase::isValid() const noexcept
{
    /* The SqlDatabase connection can be invalid when the SqlDatabase local copy is
       created and the removeDatabase() -> invalidateConnection() -> d.reset() method is
       called while this local copy is still in scope (this is the only case).
       Creating an invalid SqlDatabase instance manually by an user isn't possible because
       the default constructor is private. */
    return d && d->isDriverValid();
}

DummySqlError SqlDatabase::lastError() const noexcept // NOLINT(readability-convert-member-functions-to-static)
{
    return {};
}

QString SqlDatabase::driverName() const noexcept
{
    return d->driverName;
}

QString SqlDatabase::connectionName() const noexcept
{
    return d->connectionName;
}

QString SqlDatabase::hostName() const noexcept
{
    return d->hostName;
}

void SqlDatabase::setHostName(const QString &hostname) noexcept
{
    d->hostName = hostname;
}

void SqlDatabase::setHostName(QString &&hostname) noexcept
{
    d->hostName = std::move(hostname);
}

int SqlDatabase::port() const noexcept
{
    return d->port;
}

void SqlDatabase::setPort(const int port) noexcept
{
    d->port = port;
}

QString SqlDatabase::databaseName() const noexcept
{
    return d->databaseName;
}

void SqlDatabase::setDatabaseName(const QString &database) noexcept
{
    d->databaseName = database;
}

void SqlDatabase::setDatabaseName(QString &&database) noexcept
{
    d->databaseName = std::move(database);
}

QString SqlDatabase::userName() const noexcept
{
    return d->username;
}

void SqlDatabase::setUserName(const QString &username) noexcept
{
    d->username = username;
}

void SqlDatabase::setUserName(QString &&username) noexcept
{
    d->username = std::move(username);
}

QString SqlDatabase::password() const noexcept
{
    return d->password;
}

void SqlDatabase::setPassword(const QString &password) noexcept
{
    d->password = password;
}

void SqlDatabase::setPassword(QString &&password) noexcept
{
    d->password = std::move(password);
}

QString SqlDatabase::connectOptions() const noexcept
{
    return d->connectionOptions;
}

void SqlDatabase::setConnectOptions(const QString &options) noexcept
{
    d->connectionOptions = options;
}

void SqlDatabase::setConnectOptions(QString &&options) noexcept
{
    d->connectionOptions = std::move(options);
}

NumericalPrecisionPolicy SqlDatabase::numericalPrecisionPolicy() const noexcept
{
    if (d->isDriverValid()) T_LIKELY
        // The d->sqldriver is correct because of the d->isDriverValid() check
        return d->sqldriver->defaultNumericalPrecisionPolicy();

    else T_UNLIKELY
        return d->precisionPolicy;
}

void SqlDatabase::setNumericalPrecisionPolicy(
            const NumericalPrecisionPolicy precision) noexcept
{
    if (d->isDriverValid())
        // The d->sqldriver is correct because of the d->isDriverValid() check
        d->sqldriver->setDefaultNumericalPrecisionPolicy(precision);

    d->precisionPolicy = precision;
}

/* See the note near the DatabaseManager/SqlQuery::driver() method about
   driver() vs driverWeak(). */

const SqlDriver *SqlDatabase::driver() const noexcept
{
    return d->sqldriver.get();
}

std::weak_ptr<const SqlDriver> SqlDatabase::driverWeak() const noexcept
{
    return d->sqldriver;
}

/* The reason why the driverWeak() non-const method is exposed only here and isn't also
   exposed in the Drivers::SqlQuery/SqlResult is to be able to instantiate the SqlQuery
   manually like this:
const auto driver = SqlDatabase::database(Databases::MYSQL).driverWeak();
SqlQuery query(driver.lock()->createResult(driver));
*/

std::weak_ptr<SqlDriver> SqlDatabase::driverWeak() noexcept
{
    return d->sqldriver;
}

bool SqlDatabase::isThreadCheckEnabled() noexcept
{
    return SqlDatabasePrivate::checkSameThread();
}

void SqlDatabase::disableThreadCheck() noexcept
{
    SqlDatabasePrivate::checkSameThread() = false;
}

/* Transactions */

bool SqlDatabase::transaction()
{
    if (d->driver().hasFeature(SqlDriver::Transactions)) T_LIKELY
        return d->driver().beginTransaction();

    // Nothing to do, no transactions support
    else T_UNLIKELY
        return false;
}

bool SqlDatabase::commit()
{
    if (d->driver().hasFeature(SqlDriver::Transactions)) T_LIKELY
        return d->driver().commitTransaction();

    // Nothing to do, no transactions support
    else T_UNLIKELY
        return false;
}

bool SqlDatabase::rollback()
{
    if (d->driver().hasFeature(SqlDriver::Transactions)) T_LIKELY
        return d->driver().rollbackTransaction();

    // Nothing to do, no transactions support
    else T_UNLIKELY
        return false;
}

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug,
                  const TINYORM_PREPEND_NAMESPACE(Orm::Drivers::SqlDatabase) &connection)
{
    const QDebugStateSaver saver(debug);
    debug.noquote().nospace();

    if (!connection.isValid()) {
        debug << "SqlDatabase(invalid)";
        return debug;
    }

    /* The SqlDatabase output format is different from SqlRecord or SqlField that use
       ': ' instead of = but this is correct, it's much better to print these connection
       arguments with = because psql or mysql client applications use this format too. */
    debug << "SqlDatabase("
          << "driver=\""   << connection.driverName()   << "\", "
          << "database=\"" << connection.databaseName() << "\", "
          << "host=\""     << connection.hostName()     << "\", "
          << "port="       << connection.port()         << ", "
          << "user=\""     << connection.userName()     << "\", "
          << "open="       << connection.isOpen()       << '"';

    if (const auto connectOptions = connection.connectOptions();
        !connectOptions.isEmpty()
    )
        debug << ", options=\"" << connectOptions << '"';

    debug << ')';

    return debug;
}
#endif
