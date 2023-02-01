#include "orm/configurations/sqliteconfigurationparser.hpp"

#include "orm/constants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::return_qdatetime;

namespace Orm::Configurations
{

/* protected */

void SQLiteConfigurationParser::parseDriverSpecificOptions() const
{
    if (!config().contains(return_qdatetime))
        config().insert(return_qdatetime, true);
}

void SQLiteConfigurationParser::parseDriverSpecificOptionsOption(
        QVariantHash &/*unused*/) const
{}

} // namespace Orm::Configurations

TINYORM_END_COMMON_NAMESPACE
