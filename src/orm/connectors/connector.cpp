#include "orm/connectors/connector.hpp"

#include "orm/sqlerror.hpp"
#include "orm/support/configurationoptionsparser.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Connectors
{

const QString Connector::m_configureErrorMessage =
        "Connection configuration statement in %1() failed.";

QSqlDatabase
Connector::createConnection(const QString &name, const QVariantHash &config,
                            const QString &options) const
{
    // TEST null username/password, debug driver code silverqx

    try {
        return createQSqlDatabaseConnection(name, config, options);
    }  catch (const SqlError &e) {
        return tryAgainIfCausedByLostConnection(std::current_exception(), e, name,
                                                config, options);
    }
}

QSqlDatabase
Connector::createQSqlDatabaseConnection(const QString &name, const QVariantHash &config,
                                        const QString &options) const
{
    /* If the Qt connection repository already contains this connection,
       than obtain it, and if don't, add a database to the list of database
       connections using the driver type. */
    QSqlDatabase db = QSqlDatabase::contains(name)
                      ? QSqlDatabase::database(name, false)
                      : addQSqlDatabaseConnection(name, config, options);

    if (!db.open())
        throw SqlError(QStringLiteral("Open databse connection in %1() failed.")
                       .arg(__FUNCTION__),
                       db.lastError());

    return db;
}

QString Connector::getOptions(const QVariantHash &config) const
{
    /* This is little different than in the Eloquent, QSqlDatabase doesn't have
       any default connection options which are common for all drivers, instead
       every driver has it's own connection options.
       So I have divided it into two options, one are config options which are
       defined by the user and others are connector options.
       Options defined by a user are in the config's 'options' parameter and
       connector options are defined in the connector itself as 'm_options'
       data member. */
    // Validate, prepare, and merge connection options
    return Support::ConfigurationOptionsParser(*this)
            .parseConfiguration(config);
}

QSqlDatabase
Connector::addQSqlDatabaseConnection(const QString &name, const QVariantHash &config,
                                     const QString &options) const
{
    QSqlDatabase db;

    db = QSqlDatabase::addDatabase(config["driver"].value<QString>(), name);

    db.setHostName(config["host"].value<QString>());

    if (config.contains("database"))
        db.setDatabaseName(config["database"].value<QString>());
    if (config.contains("username"))
        db.setUserName(config["username"].value<QString>());
    if (config.contains("password"))
        db.setPassword(config["password"].value<QString>());
    if (config.contains("port"))
        db.setPort(config["port"].value<uint>());

    db.setConnectOptions(options);

    return db;
}

QSqlDatabase
Connector::tryAgainIfCausedByLostConnection(
        const std::exception_ptr &ePtr, const SqlError &e, const QString &name,
        const QVariantHash &config, const QString &options) const
{
    if (causedByLostConnection(e))
        return createQSqlDatabaseConnection(name, config, options);

    std::rethrow_exception(ePtr);
}

} // namespace Orm::Connectors
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
