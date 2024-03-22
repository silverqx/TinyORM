#include "orm/configurations/mysqlconfigurationparser.hpp"

#include "orm/constants.hpp"
#include "orm/exceptions/invalidargumenterror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::SSL_CA;
using Orm::Constants::SSL_CERT;
using Orm::Constants::SSL_KEY;
using Orm::Constants::SSL_MODE;
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

    // Copy all SSL-related options from the top-level config. to the 'options' hash
    addSslOptions(options);
}

/* private */

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
            {1, QStringLiteral("TinyORM uses its own reconnector. Also, be aware that "
                               "the MYSQL_OPT_RECONNECT is deprecated from MySQL >=8.1 "
                               "and >=8.0.24 and will be removed in future versions.")}},
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

void MySqlConfigurationParser::addSslOptions(QVariantHash &options) const
{
    /* Copy all SSL-related connection options from the top-level configuration level
       to the 'options' hash. If the options hash already contains the same option, then
       it will be overwritten. */
    copyOptionsFromTopLevel(options, {SSL_CA, SSL_CERT, SSL_KEY, SSL_MODE}, true);
}

} // namespace Orm::Configurations

TINYORM_END_COMMON_NAMESPACE
