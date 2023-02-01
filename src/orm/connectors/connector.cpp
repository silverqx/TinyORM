#include "orm/connectors/connector.hpp"

#include "orm/configurations/configurationoptionsparser.hpp"
#include "orm/constants.hpp"
#include "orm/exceptions/sqlerror.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::database_;
using Orm::Constants::driver_;
using Orm::Constants::host_;
using Orm::Constants::password_;
using Orm::Constants::port_;
using Orm::Constants::username_;

namespace Orm::Connectors
{

/* protected */

const QString Connector::m_configureErrorMessage =
        QStringLiteral("Connection configuration statement in %1() failed.");

/* public */

QSqlDatabase
Connector::createConnection(const QString &name, const QVariantHash &config,
                            const QString &options)
{
    // TEST null username/password, debug driver code silverqx

    try {
        return createQSqlDatabaseConnection(name, config, options);
    }  catch (const Exceptions::SqlError &e) {
        return tryAgainIfCausedByLostConnection(std::current_exception(), e, name,
                                                config, options);
    }
}

QString Connector::getOptions(const QVariantHash &config) const
{
    /* This is a little different than in the Eloquent, the QSqlDatabase doesn't have
       any default connection options which are common for all drivers, instead,
       every driver has it's own connection options.
       So I have divided it into two options, one are configuration options which are
       defined by the user and the others are the default connector options.
       Options defined by the user are in the configuration's 'options' option and
       connector options are defined in the connector itself as the 'm_options' static
       data member. */
    // Validate, prepare, merge, and concatenate QSqlDatabase connection 'options' option
    return Configurations::ConfigurationOptionsParser
            ::mergeAndConcatenateOptions(getConnectorOptions(), config);
}

/* protected */

QSqlDatabase
Connector::createQSqlDatabaseConnection(const QString &name, const QVariantHash &config,
                                        const QString &options)
{
    /* If the Qt connection repository already contains this connection,
       than obtain it, and if don't, add a database to the list of database
       connections using the driver type. */
    QSqlDatabase db = QSqlDatabase::contains(name)
                      ? QSqlDatabase::database(name, false)
                      : addQSqlDatabaseConnection(name, config, options);

    if (!db.open())
        throw Exceptions::SqlError(
                QStringLiteral("Failed to open database connection in %1().")
                .arg(__tiny_func__),
                db.lastError());

    return db;
}

QSqlDatabase
Connector::addQSqlDatabaseConnection(const QString &name, const QVariantHash &config,
                                     const QString &options)
{
    QSqlDatabase db;

    db = QSqlDatabase::addDatabase(config[driver_].value<QString>(), name);

    db.setHostName(config[host_].value<QString>());

    if (config.contains(database_))
        db.setDatabaseName(config[database_].value<QString>());
    if (config.contains(username_))
        db.setUserName(config[username_].value<QString>());
    if (config.contains(password_))
        db.setPassword(config[password_].value<QString>());
    if (config.contains(port_))
        db.setPort(config[port_].value<int>());

    db.setConnectOptions(options);

    return db;
}

QSqlDatabase
Connector::tryAgainIfCausedByLostConnection(
        const std::exception_ptr &ePtr, const Exceptions::SqlError &e,
        const QString &name, const QVariantHash &config, const QString &options)
{
    if (causedByLostConnection(e))
        return createQSqlDatabaseConnection(name, config, options);

    std::rethrow_exception(ePtr);
}

} // namespace Orm::Connectors

TINYORM_END_COMMON_NAMESPACE
