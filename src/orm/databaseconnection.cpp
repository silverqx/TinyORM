#include "orm/databaseconnection.hpp"

#include <QDateTime>
#if defined(TINYORM_MYSQL_PING)
#include <QDebug>
#endif

#include "orm/query/querybuilder.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

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

/* public */

// TODO err, may be configurable exceptions by config or compiler directive? (completely disable exceptions by directive) silverqx
DatabaseConnection::DatabaseConnection(
        std::function<Connectors::ConnectionName()> &&connection,
        // NOLINTNEXTLINE(modernize-pass-by-value)
        const QString &database, const QString &tablePrefix, const QVariantHash &config
)
    : m_qtConnectionResolver(std::move(connection))
    , m_database(database)
    , m_tablePrefix(tablePrefix)
    , m_config(config)
    , m_connectionName(getConfig(NAME).value<QString>())
    , m_hostName(getConfig(host_).value<QString>())
{}

QSharedPointer<QueryBuilder>
DatabaseConnection::table(const QString &table, const QString &as)
{
    auto builder = QSharedPointer<QueryBuilder>::create(*this, *m_queryGrammar);

    builder->from(table, as);

    return builder;
}

DatabaseConnection &DatabaseConnection::setTablePrefix(const QString &prefix)
{
    m_tablePrefix = prefix;

    getQueryGrammar().setTablePrefix(prefix);

    return *this;
}

BaseGrammar &DatabaseConnection::withTablePrefix(BaseGrammar &grammar) const
{
    grammar.setTablePrefix(m_tablePrefix);

    return grammar;
}

QSharedPointer<QueryBuilder> DatabaseConnection::query()
{
    return QSharedPointer<QueryBuilder>::create(*this, *m_queryGrammar);
}

/* Running SQL Queries */

QSqlQuery
DatabaseConnection::select(const QString &queryString,
                           const QVector<QVariant> &bindings)
{
    const auto functionName = __tiny_func__;

    return run<QSqlQuery>(queryString, bindings,
               [this, &functionName]
               (const QString &queryString_, const QVector<QVariant> &bindings_)
               -> QSqlQuery
    {
        if (m_pretending)
            return getQtQuery();

        // Prepare QSqlQuery
        auto query = prepareQuery(queryString_);

        bindValues(query, prepareBindings(bindings_));

        if (query.exec()) {
            // Query statements counter
            if (m_countingStatements)
                ++m_statementsCounter.normal;

            return query;
        }

        /* If an error occurs when attempting to run a query, we'll transform it
           to the exception QueryError(), which formats the error message to
           include the bindings with SQL, which will make this exception a lot
           more helpful to the developer instead of just the database's errors. */
        throw Exceptions::QueryError(
                    QStringLiteral("Select statement in %1() failed.")
                        .arg(functionName),
                    query, bindings_);
    });
}

QSqlQuery
DatabaseConnection::selectOne(const QString &queryString,
                              const QVector<QVariant> &bindings)
{
    auto query = select(queryString, bindings);

    query.first();

    return query;
}

QSqlQuery DatabaseConnection::statement(const QString &queryString,
                                        const QVector<QVariant> &bindings)
{
    const auto functionName = __tiny_func__;

    return run<QSqlQuery>(queryString, bindings,
               [this, &functionName]
               (const QString &queryString_, const QVector<QVariant> &bindings_)
               -> QSqlQuery
    {
        if (m_pretending)
            return getQtQuery();

        // Prepare QSqlQuery
        auto query = prepareQuery(queryString_);

        bindValues(query, prepareBindings(bindings_));

        if (query.exec()) {
            // Query statements counter
            if (m_countingStatements)
                ++m_statementsCounter.normal;

            recordsHaveBeenModified();

            return query;
        }

        /* If an error occurs when attempting to run a query, we'll transform it
           to the exception QueryError(), which formats the error message to
           include the bindings with SQL, which will make this exception a lot
           more helpful to the developer instead of just the database's errors. */
        throw Exceptions::QueryError(
                    // TODO next use __tiny_func__ in similar statements/exceptions silverqx
                    QStringLiteral("Statement in %1() failed.").arg(functionName),
                    query, bindings_);
    });
}

std::tuple<int, QSqlQuery>
DatabaseConnection::affectingStatement(const QString &queryString,
                                       const QVector<QVariant> &bindings)
{
    const auto functionName = __tiny_func__;

    return run<std::tuple<int, QSqlQuery>>(queryString, bindings,
            [this, &functionName]
            (const QString &queryString_, const QVector<QVariant> &bindings_)
            -> std::tuple<int, QSqlQuery>
    {
        if (m_pretending)
            return {0, getQtQuery()};

        // Prepare QSqlQuery
        auto query = prepareQuery(queryString_);

        bindValues(query, prepareBindings(bindings_));

        if (query.exec()) {
            // Affecting statements counter
            if (m_countingStatements)
                ++m_statementsCounter.affecting;

            auto numRowsAffected = query.numRowsAffected();

            recordsHaveBeenModified(numRowsAffected > 0);

            return {numRowsAffected, query};
        }

        /* If an error occurs when attempting to run a query, we'll transform it
           to the exception QueryError(), which formats the error message to
           include the bindings with SQL, which will make this exception a lot
           more helpful to the developer instead of just the database's errors. */
        throw Exceptions::QueryError(
                    QStringLiteral("Affecting statement in %1() failed.")
                        .arg(functionName),
                    query, bindings_);
    });
}

QSqlQuery DatabaseConnection::unprepared(const QString &queryString)
{
    const auto functionName = __tiny_func__;

    return run<QSqlQuery>(queryString, {},
               [this, &functionName]
               (const QString &queryString_, const QVector<QVariant> &/*unused*/)
               -> QSqlQuery
    {
        if (m_pretending)
            return getQtQuery();

        // Prepare unprepared QSqlQuery 🙂
        auto query = getQtQuery();

        if (query.exec(queryString_)) {
            // Query statements counter
            if (m_countingStatements)
                ++m_statementsCounter.normal;

            recordsHaveBeenModified();

            return query;
        }

        /* If an error occurs when attempting to run a query, we'll transform it
           to the exception QueryError(), which formats the error message to
           include the bindings with SQL, which will make this exception a lot
           more helpful to the developer instead of just the database's errors. */
        throw Exceptions::QueryError(
                    QStringLiteral("Statement in %1() failed.").arg(functionName),
                    query);
    });
}

/* Obtain connection instance */

QSqlDatabase DatabaseConnection::getQtConnection()
{
    if (!m_qtConnection) {
        // This should never happen 🤔
        Q_ASSERT(m_qtConnectionResolver);

        // Reconnect if missing
        m_qtConnection = std::invoke(m_qtConnectionResolver);

        /* This should never happen 🤔, do this check only when the QSqlDatabase
           connection was resolved by connection resolver. */
        if (!QSqlDatabase::contains(*m_qtConnection))
            throw Exceptions::RuntimeError(
                    QStringLiteral("Connection '%1' doesn't exist.")
                    .arg(*m_qtConnection));
    }

    // Return the connection from QSqlDatabase connection manager
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
DatabaseConnection::prepareBindings(QVector<QVariant> bindings) const
{
    for (auto &binding : bindings) {
        // Nothing to convert
        if (!binding.isValid() || binding.isNull())
            continue;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        switch (binding.typeId()) {
#else
        switch (binding.userType()) {
#endif
        /* We need to transform all instances of DateTimeInterface into the actual
           date string. Each query grammar maintains its own date string format
           so we'll just ask the grammar for the format to get from the date. */
        case QMetaType::QDate:
        case QMetaType::QDateTime:
            binding = binding.value<QDateTime>()
                      .toString(m_queryGrammar->getDateFormat());
            break;

        /* I have decided to not handle the QMetaType::Bool here, little info:
           - Qt's MySql driver handles bool values internally, it doesn't matter if you
             pass true/false or 0/1
           - I have not investigated how Qt's Postgres driver works internally, but
             Postgres is very sensitive about bool columns and bool values, so if you
             have bool column then you have to pass bool type to the driver
           - I don't remember about Qt's Sqlite driver exactly, but I'm pretty sure that
             it doesn't handle bool values as Qt's mysql driver does, because I had some
             problems with bool values when I have added Sqlite support */

        default:
            break;
        }
    }

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

bool DatabaseConnection::pingDatabase()
{
    throw Exceptions::RuntimeError(
                QStringLiteral("The '%1' database driver doesn't support ping command.")
                .arg(driverName()));
}

void DatabaseConnection::reconnect() const
{
    if (!m_reconnector)
        throw std::runtime_error("Lost connection and no reconnector available.");

    std::invoke(m_reconnector, *this);
}

void DatabaseConnection::disconnect()
{
    /* Closes the database connection, freeing any resources acquired,
       and invalidating any existing QSqlQuery objects that are used
       with the database.
       Only close the QSqlDatabase database connection and don't remove it
       from QSqlDatabase connection repository, so they can be reused, it's
       better for performance. */
    getRawQtConnection().close();

    m_qtConnection.reset();
    m_qtConnectionResolver = nullptr;
}

std::unique_ptr<SchemaBuilder> DatabaseConnection::getSchemaBuilder()
{
    if (!m_schemaGrammar)
        useDefaultSchemaGrammar();

    return std::make_unique<SchemaBuilder>(*this);
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

/* Getters */

QString DatabaseConnection::driverName()
{
    return getQtConnection().driverName();
}

namespace
{
    using DriverNameMapType = std::unordered_map<QString, const QString &>;

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
    Q_GLOBAL_STATIC_WITH_ARGS(DriverNameMapType, DRIVER_NAME_MAP, ({
                                  {QMYSQL,  MYSQL_},
                                  {QPSQL,   POSTGRESQL},
                                  {QSQLITE, SQLITE}
                              }));
} // namespace

const QString &DatabaseConnection::driverNamePrintable()
{
    if (m_driverNamePrintable)
        return *m_driverNamePrintable;

    // Cache
    m_driverNamePrintable = DRIVER_NAME_MAP->at(driverName());

    return *m_driverNamePrintable;
}

QVector<Log>
DatabaseConnection::pretend(const std::function<void()> &callback)
{
    return withFreshQueryLog([this, &callback]
    {
        m_pretending = true;

        /* Basically to make the database connection "pretend", we will just return
           the default values for all the query methods, then we will return an
           array of queries that were "executed" within the Closure callback. */
        std::invoke(callback);

        m_pretending = false;

        return *m_queryLog;
    });
}

QVector<Log>
DatabaseConnection::pretend(const std::function<void(DatabaseConnection &)> &callback)
{
    return withFreshQueryLog([this, &callback]
    {
        m_pretending = true;

        /* Basically to make the database connection "pretend", we will just return
           the default values for all the query methods, then we will return an
           array of queries that were "executed" within the Closure callback. */
        std::invoke(callback, *this);

        m_pretending = false;

        return *m_queryLog;
    });
}

/* protected */

void DatabaseConnection::useDefaultQueryGrammar()
{
    m_queryGrammar = getDefaultQueryGrammar();
}

void DatabaseConnection::useDefaultSchemaGrammar()
{
    m_schemaGrammar = getDefaultSchemaGrammar();
}

void DatabaseConnection::useDefaultPostProcessor()
{
    m_postProcessor = getDefaultPostProcessor();
}

std::unique_ptr<QueryProcessor> DatabaseConnection::getDefaultPostProcessor() const
{
    return std::make_unique<QueryProcessor>();
}

void DatabaseConnection::reconnectIfMissingConnection() const
{
    if (!m_qtConnectionResolver) {
        // This should never happen, but when it does, I want to know about that
        Q_ASSERT(m_qtConnection);

        reconnect();
    }
}

/* private */

QSqlQuery DatabaseConnection::prepareQuery(const QString &queryString)
{
    // Prepare query string
    auto query = getQtQuery();

    // TODO solve setForwardOnly() in DatabaseConnection class, again this problem 🤔 silverqx
//    query.setForwardOnly(m_forwardOnly);

    query.prepare(queryString);

    return query;
}

void DatabaseConnection::logConnected()
{
#ifdef TINYORM_MYSQL_PING
    if (m_connectedLogged)
        return;

    m_connectedLogged = true;

    // Reset disconnected flag
    m_disconnectedLogged = false;

    /* I still don't know if it's a good idea to log this to the console by default,
       it's a very important log message though. */
    qInfo().noquote()
            << QStringLiteral("%1 database connected (%2, %3@%4)")
               .arg(driverNamePrintable(), m_connectionName, m_hostName, m_database);
#endif
}

void DatabaseConnection::logDisconnected()
{
#ifdef TINYORM_MYSQL_PING
    if (m_disconnectedLogged)
        return;

    m_disconnectedLogged = true;

    // Reset connected flag
    m_connectedLogged = false;

    qWarning().noquote()
            << QStringLiteral("%1 database disconnected (%2, %3@%4)")
               .arg(driverNamePrintable(), m_connectionName, m_hostName, m_database);
#endif
}

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE
