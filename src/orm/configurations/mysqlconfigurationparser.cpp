#include "orm/configurations/mysqlconfigurationparser.hpp"

#include "orm/constants.hpp"
#include "orm/exceptions/invalidargumenterror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::Version;

namespace Orm::Configurations
{

/* protected */

void MySqlConfigurationParser::parseDriverSpecificOptions() const
{
    if (!config().contains(Version))
        config().insert(Version, {});
}

void MySqlConfigurationParser::parseDriverSpecificOptionsOption(
        QVariantHash &options) const
{
    // Throw if the 'options' hash contains an unsupported option
    throwIfContainsUnsupportedOption(options);
}

/*! The banned value item. */
struct BannedValue
{
    /*! Banned value. */
    QVariant value;
    /*! Error message for the exception. */
    QString  errorMessage;
};

void
MySqlConfigurationParser::throwIfContainsUnsupportedOption(const QVariantHash &options)
{
    // This connection options are banned
    static const std::unordered_map<QString, BannedValue> bannedMap {
        // We have our own reconnector
        {QStringLiteral("MYSQL_OPT_RECONNECT"),
            {1, QStringLiteral(" TinyORM uses its own reconnector.")}},
    };

    for (const auto &[key, banned] : bannedMap) {
        const auto &[value, errorMessage] = banned;

        if (options.contains(key) && options[key] == value)
            throw Exceptions::InvalidArgumentError(
                    QStringLiteral(
                        "The '%1' connection option is not allowed in the TinyORM.%2")
                    .arg(key, errorMessage));
    }
}

} // namespace Orm::Configurations

TINYORM_END_COMMON_NAMESPACE
