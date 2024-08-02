#include "orm/databaseconnection.hpp"

#ifdef TINYORM_MYSQL_PING
#  include <QDebug>
#endif

#include TINY_INCLUDE_TSqlRecord

#include "orm/exceptions/lostconnectionerror.hpp"
#include "orm/exceptions/multiplecolumnsselectederror.hpp"
#include "orm/query/querybuilder.hpp"
#include "orm/utils/configuration.hpp"
#include "orm/utils/helpers.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Utils::Helpers;

using ConfigUtils = Orm::Utils::Configuration;

namespace Orm
{

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
        const std::optional<bool> returnQDateTime, QVariantHash &&config
)
    : m_qtConnectionResolver(std::move(connection))
    , m_database(std::move(database))
    , m_tablePrefix(std::move(tablePrefix))
    , m_qtTimeZone(std::move(qtTimeZone))
    , m_isConvertingTimeZone(m_qtTimeZone.type != QtTimeZoneType::DontConvert)
    , m_returnQDateTime(returnQDateTime)
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

    return std::make_shared<QueryBuilder>(shared_from_this(), m_queryGrammar);
}

/* Running SQL Queries */

SqlQuery
DatabaseConnection::select(const QString &queryString, QList<QVariant> bindings)
{
    auto queryResult = run<TSqlQuery>(
                           queryString, std::move(bindings), Prepared,
                           [this](const QString &queryString_,
                                  const QList<QVariant> &preparedBindings)
                           -> TSqlQuery
    {
        if (m_pretending)
            return getSqlQueryForPretend();

        // Prepare QSqlQuery
        auto query = prepareQuery(queryString_);

        bindValues(query, preparedBindings);

        if (query.exec()) {
            // Query statements counter
            if (m_countingStatements)
                ++m_statementsCounter.normal;

            return query;
        }

#ifdef TINYORM_USING_QTSQLDRIVERS
        /* If an error occurs when attempting to run a query, we'll transform it
           to the exception QueryError(), which formats the error message to
           include the bindings with SQL, which will make this exception a lot
           more helpful to the developer instead of just the database's errors. */
        throw Exceptions::QueryError(
                    m_connectionName,
                    "Select statement in DatabaseConnection::select() failed.",
                    query, preparedBindings);
#else
        Q_UNREACHABLE();
#endif
    });

    return {std::move(queryResult), m_qtTimeZone, *m_queryGrammar, m_returnQDateTime};
}

SqlQuery
DatabaseConnection::selectOne(const QString &queryString, QList<QVariant> bindings)
{
    auto query = select(queryString, std::move(bindings));

    query.first();

    return query;
}

QVariant
DatabaseConnection::scalar(const QString &queryString, QList<QVariant> bindings)
{
    const auto query = selectOne(queryString, std::move(bindings));

    // Nothing to do, the query should be positioned on the first row/record
    if (!query.isValid())
        return {};

    if (const auto count = query.record().count();
        count > 1
    )
        throw Exceptions::MultipleColumnsSelectedError(count, __tiny_func__);

    return query.value(0);
}

SqlQuery
DatabaseConnection::statement(const QString &queryString, QList<QVariant> bindings)
{
    auto queryResult = run<TSqlQuery>(
                           queryString, std::move(bindings), Prepared,
                           [this](const QString &queryString_,
                                  const QList<QVariant> &preparedBindings)
                           -> TSqlQuery
    {
        if (m_pretending)
            return getSqlQueryForPretend();

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
#ifdef TINYORM_USING_QTSQLDRIVERS
        /* If an error occurs when attempting to run a query, we'll transform it
           to the exception QueryError(), which formats the error message to
           include the bindings with SQL, which will make this exception a lot
           more helpful to the developer instead of just the database's errors. */
        throw Exceptions::QueryError(
                    m_connectionName,
                    "Statement in DatabaseConnection::statement() failed.",
                    query, preparedBindings);
#else
        Q_UNREACHABLE();
#endif
    });

    return {std::move(queryResult), m_qtTimeZone, *m_queryGrammar, m_returnQDateTime};
}

/* No need to return Orm::SqlQuery because affectingStatement-s don't return any results
   from the database like eg. select queries so there is no need to correct QDateTime
   timezones. */
std::tuple<int, TSqlQuery>
DatabaseConnection::affectingStatement(const QString &queryString,
                                       QList<QVariant> bindings)
{
    return run<std::tuple<int, TSqlQuery>>(
               queryString, std::move(bindings), Prepared,
               [this](const QString &queryString_,
                      const QList<QVariant> &preparedBindings)
               -> std::tuple<int, TSqlQuery>
    {
        if (m_pretending)
            return {-1, getSqlQueryForPretend()};

        // Prepare QSqlQuery
        auto query = prepareQuery(queryString_);

        bindValues(query, preparedBindings);

        if (query.exec()) {
            // Affecting statements counter
            if (m_countingStatements)
                ++m_statementsCounter.affecting;

            auto numRowsAffected = query.numRowsAffected();

            recordsHaveBeenModified(numRowsAffected > 0);

            return {numRowsAffected, std::move(query)};
        }

#ifdef TINYORM_USING_QTSQLDRIVERS
        /* If an error occurs when attempting to run a query, we'll transform it
           to the exception QueryError(), which formats the error message to
           include the bindings with SQL, which will make this exception a lot
           more helpful to the developer instead of just the database's errors. */
        throw Exceptions::QueryError(
                    m_connectionName,
                    "Affecting statement in DatabaseConnection::affectingStatement() "
                    "failed.",
                    query, preparedBindings);
#else
        Q_UNREACHABLE();
#endif
    });
}

SqlQuery DatabaseConnection::unprepared(const QString &queryString)
{
    auto queryResult = run<TSqlQuery>(
                           queryString, {}, Unprepared,
                           [this](const QString &queryString_,
                                  const QList<QVariant> &/*unused*/)
                           -> TSqlQuery
    {
        if (m_pretending)
            return getSqlQueryForPretend();

        // Prepare unprepared QSqlQuery 🙂
        auto query = getSqlQuery();

        if (query.exec(queryString_)) {
            // Query statements counter
            if (m_countingStatements)
                ++m_statementsCounter.normal;

            recordsHaveBeenModified();

            return query;
        }

#ifdef TINYORM_USING_QTSQLDRIVERS
        /* If an error occurs when attempting to run a query, we'll transform it
           to the exception QueryError(), which formats the error message to
           include the bindings with SQL, which will make this exception a lot
           more helpful to the developer instead of just the database's errors. */
        throw Exceptions::QueryError(
                    m_connectionName,
                    "Unprepared statement in DatabaseConnection::unprepared() failed.",
                    query);
#else
        Q_UNREACHABLE();
#endif
    });

    return {std::move(queryResult), m_qtTimeZone, *m_queryGrammar, m_returnQDateTime};
}

/* Obtain connection instance */

TSqlDatabase DatabaseConnection::getSqlConnection()
{
    if (!m_qtConnection) {
        // This should never happen 🤔
        Q_ASSERT(m_qtConnectionResolver);

        // Reconnect if missing
        m_qtConnection = std::invoke(m_qtConnectionResolver);

        /* This should never happen 🤔, do this check only when the QSqlDatabase
           connection was resolved by connection resolver. */
        if (!TSqlDatabase::contains(*m_qtConnection))
            throw Exceptions::RuntimeError(
                    QStringLiteral("QSqlDatabase does not contain '%1' connection.")
                    .arg(*m_qtConnection));
    }

    // Return the connection from QSqlDatabase connection manager
    return TSqlDatabase::database(*m_qtConnection, true);
}

TSqlDatabase DatabaseConnection::getRawSqlConnection() const
{
    if (!m_qtConnection)
        throw Exceptions::RuntimeError(
                "Can not obtain a connection from the QSqlDatabase instance because "
                "the connection has not yet been established.");

    return TSqlDatabase::database(*m_qtConnection);
}

DatabaseConnection &
DatabaseConnection::setSqlConnectionResolver(
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

TSqlQuery DatabaseConnection::getSqlQuery()
{
    return TSqlQuery(getSqlConnection());
}

QList<QVariant> &
DatabaseConnection::prepareBindings(QList<QVariant> &bindings) const
{
    /* Weird return value (non-const reference) is for better variable naming in caller.
       Also, I tried to revert back movable bindings to const & bindings only because
       I thought there was no reason for movable bindings but I denied it.
       The reason was that each binding would have to be copied one by one at the end
       of the following for() loop and this is less efficient, so is better to move or
       copy bindings container once in select/statement/insert/affectingStatement methods
       and then modify them here. I leave it this way, it's not a big deal anyway. 😎 */

    for (auto &binding : bindings) {
        // Nothing to convert
        if (!binding.isValid() || binding.isNull())
            continue;

        switch (binding.typeId()) {
        case QMetaType::QDate:
            // QDate doesn't have a time zone
            binding = binding.value<QDate>().toString(Qt::ISODate);
            continue;

        /* We need to transform all instances of QDateTime into the actual date string.
           Each query grammar maintains its own date string format so we'll just ask
           the grammar for the format. */
        case QMetaType::QDateTime:
            // Convert to the time zone provided through the qt_timezone config. option
            binding = prepareBinding(binding.value<QDateTime>())
                      .toString(m_queryGrammar->getDateFormat());
            continue;

        /* We need to transform all instances of QTime into the actual time string.
           Each query grammar maintains its own time string format so we'll just ask
           the grammar for the format. */
        case QMetaType::QTime:
            // QTime doesn't have a time zone
            binding = binding.value<QTime>().toString(m_queryGrammar->getTimeFormat());
            continue;

        /* I have decided to not handle the QMetaType::Bool here, little info:
           - Qt's QMYSQL driver handles bool values internally, it doesn't matter if you
             pass true/false or 0/1
           - Qt's QPSQL driver calls QVariant(bool).toBool() ? QStringLiteral("TRUE")
                                                             : QStringLiteral("FALSE")
           - Qt's QSQLITE driver calls toInt() on the QVariant(bool):
             sqlite3_bind_int(d->stmt, i + 1, value.toInt()); */

        default: // Defined to avoid Clang Tidy warning
            continue; // Don't use the Q_UNREACHABLE()
        }
    }

    return bindings;
}

void DatabaseConnection::bindValues(TSqlQuery &query, const QList<QVariant> &bindings)
{
    for (const auto &binding : bindings)
        query.addBindValue(binding);
}

bool DatabaseConnection::pingDatabase()
{
    throw Exceptions::RuntimeError(
                QStringLiteral("The '%1' database driver doesn't support ping command.")
                .arg(driverName()));
}

/* See the note near the DatabaseManager/SqlQuery::driver() method about
   driver() vs driverWeak(). */

const TSqlDriver *DatabaseConnection::driver()
{
    return getSqlConnection().driver();
}

#ifdef TINYORM_USING_TINYDRIVERS
std::weak_ptr<const TSqlDriver> DatabaseConnection::driverWeak()
{
    return getSqlConnection().driverWeak();
}
#endif

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
    getRawSqlConnection().close();

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
    return getSqlConnection().driverName();
}

/*! Printable driver name hash type. */
using DriverNameMapType = std::unordered_map<QString, const QString &>;

/*! Map a Qt's database driver name to the pretty name. */
Q_GLOBAL_STATIC_WITH_ARGS(const DriverNameMapType, DRIVER_NAME_MAP, ({ // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)
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

QList<Log>
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

QList<Log>
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

TSqlQuery DatabaseConnection::prepareQuery(const QString &queryString)
{
    // Prepare query string
    auto query = getSqlQuery();

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
