#include "orm/db.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

/* private */

std::shared_ptr<DatabaseManager> DB::m_manager;

/* public */

/* DatabaseManager factories */

std::shared_ptr<DatabaseManager>
DB::create(const QString &defaultConnection)
{
    return DatabaseManager::create(defaultConnection);
}

std::shared_ptr<DatabaseManager>
DB::create(const QVariantHash &config, const QString &connection)
{
    return DatabaseManager::create(config, connection);
}

std::shared_ptr<DatabaseManager>
DB::create(const ConfigurationsType &configs, const QString &defaultConnection)
{
    return DatabaseManager::create(configs, defaultConnection);
}

/* Proxy methods to the DatabaseConnection */

std::shared_ptr<QueryBuilder>
DB::table(const QString &table, const QString &connection)
{
    return manager().connection(connection).table(table);
}

std::shared_ptr<QueryBuilder>
DB::tableAs(const QString &table, const QString &as, const QString &connection)
{
    return manager().connection(connection).table(table, as);
}

std::shared_ptr<QueryBuilder>
DB::query(const QString &connection)
{
    return manager().connection(connection).query();
}

TSqlQuery DB::sqlQuery(const QString &connection)
{
    return manager().connection(connection).getSqlQuery();
}

SqlQuery
DB::select(const QString &query, QList<QVariant> bindings,
           const QString &connection)
{
    return manager().connection(connection).select(query, std::move(bindings));
}

SqlQuery
DB::selectFromWriteConnection(const QString &query, QList<QVariant> bindings,
                              const QString &connection)
{
    return manager().connection(connection)
                    .selectFromWriteConnection(query, std::move(bindings));
}

SqlQuery
DB::selectOne(const QString &query, QList<QVariant> bindings,
              const QString &connection)
{
    return manager().connection(connection).selectOne(query, std::move(bindings));
}

QVariant
DB::scalar(const QString &query, QList<QVariant> bindings,
           const QString &connection)
{
    return manager().connection(connection).scalar(query, std::move(bindings));
}

SqlQuery
DB::insert(const QString &query, QList<QVariant> bindings,
           const QString &connection)
{
    return manager().connection(connection).insert(query, std::move(bindings));
}

std::tuple<int, TSqlQuery>
DB::update(const QString &query, QList<QVariant> bindings,
           const QString &connection)
{
    return manager().connection(connection).update(query, std::move(bindings));
}

std::tuple<int, TSqlQuery>
DB::remove(const QString &query, QList<QVariant> bindings,
           const QString &connection)
{
    return manager().connection(connection).remove(query, std::move(bindings));
}

SqlQuery
DB::statement(const QString &query, QList<QVariant> bindings,
              const QString &connection)
{
    return manager().connection(connection).statement(query, std::move(bindings));
}

std::tuple<int, TSqlQuery>
DB::affectingStatement(const QString &query, QList<QVariant> bindings,
                       const QString &connection)
{
    return manager().connection(connection)
                    .affectingStatement(query, std::move(bindings));
}

SqlQuery DB::unprepared(const QString &query, const QString &connection)
{
    return manager().connection(connection).unprepared(query);
}

// NOTE api different silverqx
bool DB::beginTransaction(const QString &connection)
{
    return manager().connection(connection).beginTransaction();
}

bool DB::commit(const QString &connection)
{
    return manager().connection(connection).commit();
}

bool DB::rollBack(const QString &connection)
{
    return manager().connection(connection).rollBack();
}

bool DB::savepoint(const QString &id, const QString &connection)
{
    return manager().connection(connection).savepoint(id);
}

bool DB::savepoint(const std::size_t id, const QString &connection)
{
    return manager().connection(connection).savepoint(id);
}

bool DB::rollbackToSavepoint(const QString &id, const QString &connection)
{
    return manager().connection(connection).rollbackToSavepoint(id);
}

bool DB::rollbackToSavepoint(const std::size_t id, const QString &connection)
{
    return manager().connection(connection).rollbackToSavepoint(id);
}

size_t DB::transactionLevel(const QString &connection)
{
    return manager().connection(connection).transactionLevel();
}

bool DB::isOpen(const QString &connection)
{
    return manager().connection(connection).isOpen();
}

bool DB::pingDatabase(const QString &connection)
{
    return manager().connection(connection).pingDatabase();
}

/* See the note near the DatabaseManager/SqlQuery::driver() method about
   driver() vs driverWeak(). */

const TSqlDriver *DB::driver(const QString &connection)
{
    return manager().connection(connection).driver();
}

#ifdef TINYORM_USING_TINYDRIVERS
std::weak_ptr<const TSqlDriver> DB::driverWeak(const QString &connection)
{
    return manager().connection(connection).driverWeak();
}
#endif

/* Proxy methods to the DatabaseManager */

DatabaseConnection &DB::connection(const QString &name)
{
    return manager().connection(name);
}

DatabaseConnection &DB::on(const QString &connection)
{
    return manager().connection(connection);
}

DatabaseManager &
DB::addConnection(const QVariantHash &config, const QString &name)
{
    return manager().addConnection(config, name);
}

DatabaseManager &
DB::addConnections(const ConfigurationsType &configs)
{
    return manager().addConnections(configs);
}

DatabaseManager &
DB::addConnections(const ConfigurationsType &configs, const QString &defaultConnection)
{
    return manager().addConnections(configs, defaultConnection);
}

bool DB::removeConnection(const QString &name)
{
    return manager().removeConnection(name);
}

bool DB::containsConnection(const QString &name)
{
    return manager().containsConnection(name);
}

DatabaseConnection &DB::reconnect(const QString &name)
{
    return manager().reconnect(name);
}

void DB::disconnect(const QString &name)
{
    manager().disconnect(name);
}

void DB::connectEagerly(const QString &name)
{
    manager().connectEagerly(name);
}

QStringList DB::connectionNames()
{
    return manager().connectionNames();
}

QStringList DB::openedConnectionNames()
{
    return manager().openedConnectionNames();
}

std::size_t DB::openedConnectionsSize()
{
    return manager().openedConnectionsSize();
}

QStringList DB::supportedDrivers()
{
    return manager().supportedDrivers();
}

QStringList DB::drivers()
{
    return manager().drivers();
}

bool DB::isDriverAvailable(const QString &driverName)
{
    return manager().isDriverAvailable(driverName);
}

bool DB::isConnectionDriverAvailable(const QString &connection)
{
    return manager().isConnectionDriverAvailable(connection);
}

const QString &DB::getDefaultConnection()
{
    return manager().getDefaultConnection();
}

void DB::setDefaultConnection(const QString &defaultConnection)
{
    manager().setDefaultConnection(defaultConnection);
}

void DB::resetDefaultConnection()
{
    manager().resetDefaultConnection();
}

DatabaseManager &
DB::setReconnector(const ReconnectorType &reconnector)
{
    return manager().setReconnector(reconnector);
}

/* Getters / Setters */

QString DB::driverName(const QString &connection)
{
    return manager().connection(connection).driverName();
}

const QString &DB::driverNamePrintable(const QString &connection)
{
    return manager().connection(connection).driverNamePrintable();
}

const QString &DB::databaseName(const QString &connection)
{
    return manager().connection(connection).getDatabaseName();
}

const QString &DB::hostName(const QString &connection)
{
    return manager().connection(connection).getHostName();
}

const QtTimeZoneConfig &DB::qtTimeZone(const QString &connection)
{
    return manager().connection(connection).getQtTimeZone();
}

DatabaseConnection &
DB::setQtTimeZone(const QVariant &timezone, const QString &connection)
{
    return manager().connection(connection).setQtTimeZone(timezone);
}

DatabaseConnection &
DB::setQtTimeZone(QtTimeZoneConfig &&timezone, const QString &connection)
{
    return manager().connection(connection).setQtTimeZone(std::move(timezone));
}

bool DB::isConvertingTimeZone(const QString &connection)
{
    return manager().connection(connection).isConvertingTimeZone();
}

/* Connection configurations - saved in the DatabaseManager */

/* Difference between a connection config. saved in the DatabaseManager and
   DatabaseConnection can differ in some cases. A configuration saved
   in the DatabaseConnection is already processed by the ConnectionFactory and Connector.
   Eg. the host config. option can differ because a user can pass a QStringList of hosts,
   so in the DM will be this QStringList, but in the DatabaseConnection will be only
   the QString, a hostname to which the connection was successful. */

QVariant
DB::originalConfigValue(const QString &option, const QString &connection)
{
    return manager().originalConfigValue(option, connection);
}

const QVariantHash &DB::originalConfig(const QString &connection)
{
    return manager().originalConfig(connection);
}

size_t DB::originalConfigsSize()
{
    return manager().originalConfigsSize();
}

/* Connection configurations - proxies to the DatabaseConnection */

QVariant DB::getConfigValue(const QString &option, const QString &connection)
{
    return manager().connection(connection).getConfig(option);
}

const QVariantHash &DB::getConfig(const QString &connection)
{
    return manager().connection(connection).getConfig();
}

bool DB::hasConfigValue(const QString &option, const QString &connection)
{
    return manager().connection(connection).hasConfig(option);
}

/* Pretending */

QList<Log>
DB::pretend(const std::function<void()> &callback, const QString &connection)
{
    return manager().connection(connection).pretend(callback);
}

QList<Log>
DB::pretend(const std::function<void(DatabaseConnection &)> &callback,
            const QString &connection)
{
    return manager().connection(connection).pretend(callback);
}

/* Records were modified */

bool DB::getRecordsHaveBeenModified(const QString &connection)
{
    return manager().connection(connection).getRecordsHaveBeenModified();
}

void DB::recordsHaveBeenModified(const bool value, const QString &connection)
{
    manager().connection(connection).recordsHaveBeenModified(value);
}

void DB::forgetRecordModificationState(const QString &connection)
{
    manager().connection(connection).forgetRecordModificationState();
}

/* Logging */

std::shared_ptr<QList<Log>>
DB::getQueryLog(const QString &connection)
{
    return manager().connection(connection).getQueryLog();
}

void DB::flushQueryLog(const QString &connection)
{
    manager().connection(connection).flushQueryLog();
}

void DB::enableQueryLog(const QString &connection)
{
    manager().connection(connection).enableQueryLog();
}

void DB::disableQueryLog(const QString &connection)
{
    manager().connection(connection).disableQueryLog();
}

bool DB::logging(const QString &connection)
{
    return manager().connection(connection).logging();
}

std::size_t DB::getQueryLogOrder() noexcept
{
    return manager().getQueryLogOrder();
}

/* Queries execution time counter */

bool DB::countingElapsed(const QString &connection)
{
    return manager().connection(connection).countingElapsed();
}

DatabaseConnection &DB::enableElapsedCounter(const QString &connection)
{
    return manager().connection(connection).enableElapsedCounter();
}

DatabaseConnection &DB::disableElapsedCounter(const QString &connection)
{
    return manager().connection(connection).disableElapsedCounter();
}

qint64 DB::getElapsedCounter(const QString &connection)
{
    return manager().connection(connection).getElapsedCounter();
}

qint64 DB::takeElapsedCounter(const QString &connection)
{
    return manager().connection(connection).takeElapsedCounter();
}

DatabaseConnection &DB::resetElapsedCounter(const QString &connection)
{
    return manager().connection(connection).resetElapsedCounter();
}

bool DB::anyCountingElapsed()
{
    return manager().anyCountingElapsed();
}

void DB::enableAllElapsedCounters()
{
    manager().enableAllElapsedCounters();
}

void DB::disableAllElapsedCounters()
{
    manager().disableAllElapsedCounters();
}

qint64 DB::getAllElapsedCounters()
{
    return manager().getAllElapsedCounters();
}

qint64 DB::takeAllElapsedCounters()
{
    return manager().takeAllElapsedCounters();
}

void DB::resetAllElapsedCounters()
{
    manager().resetAllElapsedCounters();
}

void DB::enableElapsedCounters(const QStringList &connections)
{
    manager().enableElapsedCounters(connections);
}

void DB::disableElapsedCounters(const QStringList &connections)
{
    manager().disableElapsedCounters(connections);
}

qint64 DB::getElapsedCounters(const QStringList &connections)
{
    return manager().getElapsedCounters(connections);
}

qint64 DB::takeElapsedCounters(const QStringList &connections)
{
    return manager().takeElapsedCounters(connections);
}

void DB::resetElapsedCounters(const QStringList &connections)
{
    manager().resetElapsedCounters(connections);
}

/* Queries executed counter */

bool DB::countingStatements(const QString &connection)
{
    return manager().connection(connection).countingStatements();
}

DatabaseConnection &DB::enableStatementsCounter(const QString &connection)
{
    return manager().connection(connection).enableStatementsCounter();
}

DatabaseConnection &DB::disableStatementsCounter(const QString &connection)
{
    return manager().connection(connection).disableStatementsCounter();
}

const StatementsCounter &DB::getStatementsCounter(const QString &connection)
{
    return manager().connection(connection).getStatementsCounter();
}

StatementsCounter DB::takeStatementsCounter(const QString &connection)
{
    return manager().connection(connection).takeStatementsCounter();
}

DatabaseConnection &DB::resetStatementsCounter(const QString &connection)
{
    return manager().connection(connection).resetStatementsCounter();
}

bool DB::anyCountingStatements()
{
    return manager().anyCountingStatements();
}

void DB::enableAllStatementCounters()
{
    manager().enableAllStatementCounters();
}

void DB::disableAllStatementCounters()
{
    manager().disableAllStatementCounters();
}

StatementsCounter DB::getAllStatementCounters()
{
    return manager().getAllStatementCounters();
}

StatementsCounter DB::takeAllStatementCounters()
{
    return manager().takeAllStatementCounters();
}

void DB::resetAllStatementCounters()
{
    manager().resetAllStatementCounters();
}

void DB::enableStatementCounters(const QStringList &connections)
{
    manager().enableStatementCounters(connections);
}

void DB::disableStatementCounters(const QStringList &connections)
{
    manager().disableStatementCounters(connections);
}

StatementsCounter DB::getStatementCounters(const QStringList &connections)
{
    return manager().getStatementCounters(connections);
}

StatementsCounter DB::takeStatementCounters(const QStringList &connections)
{
    return manager().takeStatementCounters(connections);
}

void DB::resetStatementCounters(const QStringList &connections)
{
    manager().resetStatementCounters(connections);
}

/* DB */

void DB::free() noexcept
{
    // Reset both, DatabaseManager and DB ownership of the DatabaseManager managed object
    m_manager.reset();
    DatabaseManager::free();
}

/* private */

DatabaseManager &DB::manager()
{
    if (m_manager)
        return *m_manager;

    return *(m_manager = DatabaseManager::instance());
}

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE
