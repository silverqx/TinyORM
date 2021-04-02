#include "orm/db.hpp"

#include <QSharedPointer>

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

DatabaseManager *DB::m_manager = nullptr;

DatabaseManager &DB::manager()
{
    if (!m_manager)
        m_manager = DatabaseManager::instance();

    return *m_manager;
}

std::unique_ptr<DatabaseManager>
DB::create(const QVariantHash &config, const QString &connection)
{
    return DatabaseManager::create(config, connection);
}

std::unique_ptr<DatabaseManager>
DB::create(const ConfigurationsType &configs, const QString &defaultConnection)
{
    return DatabaseManager::create(configs, defaultConnection);
}

ConnectionInterface &DB::connection(const QString &name)
{
    return manager().connection(name);
}

DatabaseManager &
DB::addConnection(const QVariantHash &config, const QString &name)
{
    return manager().addConnection(config, name);
}

bool DB::removeConnection(QString name)
{
    return manager().removeConnection(name);
}

ConnectionInterface &DB::reconnect(QString name)
{
    return manager().reconnect(name);
}

void DB::disconnect(QString name)
{
    return manager().disconnect(name);
}

const QStringList DB::supportedDrivers()
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
    return manager().setDefaultConnection(defaultConnection);
}

DatabaseManager &
DB::setReconnector(const DatabaseManager::ReconnectorType &reconnector)
{
    return manager().setReconnector(reconnector);
}

QSharedPointer<QueryBuilder>
DB::table(const QString &table, const QString &as, const QString &connection)
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

std::tuple<bool, QSqlQuery>
DB::select(const QString &query, const QVector<QVariant> &bindings)
{
    return manager().connection().select(query, bindings);
}

std::tuple<bool, QSqlQuery>
DB::selectOne(const QString &query, const QVector<QVariant> &bindings)
{
    return manager().connection().selectOne(query, bindings);
}

std::tuple<bool, QSqlQuery>
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

std::tuple<bool, QSqlQuery>
DB::statement(const QString &query, const QVector<QVariant> &bindings)
{
    return manager().connection().statement(query, bindings);
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

bool DB::savepoint(const size_t id, const QString &connection)
{
    return manager().connection(connection).savepoint(id);
}

bool DB::rollbackToSavepoint(const QString &id, const QString &connection)
{
    return manager().connection(connection).rollbackToSavepoint(id);
}

bool DB::rollbackToSavepoint(const size_t id, const QString &connection)
{
    return manager().connection(connection).rollbackToSavepoint(id);
}

uint DB::transactionLevel(const QString &connection)
{
    return manager().connection(connection).transactionLevel();
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
    return manager().enableAllElapsedCounters();
}

void DB::disableAllElapsedCounters()
{
    return manager().disableAllElapsedCounters();
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
    return manager().resetAllElapsedCounters();
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
    return manager().enableAllStatementCounters();
}

void DB::disableAllStatementCounters()
{
    return manager().disableAllStatementCounters();
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
    return manager().resetAllStatementCounters();
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

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
