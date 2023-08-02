#include "orm/connectors/mysqlconnector.hpp"

#include <QVersionNumber>
#include <QtSql/QSqlQuery>

#include "orm/constants.hpp"
#include "orm/exceptions/queryerror.hpp"
#include "orm/utils/configuration.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::charset_;
using Orm::Constants::collation_;
using Orm::Constants::COMMA;
using Orm::Constants::isolation_level;
using Orm::Constants::NAME;
using Orm::Constants::strict_;
using Orm::Constants::timezone_;

using ConfigUtils = Orm::Utils::Configuration;

namespace Orm::Connectors
{

/* public */

ConnectionName
MySqlConnector::connect(const QVariantHash &config) const
{
    auto name = config[NAME].value<QString>();

    /* We need to grab the QSqlDatabse options that should be used while making
       the brand new connection instance. The QSqlDatabase options control various
       aspects of the connection's behavior, and can be overridden by the developers. */
    const auto options = getOptions(config);

    // Create and open new database connection
    const auto connection = createConnection(name, config, options);

    // Session transaction isolation
    configureIsolationLevel(connection, config);

    // Connection encoding and collation
    configureEncoding(connection, config);

    /* Next, we will check to see if a timezone has been specified in this config
       and if it has we will issue a statement to modify the timezone with the
       database. Setting this DB timezone is an optional configuration item. */
    configureTimezone(connection, config);

    // Set database modes, affected by 'strict' or 'modes' configuration options
    setModes(connection, config);

    /* Return only connection name, because QSqlDatabase documentation doesn't
       recommend to store QSqlDatabase instance as a class data member, we can
       simply obtain the connection by QSqlDatabase::connection() when needed. */
    return name;
}

const QVariantHash &
MySqlConnector::getConnectorOptions() const
{
    return m_options;
}

/* protected */

void MySqlConnector::configureIsolationLevel(const QSqlDatabase &connection,
                                             const QVariantHash &config)
{
    if (!config.contains(isolation_level))
        return;

    QSqlQuery query(connection);

    if (query.exec(QStringLiteral("SET SESSION TRANSACTION ISOLATION LEVEL %1;")
                   .arg(config[isolation_level].value<QString>())))
        return;

    throw Exceptions::QueryError(connection.connectionName(),
                                 m_configureErrorMessage.arg(__tiny_func__), query);
}

void MySqlConnector::configureEncoding(const QSqlDatabase &connection,
                                       const QVariantHash &config)
{
    if (!config.contains(charset_))
        return;

    QSqlQuery query(connection);

    if (query.exec(QStringLiteral("set names '%1'%2;")
                   .arg(config[charset_].value<QString>(), getCollation(config))))
        return;

    throw Exceptions::QueryError(connection.connectionName(),
                                 m_configureErrorMessage.arg(__tiny_func__), query);
}

QString MySqlConnector::getCollation(const QVariantHash &config)
{
    return config.contains(collation_)
            ? QStringLiteral(" collate '%1'").arg(config[collation_].value<QString>())
            : QString("");
}

void MySqlConnector::configureTimezone(const QSqlDatabase &connection,
                                       const QVariantHash &config)
{
    if (!config.contains(timezone_))
        return;

    QSqlQuery query(connection);

    if (query.exec(QStringLiteral("set time_zone=\"%1\";")
                   .arg(config[timezone_].value<QString>())))
        return;

    throw Exceptions::QueryError(connection.connectionName(),
                                 m_configureErrorMessage.arg(__tiny_func__), query);
}

void MySqlConnector::setModes(const QSqlDatabase &connection,
                              const QVariantHash &config)
{
    // Custom modes defined
    if (config.contains("modes"))
        return setCustomModes(connection, config); // clazy:exclude=returning-void-expression

    // No strict defined
    if (!config.contains(strict_))
        return;

    QSqlQuery query(connection);

    // Enable strict mode
    if (config[strict_].value<bool>()) {
        if (query.exec(strictMode(connection, config)))
            return;

        throw Exceptions::QueryError(connection.connectionName(),
                                     m_configureErrorMessage.arg(__tiny_func__), query);
    }

    // Set defaults, no strict mode
    if (query.exec(QStringLiteral("set session sql_mode='NO_ENGINE_SUBSTITUTION'")))
        return;

    throw Exceptions::QueryError(connection.connectionName(),
                                 m_configureErrorMessage.arg(__tiny_func__), query);
}

QString MySqlConnector::strictMode(const QSqlDatabase &connection,
                                   const QVariantHash &config)
{
    const auto version = getMySqlVersion(connection, config);

    /* NO_AUTO_CREATE_USER was removed in 8.0.11 */
    if (QVersionNumber::fromString(version) >= QVersionNumber(8, 0, 11))
        return QStringLiteral("set session sql_mode='ONLY_FULL_GROUP_BY,"
                              "STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,"
                              "ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION'");

    return QStringLiteral("set session sql_mode='ONLY_FULL_GROUP_BY,"
                          "STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,"
                          "ERROR_FOR_DIVISION_BY_ZERO,NO_AUTO_CREATE_USER,"
                          "NO_ENGINE_SUBSTITUTION'");
}

QString MySqlConnector::getMySqlVersion(const QSqlDatabase &connection,
                                        const QVariantHash &config)
{
    static QString MySqlVersionCache;

    // Return the cached MySQL version
    if (!MySqlVersionCache.isEmpty())
        return MySqlVersionCache;

    // Get the MySQL version from the configuration if it was defined and is valid
    if (auto configVersionValue = ConfigUtils::getValidConfigVersion(config);
        !configVersionValue.isEmpty()
    )
        return MySqlVersionCache = configVersionValue;

    // Obtain the MySQL version from the database
    return MySqlVersionCache = getMySqlVersionFromDatabase(connection);
}

void MySqlConnector::setCustomModes(const QSqlDatabase &connection,
                                    const QVariantHash &config)
{
    const auto modes = config["modes"].value<QStringList>().join(COMMA);

    QSqlQuery query(connection);

    if (query.exec(QStringLiteral("set session sql_mode='%1';").arg(modes)))
        return;

    throw Exceptions::QueryError(connection.connectionName(),
                                 m_configureErrorMessage.arg(__tiny_func__), query);
}

/* private */

QString MySqlConnector::getMySqlVersionFromDatabase(const QSqlDatabase &connection)
{
    QSqlQuery query(connection);

    if (!query.exec(QStringLiteral("select version()")))
        throw Exceptions::QueryError(connection.connectionName(),
                                     m_configureErrorMessage.arg(__tiny_func__), query);

    if (!query.first())
        throw Exceptions::RuntimeError(
                QStringLiteral("Error during connection configuration, can not "
                               "obtain the first record in %1().")
                .arg(__tiny_func__));

    auto version = query.value(0).value<QString>();

    if (version.isEmpty())
        throw Exceptions::RuntimeError(
                QStringLiteral("The MySQL or MariaDB server returned an empty database "
                               "version number in %1().")
                .arg(__tiny_func__));

    return version;
}

} // namespace Orm::Connectors

TINYORM_END_COMMON_NAMESPACE
