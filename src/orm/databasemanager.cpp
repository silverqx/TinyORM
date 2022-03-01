#include "orm/databasemanager.hpp"

#include "orm/concerns/hasconnectionresolver.hpp"
#include "orm/exceptions/invalidargumenterror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

// CUR revisit silverqx
// CUR also check shared_ptr m_instance, if it is freed at app quit silverqx♦
/* This is needed because of the std::unique_ptr is used in the m_connections
   data member 😲, and when this dtor is not defined in the cpp, it will be
   generated by the compiler as inline dtor, what cause a compile error. */
DatabaseManager::~DatabaseManager() = default;

std::shared_ptr<DatabaseManager> DatabaseManager::m_instance;

// FUTURE add support for ::read and ::write db connections silverx
// TODO implement RepositoryFactory silverqx
DatabaseManager::DatabaseManager(const QString &defaultConnection)
{
    Configuration::defaultConnection = defaultConnection;

    // Set up the DatabaseManager as a connection resolver
    Concerns::HasConnectionResolver::setConnectionResolver(this);

    setupDefaultReconnector();
}

DatabaseManager::DatabaseManager(const QVariantHash &config, const QString &name,
                                 const QString &defaultConnection)
    : DatabaseManager(defaultConnection)
{
    addConnection(config, name);
}

DatabaseManager::DatabaseManager(const ConfigurationsType &configs,
                                 const QString &defaultConnection)
    : DatabaseManager(defaultConnection)
{
    (*m_configuration) = configs;
}

DatabaseManager &DatabaseManager::setupDefaultReconnector()
{
    m_reconnector = [this](const DatabaseConnection &connection)
    {
        reconnect(connection.getName());
    };

    return *this;
}

std::shared_ptr<DatabaseManager>
DatabaseManager::create(const QString &defaultConnection)
{
    checkInstance();

    return m_instance = std::shared_ptr<DatabaseManager>(
                            new DatabaseManager(defaultConnection));
}

// CUR add 'createLazy = true' paramater / or open = false?, add support to create eager connection silverqx
std::shared_ptr<DatabaseManager>
DatabaseManager::create(const QVariantHash &config, const QString &connection)
{
    checkInstance();

    return m_instance = std::shared_ptr<DatabaseManager>(
                            new DatabaseManager(config, connection, connection));
}

std::shared_ptr<DatabaseManager>
DatabaseManager::create(const ConfigurationsType &configs,
                        const QString &defaultConnection)
{
    checkInstance();

    return m_instance = std::shared_ptr<DatabaseManager>(
                            new DatabaseManager(configs, defaultConnection));
}

QSharedPointer<QueryBuilder>
DatabaseManager::table(
        const QString &table, const QString &as, const QString &connection)
{
    return this->connection(connection).table(table, as);
}

QSharedPointer<QueryBuilder>
DatabaseManager::query(const QString &connection)
{
    return this->connection(connection).query();
}

QSqlQuery DatabaseManager::qtQuery(const QString &connection)
{
    return this->connection(connection).getQtQuery();
}

QSqlQuery
DatabaseManager::select(const QString &query, const QVector<QVariant> &bindings)
{
    return connection().select(query, bindings);
}

QSqlQuery
DatabaseManager::selectOne(const QString &query, const QVector<QVariant> &bindings)
{
    return connection().selectOne(query, bindings);
}

QSqlQuery
DatabaseManager::insert(const QString &query, const QVector<QVariant> &bindings)
{
    return connection().insert(query, bindings);
}

std::tuple<int, QSqlQuery>
DatabaseManager::update(const QString &query, const QVector<QVariant> &bindings)
{
    return connection().update(query, bindings);
}

std::tuple<int, QSqlQuery>
DatabaseManager::remove(const QString &query, const QVector<QVariant> &bindings)
{
    return connection().remove(query, bindings);
}

QSqlQuery
DatabaseManager::statement(const QString &query, const QVector<QVariant> &bindings)
{
    return connection().statement(query, bindings);
}

std::tuple<int, QSqlQuery>
DatabaseManager::affectingStatement(const QString &query,
                                    const QVector<QVariant> &bindings)
{
    return connection().affectingStatement(query, bindings);
}

QSqlQuery DatabaseManager::unprepared(const QString &query)
{
    return connection().unprepared(query);
}

bool DatabaseManager::beginTransaction()
{
    return connection().beginTransaction();
}

bool DatabaseManager::commit()
{
    return connection().commit();
}

bool DatabaseManager::rollBack()
{
    return connection().rollBack();
}

bool DatabaseManager::savepoint(const QString &id)
{
    return connection().savepoint(id);
}

bool DatabaseManager::savepoint(const std::size_t id)
{
    return connection().savepoint(id);
}

bool DatabaseManager::rollbackToSavepoint(const QString &id)
{
    return connection().rollbackToSavepoint(id);
}

bool DatabaseManager::rollbackToSavepoint(const std::size_t id)
{
    return connection().rollbackToSavepoint(id);
}

size_t DatabaseManager::transactionLevel()
{
    return connection().transactionLevel();
}

namespace
{
    const auto *const InstanceExceptionMessage =
            "The DatabaseManager instance has not yet been created, create it "
            "by DB::create() method.";
} // namespace

std::shared_ptr<DatabaseManager> DatabaseManager::instance()
{
    if (m_instance)
        return m_instance;

    throw Exceptions::RuntimeError(InstanceExceptionMessage);
}

DatabaseManager &DatabaseManager::reference()
{
    if (m_instance)
        return *m_instance;

    throw Exceptions::RuntimeError(InstanceExceptionMessage);
}

DatabaseConnection &DatabaseManager::connection(const QString &name)
{
    const auto &connectionName = parseConnectionName(name);

    /* If we haven't created this connection, we'll create it based on the provided
       config. Once we've created the connections we will configure it. */
    (*m_connections).try_emplace(connectionName,
                                 configure(makeConnection(connectionName)));

    return *(*m_connections)[connectionName];
}

DatabaseManager &
DatabaseManager::addConnection(const QVariantHash &config, const QString &name)
{
    if ((*m_configuration).contains(name))
        throw Exceptions::InvalidArgumentError(
                QStringLiteral("The database connection '%1' already exists.")
                .arg(name));

    (*m_configuration).insert(name, config);

    return *this;
}

DatabaseManager &
DatabaseManager::addConnections(const ConfigurationsType &configs)
{
    for (auto itConfig = configs.cbegin(); itConfig != configs.cend(); ++itConfig)
        addConnection(itConfig.value(), itConfig.key());

    return *this;
}

DatabaseManager &
DatabaseManager::addConnections(const ConfigurationsType &configs,
                                const QString &defaultConnection)
{
    addConnections(configs);

    Configuration::defaultConnection = defaultConnection;

    return *this;
}

bool DatabaseManager::removeConnection(const QString &name)
{
    const auto &name_ = parseConnectionName(name);

    // Connection with this name doesn't exist
    if (!connectionNames().contains(name_))
        return false;

    /* If currently removed connection is the default connection, then reset default
       connection. */
    const auto resetDefaultConnection_ = [this, &name]
    {
        if (Configuration::defaultConnection == name)
            resetDefaultConnection();
    };

    // Not connected
    if (!(*m_connections).contains(name_)) {
        (*m_configuration).remove(name_);
        resetDefaultConnection_();
        return true;
    }

    // Disconnect first to be nice 😁 and safe 😂
    (*m_connections).find(name_)->second->disconnect();

    /* If connection was not removed, return false and don't remove Qt's database
       connection and also don't remove connection configuration. */
    if ((*m_connections).erase(name_) == 0)
        return false;

    // Remove Qt's database connection
    QSqlDatabase::removeDatabase(name_);
    // Also remove configuration
    (*m_configuration).remove(name_);

    resetDefaultConnection_();

    return true;
}

DatabaseConnection &DatabaseManager::reconnect(const QString &name)
{
    const auto &name_ = parseConnectionName(name);

    disconnect(name_);

    if (!(*m_connections).contains(name_))
        return connection(name_);

    return refreshQtConnections(name_);
}

void DatabaseManager::disconnect(const QString &name) const
{
    const auto &name_ = parseConnectionName(name);

    if (!(*m_connections).contains(name_))
        return;

    (*m_connections).find(name_)->second->disconnect();
}

QStringList DatabaseManager::supportedDrivers() const
{
    // FUTURE add method to not only supported drivers, but also check if driver is available/loadable by qsqldatabase silverqx
    // aaaaaaaaaaaaaachjo 🤔😁 -- 4 months later, looks much better, right?
    return {QMYSQL, QPSQL, QSQLITE};
}

QStringList DatabaseManager::connectionNames() const
{
    return (*m_configuration).keys();
}

QStringList DatabaseManager::openedConnectionNames() const
{
    QStringList names;
    // TODO overflow, add check code https://stackoverflow.com/questions/22184403/how-to-cast-the-size-t-to-double-or-int-c/22184657#22184657 silverqx
    names.reserve(static_cast<int>((*m_connections).size()));

    for (const auto &connection : (*m_connections))
        names << connection.first;

    return names;
}

const QString &
DatabaseManager::getDefaultConnection() const
{
    return Configuration::defaultConnection;
}

void DatabaseManager::setDefaultConnection(const QString &defaultConnection)
{
    Configuration::defaultConnection = defaultConnection;
}

void DatabaseManager::resetDefaultConnection()
{
    Configuration::defaultConnection = Configuration::defaultConnectionName;
}

DatabaseManager &
DatabaseManager::setReconnector(const ReconnectorType &reconnector)
{
    m_reconnector = reconnector;

    return *this;
}

bool DatabaseManager::countingElapsed(const QString &connection)
{
    return this->connection(connection).countingElapsed();
}

DatabaseConnection &DatabaseManager::enableElapsedCounter(const QString &connection)
{
    return this->connection(connection).enableElapsedCounter();
}

DatabaseConnection &DatabaseManager::disableElapsedCounter(const QString &connection)
{
    return this->connection(connection).disableElapsedCounter();
}

qint64 DatabaseManager::getElapsedCounter(const QString &connection)
{
    return this->connection(connection).getElapsedCounter();
}

qint64 DatabaseManager::takeElapsedCounter(const QString &connection)
{
    return this->connection(connection).takeElapsedCounter();
}

DatabaseConnection &DatabaseManager::resetElapsedCounter(const QString &connection)
{
    return this->connection(connection).resetElapsedCounter();
}

bool DatabaseManager::anyCountingElapsed()
{
    return std::ranges::any_of(openedConnectionNames(),
                               [this](const auto &connectionName)
    {
        return connection(connectionName).countingElapsed();
    });
}

void DatabaseManager::enableAllElapsedCounters()
{
    enableElapsedCounters(openedConnectionNames());
}

void DatabaseManager::disableAllElapsedCounters()
{
    disableElapsedCounters(openedConnectionNames());
}

qint64 DatabaseManager::getAllElapsedCounters()
{
    return getElapsedCounters(openedConnectionNames());
}

qint64 DatabaseManager::takeAllElapsedCounters()
{
    return takeElapsedCounters(openedConnectionNames());
}

void DatabaseManager::resetAllElapsedCounters()
{
    resetElapsedCounters(openedConnectionNames());
}

void DatabaseManager::enableElapsedCounters(const QStringList &connections)
{
    for (const auto &connectionName : connections)
        connection(connectionName).enableElapsedCounter();
}

void DatabaseManager::disableElapsedCounters(const QStringList &connections)
{
    for (const auto &connectionName : connections)
        connection(connectionName).disableElapsedCounter();
}

qint64 DatabaseManager::getElapsedCounters(const QStringList &connections)
{
    if (!anyCountingElapsed())
        return -1;

    qint64 elapsed = 0;

    for (const auto &connectionName : connections) {
        const auto &connection = this->connection(connectionName);

        if (connection.countingElapsed())
            elapsed += connection.getElapsedCounter();
    }

    return elapsed;
}

qint64 DatabaseManager::takeElapsedCounters(const QStringList &connections)
{
    if (!anyCountingElapsed())
        return -1;

    qint64 elapsed = 0;

    for (const auto &connectionName : connections) {
        auto &connection = this->connection(connectionName);

        if (connection.countingElapsed())
            elapsed += connection.takeElapsedCounter();
    }

    return elapsed;
}

void DatabaseManager::resetElapsedCounters(const QStringList &connections)
{
    for (const auto &connectionName : connections) {
        auto &connection = this->connection(connectionName);

        if (connection.countingElapsed())
            connection.resetElapsedCounter();
    }
}

bool DatabaseManager::countingStatements(const QString &connection)
{
    return this->connection(connection).countingStatements();
}

DatabaseConnection &DatabaseManager::enableStatementsCounter(const QString &connection)
{
    return this->connection(connection).enableStatementsCounter();
}

DatabaseConnection &DatabaseManager::disableStatementsCounter(const QString &connection)
{
    return this->connection(connection).disableStatementsCounter();
}

const StatementsCounter &DatabaseManager::getStatementsCounter(const QString &connection)
{
    return this->connection(connection).getStatementsCounter();
}

StatementsCounter DatabaseManager::takeStatementsCounter(const QString &connection)
{
    return this->connection(connection).takeStatementsCounter();
}

DatabaseConnection &DatabaseManager::resetStatementsCounter(const QString &connection)
{
    return this->connection(connection).resetStatementsCounter();
}

bool DatabaseManager::anyCountingStatements()
{
    return std::ranges::any_of(openedConnectionNames(),
                               [this](const auto &connectionName)
    {
        return connection(connectionName).countingStatements();
    });
}

void DatabaseManager::enableAllStatementCounters()
{
    enableStatementCounters(openedConnectionNames());
}

void DatabaseManager::disableAllStatementCounters()
{
    disableStatementCounters(openedConnectionNames());
}

StatementsCounter DatabaseManager::getAllStatementCounters()
{
    return getStatementCounters(openedConnectionNames());
}

StatementsCounter DatabaseManager::takeAllStatementCounters()
{
    return takeStatementCounters(openedConnectionNames());
}

void DatabaseManager::resetAllStatementCounters()
{
    resetStatementCounters(openedConnectionNames());
}

void DatabaseManager::enableStatementCounters(const QStringList &connections)
{
    for (const auto &connectionName : connections)
        connection(connectionName).enableStatementsCounter();
}

void DatabaseManager::disableStatementCounters(const QStringList &connections)
{
    for (const auto &connectionName : connections)
        connection(connectionName).disableStatementsCounter();
}

StatementsCounter DatabaseManager::getStatementCounters(const QStringList &connections)
{
    StatementsCounter counter;

    if (!anyCountingStatements())
        return counter;

    for (const auto &connectionName : connections) {
        const auto &connection = this->connection(connectionName);

        if (connection.countingStatements()) {
            const auto &counter_ = connection.getStatementsCounter();

            counter.normal        += counter_.normal;
            counter.affecting     += counter_.affecting;
            counter.transactional += counter_.transactional;
        }
    }

    return counter;
}

StatementsCounter DatabaseManager::takeStatementCounters(const QStringList &connections)
{
    StatementsCounter counter;

    if (!anyCountingStatements())
        return counter;

    for (const auto &connectionName : connections) {
        auto &connection = this->connection(connectionName);

        if (connection.countingElapsed()) {
            const auto counter_ = connection.takeStatementsCounter();

            counter.normal        += counter_.normal;
            counter.affecting     += counter_.affecting;
            counter.transactional += counter_.transactional;
        }
    }

    return counter;
}

void DatabaseManager::resetStatementCounters(const QStringList &connections)
{
    for (const auto &connectionName : connections) {
        auto &connection = this->connection(connectionName);

        if (connection.countingElapsed())
            connection.resetStatementsCounter();
    }
}

std::shared_ptr<QVector<Log>>
DatabaseManager::getQueryLog(const QString &connection)
{
    return this->connection(connection).getQueryLog();
}

void DatabaseManager::flushQueryLog(const QString &connection)
{
    this->connection(connection).flushQueryLog();
}

void DatabaseManager::enableQueryLog(const QString &connection)
{
    this->connection(connection).enableQueryLog();
}

void DatabaseManager::disableQueryLog(const QString &connection)
{
    this->connection(connection).disableQueryLog();
}

bool DatabaseManager::logging(const QString &connection)
{
    return this->connection(connection).logging();
}

std::size_t DatabaseManager::getQueryLogOrder()
{
    return DatabaseConnection::getQueryLogOrder();
}

QString DatabaseManager::driverName(const QString &connection)
{
    return this->connection(connection).driverName();
}

const QString &
DatabaseManager::driverNamePrintable(const QString &connection)
{
    return this->connection(connection).driverNamePrintable();
}

const QString &
DatabaseManager::databaseName(const QString &connection)
{
    return this->connection(connection).getDatabaseName();
}

const QString &
DatabaseManager::hostName(const QString &connection)
{
    return this->connection(connection).getHostName();
}

bool DatabaseManager::isOpen(const QString &connection)
{
    return this->connection(connection).isOpen();
}

QVector<Log>
DatabaseManager::pretend(const std::function<void()> &callback,
                         const QString &connection)
{
    return this->connection(connection).pretend(callback);
}

QVector<Log>
DatabaseManager::pretend(const std::function<void(DatabaseConnection &)> &callback,
                         const QString &connection)
{
    return this->connection(connection).pretend(callback);
}

bool DatabaseManager::getRecordsHaveBeenModified(const QString &connection)
{
    return this->connection(connection).getRecordsHaveBeenModified();
}

void DatabaseManager::recordsHaveBeenModified(const bool value,
                                              const QString &connection)
{
    this->connection(connection).recordsHaveBeenModified(value);
}

void DatabaseManager::forgetRecordModificationState(const QString &connection)
{
    this->connection(connection).forgetRecordModificationState();
}

const QString &
DatabaseManager::parseConnectionName(const QString &name) const
{
    return name.isEmpty() ? getDefaultConnection() : name;
}

std::unique_ptr<DatabaseConnection>
DatabaseManager::makeConnection(const QString &name)
{
    auto &config = configuration(name);

    // FUTURE add support for extensions silverqx

    return m_factory.make(config, name);
}

/* Can not be const because I'm modifying the Configuration (QVariantHash)
   in ConnectionFactory. */
QVariantHash &
DatabaseManager::configuration(const QString &name)
{
    const auto &name_ = parseConnectionName(name);

    /* Get the database connection configuration by the given name.
       If the configuration doesn't exist, we'll throw an exception and bail. */
    if (!(*m_configuration).contains(name_))
        throw Exceptions::InvalidArgumentError(
                QStringLiteral("Database connection '%1' not configured.")
                .arg(name_));

    return (*m_configuration)[name_]; // clazy:exclude=detaching-member

    // TODO add ConfigurationUrlParser silverqx
//    return (new ConfigurationUrlParser)
//                ->parseConfiguration($config);
}

std::unique_ptr<DatabaseConnection>
DatabaseManager::configure(std::unique_ptr<DatabaseConnection> &&connection) const
{
    /* Here we'll set a reconnector lambda. This reconnector can be any callable
       so we will set a Closure to reconnect from this manager with the name of
       the connection, which will allow us to reconnect from OUR connections. */
    connection->setReconnector(m_reconnector);

    return std::move(connection);
}

DatabaseConnection &
DatabaseManager::refreshQtConnections(const QString &name)
{
    const auto &name_ = parseConnectionName(name);

    /* Make OUR new connection and copy the connection resolver from this new
       connection to the current connection, this ensure that the connection
       will be resolved/connected again lazily. */
    auto fresh = configure(makeConnection(name_));

    return (*m_connections)[name_]->setQtConnectionResolver(
                fresh->getQtConnectionResolver());
}

void DatabaseManager::checkInstance()
{
    if (!m_instance)
        return;

    throw Exceptions::RuntimeError(
            "Only one instance of DatabaseManager is allowed per process.");
}

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE
