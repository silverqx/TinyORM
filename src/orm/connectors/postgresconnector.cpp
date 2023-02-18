#include "orm/connectors/postgresconnector.hpp"

#include <QtSql/QSqlQuery>

#include "orm/exceptions/queryerror.hpp"
#include "orm/utils/container.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::DEFAULT;
using Orm::Constants::LOCAL;
using Orm::Constants::NAME;
using Orm::Constants::TMPL_DQUOTES;
using Orm::Constants::charset_;
using Orm::Constants::isolation_level;
using Orm::Constants::search_path;
using Orm::Constants::synchronous_commit;
using Orm::Constants::timezone_;

using ContainerUtils = Orm::Utils::Container;

namespace Orm::Connectors
{

/* public */

ConnectionName
PostgresConnector::connect(const QVariantHash &config) const
{
    auto name = config[NAME].value<QString>();

    /* We need to grab the QSqlDatabse options that should be used while making
       the brand new connection instance. The QSqlDatabase options control various
       aspects of the connection's behavior, and can be overridden by the developers. */
    const auto options = getOptions(config);

    // Create and open new database connection
    const auto connection = createConnection(name, config, options);

    // Transaction isolation level
    configureIsolationLevel(connection, config);

    // Connection encoding
    configureEncoding(connection, config);

    /* Next, we will check to see if a timezone has been specified in this config
       and if it has we will issue a statement to modify the timezone with the
       database. Setting this DB timezone is an optional configuration item. */
    configureTimezone(connection, config);

    configureSearchPath(connection, config);

    /* Postgres allows an application_name to be set by the user and this name is
       used to when monitoring the application with pg_stat_activity. So we'll
       determine if the option has been specified and run a statement if so. */
    configureApplicationName(connection, config);

    configureSynchronousCommit(connection, config);

    /* Return only connection name, because QSqlDatabase documentation doesn't
       recommend to store QSqlDatabase instance as a class data member, we can
       simply obtain the connection by QSqlDatabase::connection() when needed. */
    return name;
}

const QVariantHash &
PostgresConnector::getConnectorOptions() const
{
    return m_options;
}

/* protected */

void PostgresConnector::configureIsolationLevel(const QSqlDatabase &connection,
                                                const QVariantHash &config)
{
    if (!config.contains(isolation_level))
        return;

    QSqlQuery query(connection);

    if (query.exec(QStringLiteral("set session characteristics as "
                                  "transaction isolation level %1;")
                   .arg(config[isolation_level].value<QString>())))
        return;

    throw Exceptions::QueryError(connection.connectionName(),
                                 m_configureErrorMessage.arg(__tiny_func__), query);
}

void PostgresConnector::configureEncoding(const QSqlDatabase &connection,
                                          const QVariantHash &config)
{
    if (!config.contains(charset_))
        return;

    QSqlQuery query(connection);

    if (query.exec(QStringLiteral("set names '%1';")
                   .arg(config[charset_].value<QString>())))
        return;

    throw Exceptions::QueryError(connection.connectionName(),
                                 m_configureErrorMessage.arg(__tiny_func__), query);
}

/*! The key comparison function for the Compare template parameter. */
struct QStringLessCi
{
    /*! Compare the given strings case insenstive (< operator). */
    inline bool operator()(const QString &left, const QString &right) const noexcept // NOLINT(readability-suspicious-call-argument)
    {
        return QString::compare(left, right, Qt::CaseInsensitive) < 0;
    }
};

void PostgresConnector::configureTimezone(const QSqlDatabase &connection,
                                          const QVariantHash &config)
{
    if (!config.contains(timezone_))
        return;

    QSqlQuery query(connection);

    static const std::set<QString, QStringLessCi> local {DEFAULT, LOCAL};

    const auto timezone = config[timezone_].value<QString>();

    if (local.contains(timezone)) {
        if (query.exec(QStringLiteral("set time zone %1;").arg(timezone)))
            return;
    } else
        if (query.exec(QStringLiteral("set time zone '%1';").arg(timezone)))
            return;

    throw Exceptions::QueryError(connection.connectionName(),
                                 m_configureErrorMessage.arg(__tiny_func__), query);
}

void PostgresConnector::configureSearchPath(const QSqlDatabase &connection,
                                            const QVariantHash &config)
{
    if (!config.contains(search_path))
        return;

    QSqlQuery query(connection);

    // Don't add the searchPath.isEmpty() check here to allow set "" (empty search path)

    if (query.exec(QStringLiteral("set search_path to %1;")
                   .arg(quoteSearchPath(
                            parseSearchPath(config[search_path]))))
    )
        return;

    throw Exceptions::QueryError(connection.connectionName(),
                                 m_configureErrorMessage.arg(__tiny_func__), query);
}

QString PostgresConnector::quoteSearchPath(QStringList &&searchPath)
{
    // Allow to set an empty search_path
    if (isSearchPathEmpty(searchPath))
        return QStringLiteral("''");

    // Really nice 😎
    return TMPL_DQUOTES.arg(ContainerUtils::join(searchPath, QStringLiteral("\", \"")));
}

void PostgresConnector::configureApplicationName(const QSqlDatabase &connection,
                                                 const QVariantHash &config)
{
    if (!config.contains("application_name"))
        return;

    QSqlQuery query(connection);

    if (query.exec(QStringLiteral("set application_name to '%1';")
                   .arg(config["application_name"].value<QString>())))
        return;

    throw Exceptions::QueryError(connection.connectionName(),
                                 m_configureErrorMessage.arg(__tiny_func__), query);
}

void PostgresConnector::configureSynchronousCommit(const QSqlDatabase &connection,
                                                   const QVariantHash &config)
{
    if (!config.contains(synchronous_commit))
        return;

    QSqlQuery query(connection);

    if (query.exec(QStringLiteral("set synchronous_commit to '%1';")
                   .arg(config[synchronous_commit].value<QString>())))
        return;

    throw Exceptions::QueryError(connection.connectionName(),
                                 m_configureErrorMessage.arg(__tiny_func__), query);
}

} // namespace Orm::Connectors

TINYORM_END_COMMON_NAMESPACE
