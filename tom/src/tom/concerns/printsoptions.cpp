#include "tom/concerns/printsoptions.hpp"

#include <orm/constants.hpp>

#include "tom/concerns/interactswithio.hpp"
#include "tom/exceptions/runtimeerror.hpp"
#include "tom/tomconstants.hpp"
#include "tom/tomutils.hpp"
#include "tom/types/commandlineoption.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::COMMA;
using Orm::Constants::SPACE;

using Tom::Constants::ShortOption;
using Tom::Constants::LongOption;
using Tom::Constants::LongOptionOnly;
using Tom::Constants::LongOptionValue;

using TomUtils = Tom::Utils;

namespace Tom::Concerns
{

/* public */

PrintsOptions::PrintsOptions(const QList<CommandLineOption> &options,
                             const Concerns::InteractsWithIO &io)
    : m_options(options)
    , m_io(io)
{}

PrintsOptions::SizeType
PrintsOptions::printOptionsSection(const bool commonOptions) const
{
    io().newLine();
    io().comment(commonOptions ? u"Common options:"_s : u"Options:"_s);

    // Get max. option size in all options
    const auto optionsMaxSize = this->optionsMaxSize();

    // Print options to the console
    printOptions(optionsMaxSize);

    return optionsMaxSize;
}

/* private */

QStringList PrintsOptions::createOptionNamesList(const QCommandLineOption &option)
{
    QStringList options;

    for (const auto names = option.names();
         const auto &name : names
    )
        // Short option
        if (name.size() == 1)
            options << ShortOption
                       // Custom logic for the verbose option, good enough 😎
                       .arg(name == u'v' ? u"v|vv|vvv"_s : name);

        // Long option
        else
            // Short and long options passed
            if (const auto valueName = option.valueName();
                names.size() == 2
            ) {
                // Without the value
                if (valueName.isEmpty())
                    options << LongOption.arg(name);
                // With the value
                else
                    options << LongOptionValue.arg(name, valueName);
            }
            // Only a long option passed
            else {
                // Without a value
                if (valueName.isEmpty())
                    options << LongOptionOnly.arg(LongOption.arg(name));
                // With a value
                else
                    options << LongOptionOnly.arg(LongOptionValue.arg(name, valueName));
            }

    return options;
}

PrintsOptions::SizeType
PrintsOptions::optionsMaxSize() const
{
    SizeType optionsMaxSize = 0;

    for (const auto &option : options()) {

        validateOption(option);

        optionsMaxSize = std::max(optionsMaxSize,
                                  createOptionNamesList(option).join(COMMA).size());
    }

    return optionsMaxSize;
}

void PrintsOptions::printOptions(const SizeType optionsMaxSize) const
{
    for (const auto &option : options()) {

        // Don't show hidden options
        if (option.hidden())
            continue;

        const auto joinedOptions = createOptionNamesList(option).join(COMMA);

        const auto indent = QString(optionsMaxSize - joinedOptions.size(), SPACE);

        io().info(u"  %1%2  "_s.arg(joinedOptions, indent), false);

        io().note(option.description(), false);

        // Print option's default value
        printOptionDefaultValue(option);
    }
}

void PrintsOptions::printOptionDefaultValue(const QCommandLineOption &option) const
{
    const auto defaultValues = option.defaultValues();

    // Multiple default values are not supported
    Q_ASSERT(defaultValues.size() <= 1);

    if (defaultValues.isEmpty())
        io().newLine();
    else
        io().comment(TomUtils::defaultValueText(defaultValues.constFirst()));
}

void PrintsOptions::validateOption(const QCommandLineOption &option)
{
    const auto optionNames = option.names();
    const auto optionsSize = optionNames.size();

    // This is may be too much, but whatever after 14 hours of coding 😎
    if (optionsSize > 2)
        throw Exceptions::RuntimeError(
                u"Only two option names per option are allowed (options: %1)."_s
                .arg(optionNames.join(COMMA)));

    static const auto longOptionTmpl = u"Long option size has to be >1 (option: %1)."_s;

    // One option name was passed
    if (optionsSize == 1) {
        if (const auto &longOption = optionNames.constFirst(); longOption.size() <= 1)
            throw Exceptions::RuntimeError(longOptionTmpl.arg(longOption));
    }
    // Two option names were passed
    else if (optionsSize == 2) {
        if (const auto &shortOption = optionNames.constFirst(); shortOption.size() != 1)
            throw Exceptions::RuntimeError(
                    u"Short option size has to be 1 (option: %1)."_s
                    .arg(shortOption));

        if (const auto &longOption = optionNames[1]; longOption.size() <= 1)
            throw Exceptions::RuntimeError(longOptionTmpl.arg(longOption));
    }
}

} // namespace Tom::Concerns

TINYORM_END_COMMON_NAMESPACE
