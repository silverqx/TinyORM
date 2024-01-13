#include "orm/drivers/sqldatabase.hpp"

#ifndef QT_NO_DEBUG_STREAM
#  include <QDebug>
#endif

#include <orm/macros/likely.hpp>

#include "orm/drivers/sqldatabase_p.hpp"
#include "orm/drivers/sqldriver.hpp"
#include "orm/drivers/sqlerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

/* private */

SqlDatabase::SqlDatabase(const QString &driver)
    : d(SqlDatabasePrivate::createSqlDatabasePrivate(driver))
{}

SqlDatabase::SqlDatabase(std::unique_ptr<SqlDriver> &&driver)
    : d(SqlDatabasePrivate::createSqlDatabasePrivate(std::move(driver)))
{}

/* public */

SqlDatabase::~SqlDatabase()
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
       directly to the driver to open the connection and is then discarded.
       Because of this, only the username setter is called, all other setters must be
       called before the open() method. */

    setUserName(username);

    return d->driver()
             .open(d->databaseName, username, password, d->hostName, d->port,
                   d->connectionOptions);
}

void SqlDatabase::close()
{
    if (!isValid())
        return;

    d->driver().close();
}

/* Getters / Setters */

bool SqlDatabase::isOpen() const
{
    return d->driver().isOpen();
}

bool SqlDatabase::isOpenError() const
{
    return d->driver().isOpenError();
}

bool SqlDatabase::isValid() const
{
    /* The SqlDatabase connection can be invalid when the SqlDatabase local copy is
       created and the removeDatabase() -> invalidateConnection() -> d.reset() method is
       called while this local copy is still in scope (this is the only case).
       Creating an invalid SqlDatabase instance manually by an user isn't possible because
       the default constructor is private. */
    return d && d->isDriverValid();
}

SqlError SqlDatabase::lastError() const
{
    return d->driver().lastError();
}

QString SqlDatabase::driverName() const
{
    return d->driverName;
}

QString SqlDatabase::connectionName() const
{
    return d->connectionName;
}

QString SqlDatabase::hostName() const
{
    return d->hostName;
}

void SqlDatabase::setHostName(const QString &host)
{
    d->hostName = host;
}

int SqlDatabase::port() const
{
    return d->port;
}

void SqlDatabase::setPort(int port)
{
    d->port = port;
}

QString SqlDatabase::databaseName() const
{
    return d->databaseName;
}

void SqlDatabase::setDatabaseName(const QString &database)
{
    d->databaseName = database;
}

QString SqlDatabase::userName() const
{
    return d->username;
}

void SqlDatabase::setUserName(const QString &username)
{
    d->username = username;
}

QString SqlDatabase::password() const
{
    return d->password;
}

void SqlDatabase::setPassword(const QString &password)
{
    d->password = password;
}

QString SqlDatabase::connectOptions() const
{
    return d->connectionOptions;
}

void SqlDatabase::setConnectOptions(const QString &options)
{
    d->connectionOptions = options;
}

NumericalPrecisionPolicy SqlDatabase::numericalPrecisionPolicy() const
{
    if (d->isDriverValid()) T_LIKELY
        // The d->sqldriver is correct because of the d->isDriverValid() check
        return d->sqldriver->defaultNumericalPrecisionPolicy();

    else T_UNLIKELY
        return d->precisionPolicy;
}

void
SqlDatabase::setNumericalPrecisionPolicy(const NumericalPrecisionPolicy precision)
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

void SqlDatabase::disableThreadCheck() noexcept
{
    SqlDatabasePrivate::checkDifferentThread() = false;
}

/* Transactions */

bool SqlDatabase::transaction()
{
    // Nothing to do, no transactions support
    if (!d->driver().hasFeature(SqlDriver::Transactions)) T_UNLIKELY
        return false;

    else T_LIKELY
        return d->driver().beginTransaction();
}

bool SqlDatabase::commit()
{
    // Nothing to do, no transactions support
    if (!d->driver().hasFeature(SqlDriver::Transactions)) T_UNLIKELY
        return false;

    else T_LIKELY
        return d->driver().commitTransaction();
}

bool SqlDatabase::rollback()
{
    // Nothing to do, no transactions support
    if (!d->driver().hasFeature(SqlDriver::Transactions)) T_UNLIKELY
        return false;

    else T_LIKELY
        return d->driver().rollbackTransaction();
}

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug,
                  const TINYORM_PREPEND_NAMESPACE(Orm::Drivers::SqlDatabase) &connection)
{
    QDebugStateSaver saver(debug);
    debug.noquote().nospace();

    // CUR drivers test and finish, also SqlError silverqx
    if (!connection.isValid()) {
        debug << "SqlDatabase(invalid)";
        return debug;
    }

    debug << "SqlDatabase("
          << "driver=\""   << connection.driverName()   << "\", "
          << "database=\"" << connection.databaseName() << "\", "
          << "host=\""     << connection.hostName()     << "\", "
          << "port="       << connection.port()         << ", "
          << "user=\""     << connection.userName()     << "\", "
          << "open="       << connection.isOpen()       << ')';

    return debug;
}
#endif
