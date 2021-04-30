#include "orm/connectors/mysqlconnector.hpp"

#include <QtSql/QSqlQuery>
#include <QVersionNumber>

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Connectors
{

ConnectionName
MySqlConnector::connect(const QVariantHash &config) const
{
    const auto name = config["name"].value<QString>();

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

void MySqlConnector::parseConfigOptions(QVariantHash &options) const
{
    // This connection options are banned
    static const QVariantHash banned {
        // We have our reconnector
        {QStringLiteral("MYSQL_OPT_RECONNECT"), 1},
    };

    auto itOption = banned.constBegin();
    while (itOption != banned.constEnd()) {
        const auto &key = itOption.key();
        const auto &value = itOption.value();

        if (options.contains(key) && options[key] == value)
            throw std::domain_error(
                    "The connection option '" + value.value<QString>().toStdString() +
                    "' is not allowed in the TinyORM, TinyORM uses its own "
                    "reconnector.");

        ++itOption;
    }
}

void MySqlConnector::configureIsolationLevel(const QSqlDatabase &connection,
                                             const QVariantHash &config) const
{
    if (!config.contains("isolation_level"))
        return;

    QSqlQuery query(connection);

    query.exec(QStringLiteral("SET SESSION TRANSACTION ISOLATION LEVEL %1;")
               .arg(config["isolation_level"].value<QString>()));
}

void MySqlConnector::configureEncoding(const QSqlDatabase &connection,
                                       const QVariantHash &config) const
{
    if (!config.contains("charset"))
        return;

    QSqlQuery query(connection);

    query.exec(QStringLiteral("set names '%1'%2;")
               .arg(config["charset"].value<QString>(), getCollation(config)));
}

QString MySqlConnector::getCollation(const QVariantHash &config) const
{
    return config.contains("collation")
            ? QStringLiteral(" collate '%1'").arg(config["collation"].value<QString>())
            : "";
}

void MySqlConnector::configureTimezone(const QSqlDatabase &connection,
                                       const QVariantHash &config) const
{
    QSqlQuery query(connection);

    query.exec(QStringLiteral("set time_zone=\"%1\";")
               .arg(config["timezone"].value<QString>()));
}

void MySqlConnector::setModes(const QSqlDatabase &connection,
                              const QVariantHash &config) const
{
    if (config.contains("modes"))

        setCustomModes(connection, config);

    else if (config.contains("strict")) {

        if (config["strict"].value<bool>()) {
            QSqlQuery query(connection);
            query.exec(strictMode(connection, config));
        }
        else {
            QSqlQuery query(connection);
            query.exec(QStringLiteral("set session sql_mode='NO_ENGINE_SUBSTITUTION'"));
        }
    }
}

QString MySqlConnector::strictMode(const QSqlDatabase &connection,
                                   const QVariantHash &config) const
{
    const auto version = getMySqlVersion(connection, config);

    /* NO_AUTO_CREATE_USER was removed in 8.0.11 */
    if (QVersionNumber::fromString(version) >= QVersionNumber(8, 0, 11))
        return QStringLiteral("set session sql_mode='ONLY_FULL_GROUP_BY,"
                              "STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,"
                              "ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION'");
    else
        return QStringLiteral("set session sql_mode='ONLY_FULL_GROUP_BY,"
                              "STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,"
                              "ERROR_FOR_DIVISION_BY_ZERO,NO_AUTO_CREATE_USER,"
                              "NO_ENGINE_SUBSTITUTION'");
}

QString MySqlConnector::getMySqlVersion(const QSqlDatabase &connection,
                                        const QVariantHash &config) const
{
    QString version;

    // Get the MySQL version from the configuration if it was defined
    if (config.contains("version") && !config["version"].value<QString>().isEmpty())
        version = config["version"].value<QString>();

    // Obtain the MySQL version from the database
    else {
        QSqlQuery query(connection);

        if (!query.exec(QStringLiteral("select version()")))
            return "";

        // TODO next handle query errors, in whole class, check all queries silverqx
        if (!query.first())
            return "";

        version = query.value(0).value<QString>();
    }

    return version;
}

void MySqlConnector::setCustomModes(const QSqlDatabase &connection,
                                    const QVariantHash &config) const
{
    const auto modes = config["modes"].value<QStringList>().join(", ");

    QSqlQuery query(connection);

    query.exec(QStringLiteral("set session sql_mode='%1';").arg(modes));
}

} // namespace Orm::Connectors
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
