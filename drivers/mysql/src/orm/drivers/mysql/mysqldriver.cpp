#include "orm/drivers/mysql/mysqldriver.hpp"

#include "orm/drivers/exceptions/logicerror.hpp"
#include "orm/drivers/exceptions/sqltransactionerror.hpp"
#include "orm/drivers/mysql/mysqlconstants_p.hpp"
#include "orm/drivers/mysql/mysqldriver_p.hpp"
#include "orm/drivers/mysql/mysqlresult.hpp"
#include "orm/drivers/mysql/mysqlutils_p.hpp"
#include "orm/drivers/sqlquery.hpp"
#include "orm/drivers/sqlrecord.hpp"
#include "orm/drivers/utils/type_p.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Drivers::MySql::Constants::BACKTICK;
using Orm::Drivers::MySql::Constants::DOT;
using Orm::Drivers::MySql::Constants::QMYSQL;

using MySqlUtils = Orm::Drivers::MySql::MySqlUtilsPrivate;

namespace Orm::Drivers::MySql
{

/* public */

MySqlDriver::MySqlDriver()
    : SqlDriver(std::make_unique<MySqlDriverPrivate>())
{}

/* Can' be inline because of the TinyMySql loadable module, to destroy the MySqlDriver
   instance from the same DLL where it was initially instantiated. */
MySqlDriver::~MySqlDriver() = default;

bool MySqlDriver::open(
        const QString &database, const QString &username, const QString &password,
        const QString &host, const int port, const QString &options)
{
    Q_D(MySqlDriver);

    if (isOpen())
        throw Exceptions::LogicError(
                u"The '%1' MySQL database connection is already open, first close "
                 "the connection and then open it again or call the reopen() method, "
                 "in %2()."_s.arg(d->connectionName, __tiny_func__));

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

bool MySqlDriver::hasFeature(const DriverFeature feature) const
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
#ifndef TINYDRIVERS_DEBUG
        throw Exceptions::RuntimeError(u"Unexpected value for enum DriverFeature."_s);
#else
        Q_UNREACHABLE();
#endif
    }
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

/* These 3 transaction-related methods can't be const because SQLite and Postgres drivers
   will call exec(QString). */

bool MySqlDriver::beginTransaction()
{
    Q_D(MySqlDriver);

    // No need to check for isOpen() connection, it's done in supportsTransactions()

    if (mysql_query(d->mysql, "START TRANSACTION") == 0)
        return true;

    throw Exceptions::SqlTransactionError(
                u"Unable to start transaction for '%1' MySQL database connection "
                 "in %2()."_s.arg(d->connectionName, __tiny_func__),
                MySqlUtils::prepareMySqlError(d->mysql));
}

bool MySqlDriver::commitTransaction()
{
    Q_D(MySqlDriver);

    // No need to check for isOpen() connection, it's done in supportsTransactions()

    if (mysql_query(d->mysql, "COMMIT") == 0)
        return true;

    throw Exceptions::SqlTransactionError(
                u"Unable to commit transaction for '%1' MySQL database connection "
                 "in %2()."_s.arg(d->connectionName, __tiny_func__),
                MySqlUtils::prepareMySqlError(d->mysql));
}

bool MySqlDriver::rollbackTransaction()
{
    Q_D(MySqlDriver);

    // No need to check for isOpen() connection, it's done in supportsTransactions()

    if (mysql_query(d->mysql, "ROLLBACK") == 0)
        return true;

    throw Exceptions::SqlTransactionError(
                u"Unable to rollback transaction for '%1' MySQL database connection "
                 "in %2()."_s.arg(d->connectionName, __tiny_func__),
                MySqlUtils::prepareMySqlError(d->mysql));
}

/* Others */

QString MySqlDriver::escapeIdentifier(const QString &identifier,
                                      const IdentifierType type) const
{
    // It should have the same implementation as BaseGrammar::wrap()
    // NOTE API different, we are also escaping an empty identifier and we checking * silverqx
    // Nothing to do, already escaped or * column shorthand used
    if ((type == FieldName && identifier == u'*') ||
        isIdentifierEscaped(identifier, type)
    )
        return identifier;

    auto escapedIdentifier = identifier;

    escapedIdentifier.replace(BACKTICK, u"``"_s);
    escapedIdentifier.replace(DOT, u"`.`"_s);

    return BACKTICK % escapedIdentifier % BACKTICK;
}

bool MySqlDriver::isIdentifierEscaped(const QString &identifier,
                                      const IdentifierType /*unused*/) const
{
    // NOTE API different, we are considering an empty identifier silverqx
    /* An empty identifier like `` must also be considered as escaped. Also, ignoring
       the ANSI_QUOTES mode as it's very rare. */
    return identifier.size() >= 2 &&
           identifier.startsWith(BACKTICK) &&
           identifier.endsWith(BACKTICK);
}

std::unique_ptr<SqlResult> MySqlDriver::createResult() const
{
    /* We must use the const_cast<> as the weak_from_this() return type is controlled
       by the current method const-nes, what means it's only our implementation detail
       that we have to use the const_cast<> as we can't control this. Also, it's better
       to have the same const-nes for the createResult() as defined in the QtSql.
       See NOTES.txt[std::enable_shared_from_this<SqlDriver>] for more info. */
    return std::make_unique<MySqlResult>(
                const_cast<MySqlDriver &>(*this).weak_from_this()); // NOLINT(cppcoreguidelines-pro-type-const-cast)
}

SqlRecord MySqlDriver::record(const QString &table) const
{
    return selectAllColumnsWithLimit0(table).record(false);
}

SqlRecord MySqlDriver::recordWithDefaultValues(const QString &table) const
{
    return selectAllColumnsWithLimit0(table).recordAllColumns(true);
}

} // namespace Orm::Drivers::MySql

TINYORM_END_COMMON_NAMESPACE
