#include "orm/configurations/configurationparserfactory.hpp"

#include "orm/configurations/mysqlconfigurationparser.hpp"
#include "orm/configurations/postgresconfigurationparser.hpp"
#include "orm/configurations/sqliteconfigurationparser.hpp"
#include "orm/constants.hpp"
#include "orm/exceptions/invalidargumenterror.hpp"
#include "orm/macros/threadlocal.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::QMYSQL;
using Orm::Constants::QPSQL;
using Orm::Constants::QSQLITE;
using Orm::Constants::driver_;

namespace Orm::Configurations
{

/* public */

const ConfigurationParserInterface &
ConfigurationParserFactory::cachedParser(const QVariantHash &config)
{
    // Get a normalized driver name (using the toUpper())
    const auto driver = getDriverName(config);

    T_THREAD_LOCAL
    static std::unordered_map<QString,
    std::unique_ptr<ConfigurationParserInterface>> cache;

    if (cache.contains(driver))
        return *cache.at(driver);

    // Create a new configuration parser instance and save it to the cache
    auto [it, ok] = cache.emplace(driver, make(driver));
    Q_ASSERT(ok);

    return *it->second;
}

std::unique_ptr<ConfigurationParserInterface>
ConfigurationParserFactory::make(const QString &driver)
{
    if (driver == QMYSQL)
        return std::make_unique<MySqlConfigurationParser>();

    if (driver == QPSQL)
        return std::make_unique<PostgresConfigurationParser>();

    if (driver == QSQLITE)
        return std::make_unique<SQLiteConfigurationParser>();

//    if (driver == "SQLSRV")
//        return std::make_unique<SqlServerConfigurationParser>();

    throw Exceptions::InvalidArgumentError(
                QStringLiteral("Unsupported driver option '%1' in %2().")
                .arg(driver, __tiny_func__));
}

/* private */

QString ConfigurationParserFactory::getDriverName(const QVariantHash &config)
{
    // This method works with the user defined data, so I left this check here
    if (config.contains(driver_))
        // Normalized driver name (using the toUpper())
        return config[driver_].value<QString>().toUpper();

    throw Exceptions::InvalidArgumentError(
                QStringLiteral("A 'driver' configuration parameter must be specified "
                               "in %1().")
                .arg(__tiny_func__));
}

} // namespace Orm::Configurations

TINYORM_END_COMMON_NAMESPACE
