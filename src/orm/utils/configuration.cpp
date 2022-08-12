#include "orm/utils/configuration.hpp"

#include <QVariantHash>
#include <QVersionNumber>

#include "orm/constants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::Version;

namespace Orm::Utils
{

/* public */

bool Configuration::hasValidConfigVersion(const QVariantHash &config)
{
    return !getValidConfigVersion(config).isEmpty();
}

QString Configuration::getValidConfigVersion(const QVariantHash &config)
{
    if (config.contains(Version))
        if (const auto version = config.value(Version);
            version.isValid() && !version.isNull() && version.canConvert<QString>()
        ) {
            auto versionValue = version.value<QString>();

            // Validate whether a version number is correctly formatted
            if (const auto versionNumber = QVersionNumber::fromString(versionValue);
                !versionNumber.isNull()
            )
                return versionValue;
        }

    return {};
}

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE
