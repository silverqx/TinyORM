#include "orm/db.hpp"

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

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
