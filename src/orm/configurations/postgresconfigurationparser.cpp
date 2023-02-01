#include "orm/configurations/postgresconfigurationparser.hpp"

#include "orm/constants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::dont_drop;
using Orm::Constants::spatial_ref_sys;

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
        QVariantHash &/*unused*/) const
{}

} // namespace Orm::Configurations

TINYORM_END_COMMON_NAMESPACE
