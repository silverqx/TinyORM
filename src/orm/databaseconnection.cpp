#include "orm/databaseconnection.hpp"

#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>

#include "mysql.h"

#include "orm/logquery.hpp"
#include "orm/query/querybuilder.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

const char *DatabaseConnection::defaultConnectionName = const_cast<char *>("tinyorm_default");
const char *DatabaseConnection::SAVEPOINT_NAMESPACE   = const_cast<char *>("tinyorm_savepoint");

/*!
    \class DatabaseConnection
    \brief The DatabaseConnection class handles a connection to the database.

    \ingroup database
    \inmodule Export

    Wrapper around QSqlDatabase class, many methods are only proxies with
    some error handling.
    Savepoints (nested transactions) are not managed automatically like eg
    in Laravel's Eloquent ORM, because I want to be more explicit, so when
    I need to start Savepoint, I will call savepoint() method and not
    transcation(). The same is true for rollBack(), so I will not call
    rollBack() for both, to end transaction and to end savepoint, instead,
    I will call rollBack() for transaction and rollbackToSavepoint("xx_1")
    for savepoint. This makes it clear at a glance what is happening.
*/

// TODO err, may be configurable exceptions by config or compiler directive? (completely disable exceptions by directive) silverqx
DatabaseConnection::DatabaseConnection(
        const std::function<Connectors::ConnectionName ()> &connection,
        const QString &database, const QString tablePrefix,
        const QVariantHash &config
)
    : m_qtConnectionResolver(std::move(connection))
    , m_database(database)
    , m_tablePrefix(tablePrefix)
    , m_config(config)
{}

QSharedPointer<QueryBuilder>
DatabaseConnection::table(const QString &table, const QString &as)
{
    auto builder = QSharedPointer<QueryBuilder>::create(*this, m_queryGrammar);

    builder->from(table, as);

    return builder;
}

QSharedPointer<QueryBuilder> DatabaseConnection::query()
{
    return QSharedPointer<QueryBuilder>::create(*this, m_queryGrammar);
}

bool DatabaseConnection::beginTransaction()
{
    Q_ASSERT(m_inTransaction == false);

    if (!getQtConnection().transaction())
        return false;

    m_inTransaction = true;

    return true;
}

bool DatabaseConnection::commit()
{
    Q_ASSERT(m_inTransaction);

    if (!getQtConnection().commit())
        return false;

    m_inTransaction = false;

    return true;
}

bool DatabaseConnection::rollBack()
{
    Q_ASSERT(m_inTransaction);

    if (!getQtConnection().rollback())
        return false;

    m_inTransaction = false;

    return true;
}

bool DatabaseConnection::savepoint(const QString &id)
{
    // TODO rewrite savepoint() and rollBack() with a new m_connection.statement() API silverqx
    Q_ASSERT(m_inTransaction);
    auto savePoint = getQtQuery();
    const auto query = QStringLiteral("SAVEPOINT %1_%2").arg(SAVEPOINT_NAMESPACE, id);
    // Execute a savepoint query
    const auto ok = savePoint.exec(query);
    Q_ASSERT_X(ok,
               "DatabaseConnection::savepoint(QString &id)",
               query.toUtf8().constData());
    if (!ok) {
        const auto savepointId = QStringLiteral("%1_%2").arg(SAVEPOINT_NAMESPACE, id);
        qDebug() << "Savepoint" << savepointId << "failed :"
                 << savePoint.lastError().text();
    }
    ++m_savepoints;
    return ok;
}

bool DatabaseConnection::rollbackToSavepoint(const QString &id)
{
    Q_ASSERT(m_inTransaction);
    Q_ASSERT(m_savepoints > 0);
    auto rollbackToSavepoint = getQtQuery();
    const auto query = QStringLiteral("ROLLBACK TO SAVEPOINT %1_%2")
                       .arg(SAVEPOINT_NAMESPACE, id);
    // Execute a rollback to savepoint query
    const auto ok = rollbackToSavepoint.exec(query);
    Q_ASSERT_X(ok,
               "DatabaseConnection::rollbackToSavepoint(QString &id)",
               query.toUtf8().constData());
    if (!ok) {
        const auto savepointId = QStringLiteral("%1_%2").arg(SAVEPOINT_NAMESPACE, id);
        qDebug() << "Rollback to Savepoint" << savepointId << "failed :"
                 << rollbackToSavepoint.lastError().text();
    }
    m_savepoints = std::max<int>(0, m_savepoints - 1);

    return ok;
}

std::tuple<bool, QSqlQuery>
DatabaseConnection::select(const QString &queryString,
                           const QVector<QVariant> &bindings)
{
    /* select() and statement() have the same implementation for now, but
       they don't in original Eloquent implementation.
       They will have different implementation, when I implement
       ::read ::write connections and sticky connection
       (recordsHaveBeenModified()). */
    return statement(queryString, bindings);
}

std::tuple<bool, QSqlQuery>
DatabaseConnection::selectOne(const QString &queryString,
                              const QVector<QVariant> &bindings)
{
    auto [ok, qtQuery] = statement(queryString, bindings);

    if (!ok)
        return {ok, qtQuery};

    ok = qtQuery.first();

    return {ok, qtQuery};
}

std::tuple<bool, QSqlQuery>
DatabaseConnection::insert(const QString &queryString,
                           const QVector<QVariant> &bindings)
{
    return statement(queryString, bindings);
}

std::tuple<int, QSqlQuery>
DatabaseConnection::update(const QString &queryString,
                           const QVector<QVariant> &bindings)
{
    return affectingStatement(queryString, bindings);
}

std::tuple<int, QSqlQuery>
DatabaseConnection::remove(const QString &queryString,
                           const QVector<QVariant> &bindings)
{
    return affectingStatement(queryString, bindings);
}

std::tuple<bool, QSqlQuery>
DatabaseConnection::statement(const QString &queryString,
                              const QVector<QVariant> &bindings)
{
    return run<bool>(
                queryString, bindings,
                [this](const QString &queryString, const QVector<QVariant> &bindings)
                -> std::tuple<bool, QSqlQuery>
    {
        // Prepare QSqlQuery
        auto query = prepareQuery(queryString);

        bindValues(query, prepareBindings(bindings));

        if (const auto ok = query.exec(); ok)
            return {ok, query};

        /* If an error occurs when attempting to run a query, we'll transform it
           to the exception QueryError(), which formats the error message to
           include the bindings with SQL, which will make this exception a lot
           more helpful to the developer instead of just the database's errors. */
        throw QueryError(
                    // TODO next use __FUNCTION__ in similar statements silverqx
                    QStringLiteral("Statement in %1() failed.").arg(__FUNCTION__),
                    query, bindings);
    });
}

std::tuple<int, QSqlQuery>
DatabaseConnection::affectingStatement(const QString &queryString,
                                       const QVector<QVariant> &bindings)
{
    return run<int>(
                queryString, bindings,
                [this](const QString &queryString, const QVector<QVariant> &bindings)
                -> std::tuple<bool, QSqlQuery>
    {
        // Prepare QSqlQuery
        auto query = prepareQuery(queryString);

        bindValues(query, prepareBindings(bindings));

        if (query.exec())
            return {query.numRowsAffected(), query};

        /* If an error occurs when attempting to run a query, we'll transform it
           to the exception QueryError(), which formats the error message to
           include the bindings with SQL, which will make this exception a lot
           more helpful to the developer instead of just the database's errors. */
        throw QueryError(
                    QStringLiteral("Affecting statement in %1() failed.")
                        .arg(__FUNCTION__),
                    query, bindings);
    });
}

QSqlDatabase DatabaseConnection::getQtConnection()
{
    if (!m_qtConnection) {
        // This should never happen 🤔
        Q_ASSERT(m_qtConnectionResolver);

        m_qtConnection = std::invoke(m_qtConnectionResolver);

        /* This should never happen 🤔, do this check only when the QSqlDatabase
           connection was resolved by connection resolver. */
        if (!QSqlDatabase::contains(*m_qtConnection))
            throw std::domain_error(
                    "Connection '" + m_qtConnection->toStdString() + "' doesn't exist.");
    }

    // Return the connection from QSqlDatabase connection manager
    return QSqlDatabase::database(*m_qtConnection);
}

QSqlDatabase DatabaseConnection::getRawQtConnection() const
{
    return QSqlDatabase::database(*m_qtConnection);
}

DatabaseConnection &
DatabaseConnection::setQtConnectionResolver(
        const std::function<Connectors::ConnectionName()> &resolver)
{
    /* Reset transaction and savepoints as the underlying connection will be
       disconnected and reconnected again. The m_qtConnection have to be
       reset because it indicates whether the underlying connection is active. */
    resetTransactions();

    /* m_qtConnection.reset() is called also in DatabaseConnection::disconnect(),
       because both methods are public apis.
       m_qtConnection can also be understood as m_qtConnectionWasResolved,
       because it performs two functions, saves active connection name and
       if it's not nullopt, then it means, that the database connection was
       resolved by m_qtConnectionResolver.
       If it's nullopt, then m_qtConnectionResolver should be called to
       resolve a new database connection.
       This ensures, that a database connection will be resolved lazily, only
       when actually needed. */
    m_qtConnection.reset();
    m_qtConnectionResolver = resolver;

    return *this;
}

QSqlQuery DatabaseConnection::getQtQuery()
{
    return QSqlQuery(getQtConnection());
}

// TODO perf, modify bindings directly and return reference, debug impact silverqx
QVector<QVariant>
DatabaseConnection::prepareBindings(const QVector<QVariant> &bindings) const
{
//    const auto &grammar = getQueryGrammar();

    // TODO future, here will be implemented datetime values silverqx
    return bindings;
}

void DatabaseConnection::bindValues(QSqlQuery &query,
                                    const QVector<QVariant> &bindings) const
{
    auto itBinding = bindings.constBegin();
    while (itBinding != bindings.constEnd()) {
        query.addBindValue(*itBinding);

        ++itBinding;
    }
}

void DatabaseConnection::logQuery(const QSqlQuery &query,
                                  const std::optional<quint64> elapsed = std::nullopt) const
{
    qDebug().nospace().noquote()
        << "Executed prepared query (" << (elapsed ? *elapsed : -1) << "ms, "
        << query.size() << " results, " << query.numRowsAffected()
        << " affected) : " << parseExecutedQuery(query);
}

bool DatabaseConnection::pingDatabase()
{
    auto qtConnection = getQtConnection();

    const auto getMysqlHandle = [&qtConnection]() -> MYSQL *
    {
        if (auto driverHandle = qtConnection.driver()->handle();
            qstrcmp(driverHandle.typeName(), "MYSQL*") == 0
        )
            return *static_cast<MYSQL **>(driverHandle.data());

        return nullptr;
    };

    const auto mysqlPing = [getMysqlHandle]() -> bool
    {
        auto mysqlHandle = getMysqlHandle();
        if (mysqlHandle == nullptr)
            return false;

        const auto ping = mysql_ping(mysqlHandle);
        const auto errNo = mysql_errno(mysqlHandle);

        /* So strange logic, because I want interpret CR_COMMANDS_OUT_OF_SYNC errno as
           successful ping. */
        if (ping != 0 && errNo == CR_COMMANDS_OUT_OF_SYNC) {
            // TODO log to file, how often this happen silverqx
            qWarning("mysql_ping() returned : CR_COMMANDS_OUT_OF_SYNC(%ud)", errNo);
            return true;
        }
        else if (ping == 0)
            return true;
        else if (ping != 0)
            return false;
        else {
            qWarning() << "Unknown behavior during mysql_ping(), this should never happen :/";
            return false;
        }
    };

    if (qtConnection.isOpen() && mysqlPing()) {
        logConnected();
        return true;
    }

    // The database connection was lost
    logDisconnected();

    // Database connection have to be closed manually
    // isOpen() check is called in MySQL driver
    qtConnection.close();

    // Reset in transaction state and the savepoints counter
    resetTransactions();

    return false;
}

void DatabaseConnection::reconnect() const
{
    if (!m_reconnector)
        throw std::runtime_error("Lost connection and no reconnector available.");

    std::invoke(m_reconnector, *this);
}

void DatabaseConnection::disconnect()
{
    m_qtConnection.reset();
    m_qtConnectionResolver = nullptr;

    /* Closes the database connection, freeing any resources acquired,
       and invalidating any existing QSqlQuery objects that are used
       with the database.
       Only close the QSqlDatabase database connection and don't remove it
       from QSqlDatabase connection repository, so they can be reused, it's
       better for performance. */
    getRawQtConnection().close();
}

DatabaseConnection &
DatabaseConnection::setReconnector(const ReconnectorType &reconnector)
{
    m_reconnector = reconnector;

    return *this;
}

QVariant DatabaseConnection::getConfig(const QString &option) const
{
    return m_config.value(option);
}

QVariant DatabaseConnection::getConfig() const
{
    return m_config;
}

void DatabaseConnection::reconnectIfMissingConnection() const
{
    if (!m_qtConnectionResolver) {
        // This should never happen, but when it does, I want to know about that
        Q_ASSERT(m_qtConnection);

        reconnect();
    }
}

DatabaseConnection &DatabaseConnection::resetTransactions()
{
    m_savepoints = 0;
    m_inTransaction = false;

    return *this;
}

QSqlQuery DatabaseConnection::prepareQuery(const QString &queryString)
{
    // Prepare query string
    auto query = getQtQuery();

    // TODO solve setForwardOnly() in DatabaseConnection class, again this problem 🤔 silverqx
//    query.setForwardOnly(m_forwardOnly);

    query.prepare(queryString);

    return query;
}

void DatabaseConnection::logDisconnected()
{
    if (m_disconnectedLogged)
        return;
    m_disconnectedLogged = true;

    // Reset connected flag
    m_connectedLogged = false;

    qWarning() << "No active database connection, torrent additions / removes will "
                  "not be commited";
}

void DatabaseConnection::logConnected()
{
    if (m_connectedLogged)
        return;
    m_connectedLogged = true;

    // Reset disconnected flag
    m_disconnectedLogged = false;

    qInfo() << "Database connected";
}

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
