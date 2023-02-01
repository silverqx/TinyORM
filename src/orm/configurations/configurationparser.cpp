#include "orm/configurations/configurationparser.hpp"

#include "orm/macros/likely.hpp"
#include "orm/ormtypes.hpp"
#include "orm/utils/configuration.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using ConfigUtils = Orm::Utils::Configuration;

namespace Orm::Configurations
{

/* First, the common options at the top-level are parsed, then the driver-specific
   options are parsed. This parsed configuration is called the original configuration and
   is accessible by the DatabaseManager::originalConfig() method.
   The copy of this original configuration is made at this point, after the copy,
   the qt_timezone configuration option and the connection 'options' option are parsed.
   The 'options' option is parsed by the ConfigurationOptionsParser class, every driver
   has it's own parsing logic.
   Look also at the Connector::getOptions() method for more info.
   At this point, all the configuration is parsed and can be passed to the database
   connection, this parsed configuration is accessible by
   the DatabaseManager::getConfig(connection) method.
   The database configuration is parsed as late as possible, precisely isn't parsed
   during the DatabaseManager::addConnection(configuration) call, but it's parsed if
   you try to obtain the connection from the DM eg. with DatabaseManager::connection()
   call or if the first query query is sent to the database eg. with
   the DatabaseManager::select() method or similar methods. */

/* public */

QVariantHash
ConfigurationParser::parseConfiguration(QVariantHash &config,
                                        const ConnectionName &connection) const
{
    // Cache the references, it helps to avoid passing them to every method
    m_config = config;
    m_connection = connection;

    // Parse common configuration options for all supported databases
    parseCommonOptions();
    // Parse the driver-specific database configuration
    parseDriverSpecificOptions();

    /* Changes after this point will be saved only in the DatabaseConnection
       configuration, the original DatabaseManager configuration will remain intact. */
    auto configCopy = this->config();

    // Parse the qt_timezone configuration option
    parseQtTimeZone(configCopy);
    // Parse the 'options' configuration option
    parseOptionsOption(configCopy);

    return configCopy;
}

/* private */

void
ConfigurationParser::parseCommonOptions() const
{
    /* Insert/normalize needed configuration values, following inserted values will be
       also changed in the so called the original configuration that will be saved
       in the DatabaseManager only. */
    config().insert(NAME, connection());

    // Modify the driver name to uppercase
    normalizeDriverName();

    if (!config().contains(database_))
        config().insert(database_, EMPTY);

    if (!config().contains(prefix_))
        config().insert(prefix_, EMPTY);

    if (!config().contains(options_))
        config().insert(options_, QVariantHash());

    if (!config().contains(prefix_indexes))
        config().insert(prefix_indexes, false);
}

void ConfigurationParser::normalizeDriverName() const
{
    if (config().contains(driver_)) T_LIKELY {
        auto &driver = config()[driver_];

        driver = driver.value<QString>().toUpper();
    }
    else T_UNLIKELY
        config().insert(driver_, EMPTY);
}

void ConfigurationParser::parseQtTimeZone(QVariantHash &config) const
{
    auto &qtTimeZone = config[qt_timezone];

    // Nothing to parse, already contains the QtTimeZoneConfig
    if (qtTimeZone.canConvert<QtTimeZoneConfig>())
        return;

    qtTimeZone = QVariant::fromValue(
                     ConfigUtils::prepareQtTimeZone(config, connection()));
}

} // namespace Orm::Configurations

TINYORM_END_COMMON_NAMESPACE
