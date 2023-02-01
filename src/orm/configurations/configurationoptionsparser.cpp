#include "orm/configurations/configurationoptionsparser.hpp"

#include "orm/constants.hpp"
#include "orm/exceptions/invalidargumenterror.hpp"
#include "orm/utils/helpers.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::EQ_C;
using Orm::Constants::SEMICOLON;
using Orm::Constants::options_;

using Orm::Utils::Helpers;

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
    configOptions = std::move(preparedConfigOptions);
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

/* private */

void ConfigurationOptionsParser::validateConfigOptions(const QVariant &options)
{
    if (Helpers::qVariantTypeId(options) == QMetaType::QString ||
        options.canConvert<QVariantHash>()
    )
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
    if (Helpers::qVariantTypeId(options) != QMetaType::QString)
        return options.value<QVariantHash>();

    // The following algorithm converts the QString defined 'options' to the QVariantHash
    const auto optionsString = options.value<QString>();
    // QStringView saves 3 copies
    const auto list = QStringView(optionsString).split(SEMICOLON, Qt::SkipEmptyParts);

    QVariantHash preparedOptions;
    preparedOptions.reserve(list.size());

    for (const auto value : list) {
        Q_ASSERT(value.count(EQ_C) == 1);

        const auto option = value.split(EQ_C);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        preparedOptions.emplace(
#else
        preparedOptions.insert(
#endif
                    option.constFirst().trimmed().toString(),
                    option[1].trimmed().toString());
    }

    return preparedOptions;
}

QVariantHash
ConfigurationOptionsParser::mergeOptions(const QVariantHash &connectortOptions,
                                         QVariantHash &&preparedConfigOptions)
{
    // Move to the variable with a better name
    const auto preparedConfigOptionsSize = preparedConfigOptions.size();
    QVariantHash merged = std::move(preparedConfigOptions);
    merged.reserve(merged.size() + preparedConfigOptionsSize);

    /* Insert options from the default connector options hash, if the prepared
       configuration options already doesn't contain it, so user can overwrite
       default connector options. */
    auto itDefault = connectortOptions.constBegin();
    while (itDefault != connectortOptions.constEnd()) {
        const auto &key = itDefault.key();
        const auto &value = itDefault.value();

        if (!merged.contains(key))
            merged.insert(key, value);

        ++itDefault;
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
        const auto &value = itOption.value();

        concatenated << QStringLiteral("%1=%2").arg(key, value.value<QString>());

        ++itOption;
    }

    return concatenated.join(SEMICOLON);
}

} // namespace Orm::Configurations

TINYORM_END_COMMON_NAMESPACE
