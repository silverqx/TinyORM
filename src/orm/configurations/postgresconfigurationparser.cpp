#include "orm/configurations/postgresconfigurationparser.hpp"

#include "orm/constants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::dont_drop;
using Orm::Constants::spatial_ref_sys;
using Orm::Constants::sslmode_;
using Orm::Constants::sslcert;
using Orm::Constants::sslkey;
using Orm::Constants::sslrootcert;

namespace Orm::Configurations
{

/* protected */

void PostgresConfigurationParser::parseDriverSpecificOptions() const
{
    if (!config().contains(dont_drop))
        // The spatial_ref_sys table is used by the PostGIS
        config().insert(dont_drop, QStringList {spatial_ref_sys});
}

void PostgresConfigurationParser::parseDriverSpecificOptionsOption(
        QVariantHash &options) const
{
    // Copy all SSL-related options from the top-level config. to the 'options' hash
    addSslOptions(options);
}

/* private */

void PostgresConfigurationParser::addSslOptions(QVariantHash &options) const
{
    /* Copy all SSL-related connection options from the top-level configuration level
       to the 'options' hash. If the options hash already contains the same option, then
       it will be overwritten. */
    for (auto &&option : {sslmode_, sslcert, sslkey, sslrootcert}) {
        // Nothing to do, the original configuration doesn't contain it
        if (!config().contains(option))
            continue;

        // Copy the value to the 'options' hash
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        if (const auto &newValue = config()[option];
            !newValue.value<QString>().isEmpty()
        )
            options.insert(option, newValue);
#else
        if (auto newValue = config().value(option);
            !newValue.value<QString>().isEmpty()
        )
            options.emplace(std::move(const_cast<QString &>(option)),
                            std::move(newValue));
#endif
    }
}

} // namespace Orm::Configurations

TINYORM_END_COMMON_NAMESPACE
