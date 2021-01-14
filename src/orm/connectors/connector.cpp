#include "orm/connectors/connector.hpp"

#include "orm/support/configurationoptionsparser.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Connectors
{

QSqlDatabase
Connector::createConnection(const QString &name, const QVariantHash &config,
                            const QString &options) const
{
    return createQSqlDatabaseConnection(name, config, options);
}

QSqlDatabase
Connector::createQSqlDatabaseConnection(const QString &name, const QVariantHash &config,
                                        const QString &options) const
{
    auto db = QSqlDatabase::addDatabase(config["driver"].toString(), name);

    db.setHostName(config["host"].toString());

    if (config.contains("database"))
        db.setDatabaseName(config["database"].toString());
    if (config.contains("username"))
        db.setUserName(config["username"].toString());
    if (config.contains("password"))
        db.setPassword(config["password"].toString());
    if (config.contains("port"))
        db.setPort(config["port"].toUInt());

    db.setConnectOptions(options);

    // TODO now handle db.open() error silverqx
    db.open();

    return db;
}

QString Connector::getOptions(const QVariantHash &config) const
{
    /* This is little different than in the Eloquent, QSqlDatabase doesn't have
       any default connection options which are common for all drivers, instead
       every driver has it's own connection options.
       So I have divided it into two options, one are config options which are
       defined by the user and others are connector options. */
    // Validate, prepare, and merge connection options
    return Support::ConfigurationOptionsParser(*this)
            .parseConfiguration(config);
}

} // namespace Orm::Connectors
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
