#include "orm/db.hpp"

#include <QSharedPointer>

#include "orm/macros/likely.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

std::shared_ptr<DatabaseManager> DB::m_manager;

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

DatabaseConnection &DB::connection(const QString &name)
{
    return manager().connection(name);
}

DatabaseConnection &DB::on(const QString &name)
{
    return manager().connection(name);
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

QSqlDatabase DB::connectEagerly(const QString &name)
{
    return manager().connectEagerly(name);
}

QStringList DB::supportedDrivers()
{
    return manager().supportedDrivers();
}

QStringList DB::connectionNames()
{
    return manager().connectionNames();
}

QStringList DB::openedConnectionNames()
{
    return manager().openedConnectionNames();
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

QSharedPointer<QueryBuilder>
DB::table(const QString &table, const QString &connection)
{
    return manager().connection(connection).table(table);
}

QSharedPointer<QueryBuilder>
DB::tableAs(const QString &table, const QString &as, const QString &connection)
{
    return manager().connection(connection).table(table, as);
}

QSharedPointer<QueryBuilder>
DB::query(const QString &connection)
{
    return manager().connection(connection).query();
}

QSqlQuery DB::qtQuery(const QString &connection)
{
    return manager().connection(connection).getQtQuery();
}

QSqlQuery
DB::select(const QString &query, const QVector<QVariant> &bindings)
{
    return manager().connection().select(query, bindings);
}

QSqlQuery
DB::selectOne(const QString &query, const QVector<QVariant> &bindings)
{
    return manager().connection().selectOne(query, bindings);
}

QSqlQuery
DB::insert(const QString &query, const QVector<QVariant> &bindings)
{
    return manager().connection().insert(query, bindings);
}

std::tuple<int, QSqlQuery>
DB::update(const QString &query, const QVector<QVariant> &bindings)
{
    return manager().connection().update(query, bindings);
}

std::tuple<int, QSqlQuery>
DB::remove(const QString &query, const QVector<QVariant> &bindings)
{
    return manager().connection().remove(query, bindings);
}

QSqlQuery
DB::statement(const QString &query, const QVector<QVariant> &bindings)
{
    return manager().connection().statement(query, bindings);
}

std::tuple<int, QSqlQuery>
DB::affectingStatement(const QString &query, const QVector<QVariant> &bindings)
{
    return manager().connection().affectingStatement(query, bindings);
}

QSqlQuery DB::unprepared(const QString &query)
{
    return manager().connection().unprepared(query);
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

std::shared_ptr<QVector<Log>>
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

std::size_t DB::getQueryLogOrder()
{
    return manager().getQueryLogOrder();
}

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

QVector<Log>
DB::pretend(const std::function<void()> &callback, const QString &connection)
{
    return manager().connection(connection).pretend(callback);
}

QVector<Log>
DB::pretend(const std::function<void(DatabaseConnection &)> &callback,
            const QString &connection)
{
    return manager().connection(connection).pretend(callback);
}

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

DatabaseManager &DB::manager()
{
    if (m_manager) T_LIKELY
        return *m_manager;

    else T_UNLIKELY
        return *(m_manager = DatabaseManager::instance());
}

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE
