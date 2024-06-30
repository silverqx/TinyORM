#include "orm/configurations/configurationoptionsparser.hpp"

#include "orm/configurations/configurationparser.hpp"
#include "orm/constants.hpp"
#include "orm/exceptions/invalidargumenterror.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::COMMA_C;
using Orm::Constants::EMPTY;
using Orm::Constants::EQ_C;
using Orm::Constants::SEMICOLON;
using Orm::Constants::options_;

namespace Orm::Configurations
{

/* public */

void ConfigurationOptionsParser::parseOptionsOption(QVariantHash &config) const
{
    // Get options from a user configuration
    auto &configOptions = config[options_];

    // Validate the options type in the user-defined configuration
    validateConfigOptions(configOptions);

    /* Prepare options for preparedConfigurationOptions() function, convert
       to the QVariantHash if needed. */
    auto preparedConfigOptions = prepareConfigOptions(configOptions);

    // Parse the driver-specific 'options' configuration option (validation/modification)
    parseDriverSpecificOptionsOption(preparedConfigOptions);

    // Swap the original 'options' with the newly prepared 'options' hash
    configOptions.setValue(std::move(preparedConfigOptions)); // NOLINT(performance-move-const-arg)
}

QString
ConfigurationOptionsParser::mergeAndConcatenateOptions(
        const QVariantHash &connectortOptions, const QVariantHash &config)
{
    // Merge the TinyORM's default connector 'options' with the user-defined options
    const auto mergedOptions = mergeOptions(connectortOptions,
                                            config[options_].value<QVariantHash>());

    // Return in the format expected by the QSqlDatabase
    return concatenateOptions(mergedOptions);
}

/* protected */

void ConfigurationOptionsParser::copyOptionsFromTopLevel(
        QVariantHash &options, std::vector<QString> &&optionNames, // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
        const bool checkLowerCase) const
{
    /* Copy the given connection options from the top-level configuration level
       to the 'options' hash. If the options hash already contains the same option, then
       it will be overwritten. */
    for (auto &&option : optionNames) {
        /* Support top-level option names in lowercase, it's used only by the MySQL,
           eg. SSL_CA vs ssl_ca. Newly created option will have the original requested
           name. The lowercase name is used only during the check and obtain a value. */
        const auto optionTopLevel = getTopLevelOptionName(option, checkLowerCase);

        // Nothing to do, the original configuration doesn't contain it
        if (optionTopLevel.isEmpty())
            continue;

        // Copy the value to the 'options' hash
        if (auto newValue = config().value(optionTopLevel);
            !newValue.value<QString>().isEmpty()
        )
            options.emplace(std::move(option), std::move(newValue));
    }
}

/* private */

void ConfigurationOptionsParser::validateConfigOptions(const QVariant &options)
{
    if (options.typeId() == QMetaType::QString || options.canConvert<QVariantHash>())
        return;

    throw Exceptions::InvalidArgumentError(
                QStringLiteral(
                    "Passed an unsupported 'options' type in the connection "
                    "configuration, it has to be the QString or QVariantHash type "
                    "in %1().")
                .arg(__tiny_func__));
}

QVariantHash ConfigurationOptionsParser::prepareConfigOptions(const QVariant &options)
{
    /* Nothing to do, already contains the QVariantHas. Input is already validated, so
       we can be sure that the 'options' option type is the QVariantHash. */
    if (options.typeId() != QMetaType::QString)
        return options.value<QVariantHash>();

    /* The following algorithm converts the QString defined 'options' to the QVariantHash.
       QStringView saves 3 copies (per one split operation). */
    const auto optionsRaw = splitConfigOptions(options.value<QString>());

    QVariantHash preparedOptions;
    preparedOptions.reserve(optionsRaw.size());

    /* The following logic must be 1:1 as in MySqlDriverPrivate::parseMySqlOption() to
       work properly, when we have another drivers we will have to ensure it for them
       as well. */
    for (const auto optionRaw : optionsRaw) {
        /* Can contain 0 or 1 = character; 0 for flags and 1 for options with a value.
           An option flag with no value is considered to be ON/TRUE (enabled). */
        const auto optionRawCount = optionRaw.count(EQ_C);
        Q_ASSERT(optionRawCount >= 0 && optionRawCount <= 1);

        const auto option = optionRaw.split(EQ_C);

        preparedOptions.emplace(
                    option.constFirst().trimmed().toString(),
                    optionRawCount == 0 ? EMPTY : option[1].trimmed().toString());
    }

    return preparedOptions;
}

QList<QStringView>
ConfigurationOptionsParser::splitConfigOptions(const QString &optionsString)
{
    QList<QStringView> list;
    list.reserve(optionsString.count(SEMICOLON) + optionsString.count(COMMA_C) + 2);

    // Split by the ; and also ,
    for (auto &&value : QStringView(optionsString).split(SEMICOLON, Qt::SkipEmptyParts))
        list << value.split(COMMA_C, Qt::SkipEmptyParts);

    return list;
}

QVariantHash
ConfigurationOptionsParser::mergeOptions(const QVariantHash &connectortOptions,
                                         QVariantHash &&preparedConfigOptions)
{
    QVariantHash merged(std::move(preparedConfigOptions));
    merged.reserve(merged.size() + connectortOptions.size());

    /* Insert options from the default connector options hash, if the prepared
       configuration options already doesn't contain it, so user can overwrite
       default connector options. */
    auto itConnectortOptions = connectortOptions.constBegin();
    while (itConnectortOptions != connectortOptions.constEnd()) {
        const auto &key = itConnectortOptions.key();
        const auto &value = itConnectortOptions.value();

        if (!merged.contains(key))
            merged.emplace(key, value);

        ++itConnectortOptions;
    }

    return merged;
}

QString ConfigurationOptionsParser::concatenateOptions(const QVariantHash &options)
{
    QStringList concatenated;
    concatenated.reserve(options.size());

    auto itOption = options.constBegin();
    while (itOption != options.constEnd()) {
        const auto &key = itOption.key();
        const auto value = itOption.value().value<QString>();

        concatenated.emplaceBack(
                    // Support option flags without a value (are considred as enabled)
                    value.isEmpty() ? key : QStringLiteral("%1=%2").arg(key, value));

        ++itOption;
    }

    return concatenated.join(SEMICOLON);
}

QString
ConfigurationOptionsParser::getTopLevelOptionName(const QString &option,
                                                  const bool checkLowerCase) const
{
    if (config().contains(option))
        return option;

    // Allow to define the option name in lowercase
    if (checkLowerCase)
        if (auto optionNameLower = option.toLower();
            config().contains(optionNameLower)
        )
            return optionNameLower;

    return {};
}

QVariantHash &ConfigurationOptionsParser::config() const
{
    return parser().config();
}

const ConfigurationParser &ConfigurationOptionsParser::parser() const
{
    return dynamic_cast<const ConfigurationParser &>(*this);
}

} // namespace Orm::Configurations

TINYORM_END_COMMON_NAMESPACE
