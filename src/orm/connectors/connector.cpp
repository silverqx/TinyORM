#include "orm/connectors/connector.hpp"

#include "orm/constants.hpp"
#include "orm/exceptions/sqlerror.hpp"
#include "orm/support/configurationoptionsparser.hpp"
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
                            const QString &options) const
{
    // TEST null username/password, debug driver code silverqx

    try {
        return createQSqlDatabaseConnection(name, config, options);
    }  catch (const Exceptions::SqlError &e) {
        return tryAgainIfCausedByLostConnection(std::current_exception(), e, name,
                                                config, options);
    }
}

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

/* protected */

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
        const QString &name, const QVariantHash &config, const QString &options) const
{
    if (causedByLostConnection(e))
        return createQSqlDatabaseConnection(name, config, options);

    std::rethrow_exception(ePtr);
}

} // namespace Orm::Connectors

TINYORM_END_COMMON_NAMESPACE
