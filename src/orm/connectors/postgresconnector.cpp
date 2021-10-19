#include "orm/connectors/postgresconnector.hpp"

#include <QRegularExpression>
#include <QtSql/QSqlQuery>

#include "orm/constants.hpp"
#include "orm/exceptions/queryerror.hpp"
#include "orm/utils/type.hpp"

using namespace Orm::Constants;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Connectors
{

ConnectionName
PostgresConnector::connect(const QVariantHash &config) const
{
    const auto name = config[NAME].value<QString>();

    /* We need to grab the QSqlDatabse options that should be used while making
       the brand new connection instance. The QSqlDatabase options control various
       aspects of the connection's behavior, and can be overridden by the developers. */
    const auto options = getOptions(config);

    // Create and open new database connection
    const auto connection = createConnection(name, config, options);

    // Connection encoding
    configureEncoding(connection, config);

    /* Next, we will check to see if a timezone has been specified in this config
       and if it has we will issue a statement to modify the timezone with the
       database. Setting this DB timezone is an optional configuration item. */
    configureTimezone(connection, config);

    configureSchema(connection, config);

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

void PostgresConnector::parseConfigOptions(QVariantHash &) const
{}

void PostgresConnector::configureEncoding(const QSqlDatabase &connection,
                                          const QVariantHash &config) const
{
    if (!config.contains("charset"))
        return;

    QSqlQuery query(connection);

    if (query.exec(QStringLiteral("set names '%1'")
                   .arg(config["charset"].value<QString>())))
        return;

    throw Exceptions::QueryError(m_configureErrorMessage.arg(__tiny_func__), query);
}

void PostgresConnector::configureTimezone(const QSqlDatabase &connection,
                                          const QVariantHash &config) const
{
    if (!config.contains("timezone"))
        return;

    QSqlQuery query(connection);

    static const QStringList local {"local", "default"};

    const auto timezone = config["timezone"].value<QString>();

    if (local.contains(timezone, Qt::CaseInsensitive)) {
        if (query.exec(QStringLiteral("set time zone %1").arg(timezone)))
            return;
    } else
        if (query.exec(QStringLiteral("set time zone '%1'").arg(timezone)))
            return;

    throw Exceptions::QueryError(m_configureErrorMessage.arg(__tiny_func__), query);
}

void PostgresConnector::configureSchema(const QSqlDatabase &connection,
                                        const QVariantHash &config) const
{
    if (!config.contains("schema"))
        return;

    QSqlQuery query(connection);

    const auto schema = formatSchema(config["schema"].value<QStringList>());

    if (query.exec(QStringLiteral("set search_path to %1").arg(schema)))
        return;

    throw Exceptions::QueryError(m_configureErrorMessage.arg(__tiny_func__), query);
}

QString PostgresConnector::formatSchema(QStringList schema) const
{
    /* A schema configuration option can be passed as QString and also
       as QStringList at once. */
    if (schema.size() == 1)
        schema = schema.at(0).trimmed()
                 .split(QRegularExpression("\\s*(?:,|;)\\s*"), Qt::SkipEmptyParts);

    // Really nice 😎
    return QStringLiteral("\"%1\"").arg(schema.join(QLatin1String("\", \"")));
}

void PostgresConnector::configureApplicationName(const QSqlDatabase &connection,
                                                 const QVariantHash &config) const
{
    if (!config.contains("application_name"))
        return;

    QSqlQuery query(connection);

    if (query.exec(QStringLiteral("set application_name to '%1'")
                   .arg(config["application_name"].value<QString>())))
        return;

    throw Exceptions::QueryError(m_configureErrorMessage.arg(__tiny_func__), query);
}

void PostgresConnector::configureSynchronousCommit(const QSqlDatabase &connection,
                                                   const QVariantHash &config) const
{
    if (!config.contains("synchronous_commit"))
        return;

    QSqlQuery query(connection);

    if (query.exec(QStringLiteral("set synchronous_commit to '%1'")
                   .arg(config["synchronous_commit"].value<QString>())))
        return;

    throw Exceptions::QueryError(m_configureErrorMessage.arg(__tiny_func__), query);
}

} // namespace Orm::Connectors

TINYORM_END_COMMON_NAMESPACE
