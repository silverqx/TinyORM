#include "orm/databaseconnection.hpp"

#include <QElapsedTimer>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>

#include "mysql.h"

#include "orm/logquery.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

const char *DatabaseConnection::SAVEPOINT_NAMESPACE = const_cast<char *>("tinyorm_savepoint");

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
    transcation(). The same is true for rollback(), so I will not call
    rollback() for both, to end transaction and to end savepoint, instead,
    I will call rollback() for transaction and rollbackToSavepoint("xx_1")
    for savepoint. This makes it clear at a glance what is happening.
*/

namespace
{
    // TODO duplicate, merge with logExecutedQuery() silverqx
    const auto logQuery = [](const QSqlQuery &query,
                          const std::optional<quint64> elapsed = std::nullopt)
    {
        qDebug().nospace().noquote()
            << "Executed prepared query (" << (elapsed ? *elapsed : -1) << "ms, "
            << query.size() << " results, " << query.numRowsAffected()
            << " affected) : " << parseExecutedQuery(query);
    };
}

// TODO next rewrite DatabaseConnection to match Laravel's implementation 🤓😕 silverqx
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

bool DatabaseConnection::pingDatabase()
{
    auto db = getQtConnection();

    const auto getMysqlHandle = [&db]() -> MYSQL *
    {
        auto driverHandle = db.driver()->handle();
        if (qstrcmp(driverHandle.typeName(), "MYSQL*") == 0)
            return *static_cast<MYSQL **>(driverHandle.data());
        return nullptr;
    };
    const auto mysqlPing = [getMysqlHandle]()
    {
        auto mysqlHandle = getMysqlHandle();
        if (mysqlHandle == nullptr)
            return false;
        auto ping = mysql_ping(mysqlHandle);
        auto errNo = mysql_errno(mysqlHandle);
        /* So strange logic, because I want interpret CR_COMMANDS_OUT_OF_SYNC errno as
           successful ping. */
        if ((ping != 0) && (errNo == CR_COMMANDS_OUT_OF_SYNC)) {
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

    if (db.isOpen() && mysqlPing()) {
        showDbConnected();
        return true;
    }

    // The connection was lost here
    showDbDisconnected();
    // Database connection have to be closed manually
    // isOpen() check is called in MySQL driver
    db.close();
    // Reset in transaction state
    m_inTransaction = false;
    // Reset the savepoints counter
    m_savepoints = 0;
    return false;
}

bool DatabaseConnection::transaction()
{
    Q_ASSERT(m_inTransaction == false);
    const auto ok = getQtConnection().transaction();
    if (!ok)
        return false;
    m_inTransaction = true;
    return true;
}

bool DatabaseConnection::commit()
{
    Q_ASSERT(m_inTransaction);
    const auto ok = getQtConnection().commit();
    if (!ok)
        return false;

    m_inTransaction = false;
    return true;
}

bool DatabaseConnection::rollback()
{
    Q_ASSERT(m_inTransaction);
    const auto ok = getQtConnection().rollback();
    if (!ok)
        return false;

    m_inTransaction = false;
    return true;
}

bool DatabaseConnection::savepoint(const QString &id)
{
    // TODO rewrite savepoint() and rollback() with a new m_db.statement() API silverqx
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
    auto query = prepareQuery(queryString, bindings);

#ifdef TINYORM_DEBUG_SQL
    QElapsedTimer timer;
    timer.start();
#endif

    const auto ok = query.exec();
    if (!ok) {
        qDebug() << "Statement in DatabaseConnection::statement() failed :"
                 << query.lastError().text();
#ifdef TINYORM_DEBUG_SQL
        logQuery(query, timer.elapsed());
#else
        logQuery(query);
#endif
    }
#ifdef TINYORM_DEBUG_SQL
    else
        logQuery(query, timer.elapsed());
#endif

    return {ok, query};
}

std::tuple<int, QSqlQuery>
DatabaseConnection::affectingStatement(const QString &queryString,
                                       const QVector<QVariant> &bindings)
{
    auto query = prepareQuery(queryString, bindings);

#ifdef TINYORM_DEBUG_SQL
    QElapsedTimer timer;
    timer.start();
#endif

    // TODO err, throw exceptions when failed, may be configurable by config or compiler directive silverqx
    const auto ok = query.exec();
    if (!ok) {
        qDebug() << "Affecting statement in DatabaseConnection::affectingStatement() failed :"
                 << query.lastError().text();
#ifdef TINYORM_DEBUG_SQL
        logQuery(query, timer.elapsed());
#else
        logQuery(query);
#endif
    }
#ifdef TINYORM_DEBUG_SQL
    else
        logQuery(query, timer.elapsed());
#endif

    return {query.numRowsAffected(), query};
}

QSqlDatabase DatabaseConnection::getQtConnection()
{
    if (!m_qtConnection) {
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

QSqlQuery DatabaseConnection::getQtQuery()
{
    return QSqlQuery(getQtConnection());
}

QVariant DatabaseConnection::getConfig(const QString &option) const
{
    return m_config.value(option);
}

QVariant DatabaseConnection::getConfig() const
{
    return m_config;
}

void DatabaseConnection::showDbDisconnected()
{
    if (m_dbDisconnectedShowed)
        return;
    m_dbDisconnectedShowed = true;

    // Reset connected flag
    m_dbConnectedShowed = false;

    qWarning() << "No active database connection, torrent additions / removes will "
                  "not be commited";
}

void DatabaseConnection::showDbConnected()
{
    if (m_dbConnectedShowed)
        return;
    m_dbConnectedShowed = true;

    // Reset disconnected flag
    m_dbDisconnectedShowed = false;

    qInfo() << "Database connected";
}

QSqlQuery DatabaseConnection::prepareQuery(const QString &queryString,
                                           const QVector<QVariant> &bindings)
{
    // Prepare query string
    auto query = getQtQuery();
    // TODO solve setForwardOnly() in DatabaseConnection class, again this problem 🤔 silverqx
//    query.setForwardOnly(m_forwardOnly);
    query.prepare(queryString);

    // Prepare query bindings
    auto itBinding = bindings.constBegin();
    while (itBinding != bindings.constEnd()) {
        query.addBindValue(*itBinding);
        ++itBinding;
    }

    return query;
}

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
