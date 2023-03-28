#include "orm/databaseconnection.hpp"

#if defined(TINYORM_MYSQL_PING)
#  include <QDebug>
#endif

#include <QtSql/QSqlRecord>

#include "orm/exceptions/lostconnectionerror.hpp"
#include "orm/exceptions/multiplecolumnsselectederror.hpp"
#include "orm/query/querybuilder.hpp"
#include "orm/utils/configuration.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Utils::Helpers;

using ConfigUtils = Orm::Utils::Configuration;

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

DatabaseConnection::DatabaseConnection(
        std::function<Connectors::ConnectionName()> &&connection,
        QString &&database, QString &&tablePrefix, QtTimeZoneConfig &&qtTimeZone,
        QVariantHash &&config
)
    : m_qtConnectionResolver(std::move(connection))
    , m_database(std::move(database))
    , m_tablePrefix(std::move(tablePrefix))
    , m_qtTimeZone(std::move(qtTimeZone))
    , m_isConvertingTimeZone(m_qtTimeZone.type != QtTimeZoneType::DontConvert)
    , m_config(std::move(config))
    , m_connectionName(getConfig(NAME).value<QString>())
    , m_hostName(getConfig(host_).value<QString>())
{}

DatabaseConnection::DatabaseConnection(
        std::function<Connectors::ConnectionName()> &&connection,
        QString &&database, QString &&tablePrefix, QtTimeZoneConfig &&qtTimeZone,
        std::optional<bool> &&returnQDateTime, QVariantHash &&config
)
    : m_qtConnectionResolver(std::move(connection))
    , m_database(std::move(database))
    , m_tablePrefix(std::move(tablePrefix))
    , m_qtTimeZone(std::move(qtTimeZone))
    , m_isConvertingTimeZone(m_qtTimeZone.type != QtTimeZoneType::DontConvert)
    , m_returnQDateTime(std::move(returnQDateTime))
    , m_config(std::move(config))
    , m_connectionName(getConfig(NAME).value<QString>())
    , m_hostName(getConfig(host_).value<QString>())
{}

std::shared_ptr<QueryBuilder>
DatabaseConnection::table(const QString &table, const QString &as)
{
    auto builder = query();

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

std::shared_ptr<QueryBuilder> DatabaseConnection::query()
{
    /* Not needed because this class is abstract, but can happen if I forget to call
       the useDefaultQueryGrammar() in descendants, so I leave this check here. */
    Q_ASSERT(m_queryGrammar);

    return std::make_shared<QueryBuilder>(*this, m_queryGrammar);
}

/* Running SQL Queries */

SqlQuery
DatabaseConnection::select(const QString &queryString,
                           const QVector<QVariant> &bindings)
{
    auto queryResult = run<QSqlQuery>(
                           queryString, bindings, Prepared,
                           [this](const QString &queryString_,
                                  const QVector<QVariant> &preparedBindings)
                           -> QSqlQuery
    {
        if (m_pretending)
            return getQtQueryForPretend();

        // Prepare QSqlQuery
        auto query = prepareQuery(queryString_);

        bindValues(query, preparedBindings);

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
                    m_connectionName,
                    "Select statement in DatabaseConnection::select() failed.",
                    query, preparedBindings);
    });

    return {std::move(queryResult), m_qtTimeZone, *m_queryGrammar, m_returnQDateTime};
}

SqlQuery
DatabaseConnection::selectOne(const QString &queryString,
                              const QVector<QVariant> &bindings)
{
    auto query = select(queryString, bindings);

    query.first();

    return query;
}

QVariant
DatabaseConnection::scalar(const QString &queryString, const QVector<QVariant> &bindings)
{
    const auto query = selectOne(queryString, bindings);

    // Nothing to do, the query should be positioned on the first row/record
    if (!query.isValid())
        return {};

    if (const auto count = query.record().count();
        count > 1
    )
        throw Exceptions::MultipleColumnsSelectedError(count, __tiny_func__);

    return query.value(0);
}

SqlQuery DatabaseConnection::statement(const QString &queryString,
                                       const QVector<QVariant> &bindings)
{
    auto queryResult = run<QSqlQuery>(
                           queryString, bindings, Prepared,
                           [this](const QString &queryString_,
                                  const QVector<QVariant> &preparedBindings)
                           -> QSqlQuery
    {
        if (m_pretending)
            return getQtQueryForPretend();

        // Prepare QSqlQuery
        auto query = prepareQuery(queryString_);

        bindValues(query, preparedBindings);

        if (query.exec()) {
            // Query statements counter
            if (m_countingStatements)
                ++m_statementsCounter.normal;

            recordsHaveBeenModified();

            return query;
        }

        // TODO perf, use __tiny_func__ but when I fix pref. problem with it, rewrite it w/o the QRegularExpression, look at and revert the 8e114524 and 03fc82ae commits, also use static local variable instead! ALSO create macro eg. T_FUNCTION_NAME - static const auto functionName = __tiny_func__; silverqx
        /* If an error occurs when attempting to run a query, we'll transform it
           to the exception QueryError(), which formats the error message to
           include the bindings with SQL, which will make this exception a lot
           more helpful to the developer instead of just the database's errors. */
        throw Exceptions::QueryError(
                    m_connectionName,
                    "Statement in DatabaseConnection::statement() failed.",
                    query, preparedBindings);
    });

    return {std::move(queryResult), m_qtTimeZone, *m_queryGrammar, m_returnQDateTime};
}

std::tuple<int, QSqlQuery>
DatabaseConnection::affectingStatement(const QString &queryString,
                                       const QVector<QVariant> &bindings)
{
    return run<std::tuple<int, QSqlQuery>>(
               queryString, bindings, Prepared,
               [this](const QString &queryString_,
                      const QVector<QVariant> &preparedBindings)
               -> std::tuple<int, QSqlQuery>
    {
        if (m_pretending)
            return {-1, getQtQueryForPretend()};

        // Prepare QSqlQuery
        auto query = prepareQuery(queryString_);

        bindValues(query, preparedBindings);

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
                    m_connectionName,
                    "Affecting statement in DatabaseConnection::affectingStatement() "
                    "failed.",
                    query, preparedBindings);
    });
}

SqlQuery DatabaseConnection::unprepared(const QString &queryString)
{
    auto queryResult = run<QSqlQuery>(
                           queryString, {}, Unprepared,
                           [this](const QString &queryString_,
                                  const QVector<QVariant> &/*unused*/)
                           -> QSqlQuery
    {
        if (m_pretending)
            return getQtQueryForPretend();

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
                    m_connectionName,
                    "Unprepared statement in DatabaseConnection::unprepared() failed.",
                    query);
    });

    return {std::move(queryResult), m_qtTimeZone, *m_queryGrammar, m_returnQDateTime};
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
                    QStringLiteral("QSqlDatabase does not contain '%1' connection.")
                    .arg(*m_qtConnection));
    }

    // Return the connection from QSqlDatabase connection manager
    return QSqlDatabase::database(*m_qtConnection, true);
}

QSqlDatabase DatabaseConnection::getRawQtConnection() const
{
    if (!m_qtConnection)
        throw Exceptions::RuntimeError(
                "Can not obtain a connection from the QSqlDatabase instance because "
                "the connection has not yet been established.");

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
       because it has two functions, saves active connection name and
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

        switch (Helpers::qVariantTypeId(binding)) {
        // QDate doesn't have a time zone
        case QMetaType::QDate:
            binding = binding.value<QDate>().toString(Qt::ISODate);
            break;

        /* We need to transform all instances of QDateTime into the actual date string.
           Each query grammar maintains its own date string format so we'll just ask
           the grammar for the format to get from the date. */
        case QMetaType::QDateTime:
            binding = prepareBinding(binding.value<QDateTime>())
                      .toString(m_queryGrammar->getDateFormat());
            break;

        /* I have decided to not handle the QMetaType::Bool here, little info:
           - Qt's QMYSQL driver handles bool values internally, it doesn't matter if you
             pass true/false or 0/1
           - Qt's QPSQL driver calls QVariant(bool).toBool() ? QStringLiteral("TRUE")
                                                             : QStringLiteral("FALSE")
           - Qt's QSQLITE driver calls toInt() on the QVariant(bool):
             sqlite3_bind_int(d->stmt, i + 1, value.toInt()); */

        default:
            break; // Don't use the Q_UNREACHABLE()
        }
    }

    return bindings;
}

void DatabaseConnection::bindValues(QSqlQuery &query, const QVector<QVariant> &bindings)
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

QSqlDriver *DatabaseConnection::driver()
{
    return getQtConnection().driver();
}

void DatabaseConnection::reconnectIfMissingConnection() const
{
    /* Calls a connection resolver defined
       in the ConnectionFactory::createQSqlDatabaseResolver(), the connection resolver
       is passed to the DatabaseConnection constructor and is always available. Only
       one exception is when disconnect() is called, it resets connection resolver which
       will be recreated (with the db connection of course) here, that is only one case
       when the code below (reconnect() logic) is true as I'm aware of. */
    if (m_qtConnectionResolver)
        return;

    /* This should never happen, but when it does, I want to know about that. If the
       m_qtConnectionResolver is not set then m_qtConnection has to be std::nullopt. */
    Q_ASSERT(!m_qtConnection);

    reconnect();
}

void DatabaseConnection::reconnect() const
{
    if (!m_reconnector)
        throw Exceptions::LostConnectionError(
                QStringLiteral("Lost connection and no reconnector available in %1().")
                .arg(__tiny_func__));

    std::invoke(m_reconnector, *this);
}

void DatabaseConnection::disconnect()
{
    // Nothing to disconnect
    if (!m_qtConnection)
        return;

    /* Closes the database connection, freeing any resources acquired,
       and invalidating any existing QSqlQuery objects that are used
       with the database.
       Only close the QSqlDatabase database connection and don't remove it
       from QSqlDatabase connection repository, so it can be reused, it's
       better for performance.
       Revisited, it's ok and will not cause any leaks or dangling connection. */
    getRawQtConnection().close();

    m_qtConnection.reset();
    m_qtConnectionResolver = nullptr;
}

SchemaBuilder &DatabaseConnection::getSchemaBuilder()
{
    if (!m_schemaGrammar)
        useDefaultSchemaGrammar();

    if (!m_schemaBuilder)
        useDefaultSchemaBuilder();

    return *m_schemaBuilder;
}

std::shared_ptr<SchemaGrammar> DatabaseConnection::getSchemaGrammarShared()
{
    if (!m_schemaGrammar)
        useDefaultSchemaGrammar();

    return m_schemaGrammar;
}

DatabaseConnection &
DatabaseConnection::setReconnector(const ReconnectorType &reconnector)
{
    m_reconnector = reconnector;

    return *this;
}

/* Connection configuration */

QVariant DatabaseConnection::getConfig(const QString &option) const
{
    return m_config.value(option);
}

bool DatabaseConnection::hasConfig(const QString &option) const
{
    return m_config.contains(option);
}

/* Getters */

QString DatabaseConnection::driverName()
{
    return getQtConnection().driverName();
}

/*! Printable driver name hash type. */
using DriverNameMapType = std::unordered_map<QString, const QString &>;

/*! Map a Qt's database driver name to the pretty name. */
Q_GLOBAL_STATIC_WITH_ARGS(DriverNameMapType, DRIVER_NAME_MAP, ({ // NOLINT(misc-use-anonymous-namespace)
                              {QMYSQL,  MYSQL_},
                              {QPSQL,   POSTGRESQL},
                              {QSQLITE, SQLITE}
                          }))

const QString &DatabaseConnection::driverNamePrintable()
{
    if (m_driverNamePrintable)
        return *m_driverNamePrintable;

    // Cache
    m_driverNamePrintable = DRIVER_NAME_MAP->at(driverName());

    return *m_driverNamePrintable;
}

DatabaseConnection &
DatabaseConnection::setQtTimeZone(const QVariant &qtTimeZone)
{
    m_qtTimeZone = ConfigUtils::prepareQtTimeZone(qtTimeZone, m_connectionName);

    m_isConvertingTimeZone = m_qtTimeZone.type != QtTimeZoneType::DontConvert;

    return *this;
}

DatabaseConnection &
DatabaseConnection::setQtTimeZone(QtTimeZoneConfig &&qtTimeZone) noexcept
{
    m_qtTimeZone = std::move(qtTimeZone);

    m_isConvertingTimeZone = m_qtTimeZone.type != QtTimeZoneType::DontConvert;

    return *this;
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

void Orm::DatabaseConnection::useDefaultSchemaBuilder()
{
    m_schemaBuilder = getDefaultSchemaBuilder();
}

void DatabaseConnection::useDefaultPostProcessor()
{
    m_postProcessor = getDefaultPostProcessor();
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

QDateTime DatabaseConnection::prepareBinding(const QDateTime &binding) const
{
    /* Nothing to convert, the qt_timezone config. option is not valid or was not defined
       or the QDateTime binding is null. */
    if (!m_isConvertingTimeZone || binding.isNull())
        return binding;

    /* Convert to the time zone provided through the qt_timezone connection config. option
       for QDateTime binding. It fixes buggy behavior of all QtSql database drivers. */
    return Helpers::convertTimeZone(binding, m_qtTimeZone);
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
