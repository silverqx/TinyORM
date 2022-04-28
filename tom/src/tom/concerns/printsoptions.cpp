#include "tom/concerns/printsoptions.hpp"

#include <orm/constants.hpp>

#include "tom/application.hpp"
#include "tom/commands/command.hpp"
#include "tom/tomconstants.hpp"

using Orm::Constants::COMMA;
using Orm::Constants::SPACE;

using Tom::Constants::ShortOption;
using Tom::Constants::LongOption;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Concerns
{

PrintsOptions::PrintsOptions(const Commands::Command &command, const int /*unused*/)
    : m_command(command)
{}

int PrintsOptions::printOptionsSection(const bool commonOptions) const
{
    m_command.get().newLine();
    m_command.get().comment(commonOptions ? QStringLiteral("Common options:")
                                          : QStringLiteral("Options:"));

    // Get max. option size in all options
    int optionsMaxSize = this->optionsMaxSize();
    // Print options to the console
    printOptions(optionsMaxSize);

    return optionsMaxSize;
}

int PrintsOptions::optionsMaxSize() const
{
    int optionsMaxSize = 0;

    for (const auto &option : std::as_const(m_command.get().application().m_options)) {
        QStringList options;

        for (const auto &names = option.names();
             const auto &name : names
        )
            // Short option
            if (name.size() == 1)
                options << ShortOption
                           // Custom logic for the verbose option, good enough 😎
                           .arg(name == QChar('v') ? QStringLiteral("v|vv|vvv")
                                                   : name);

            // Long option
            else
                // Short and long options passed
                if (names.size() == 2)
                    options << LongOption.arg(name);
                // Only long option passed
                else
                    options << QStringLiteral("    --%1").arg(name);

        optionsMaxSize = std::max<int>(optionsMaxSize,
                                       static_cast<int>(options.join(COMMA).size()));
    }

    return optionsMaxSize;
}

void PrintsOptions::printOptions(const int optionsMaxSize) const
{
    for (const auto &option : std::as_const(m_command.get().application().m_options)) {
        QStringList options;

        for (const auto &names = option.names();
             const auto &name : names
        )
            // Short option
            if (name.size() == 1)
                options << ShortOption
                           // Custom logic for the verbose option, good enough 😎
                           .arg(name == QChar('v') ? QStringLiteral("v|vv|vvv")
                                                   : name);

            // Long option
            else
                // Short and long options passed
                if (names.size() == 2)
                    options << LongOption.arg(name);
                // Only long option passed
                else
                    options << QStringLiteral("    --%1").arg(name);

        auto joined = options.join(COMMA);
        auto indent = QString(optionsMaxSize - joined.size(), SPACE);

        m_command.get().info(QStringLiteral("  %1%2  ").arg(std::move(joined),
                                                            std::move(indent)),
                             false)
                .note(option.description());
    }
}

} // namespace Tom::Concerns

TINYORM_END_COMMON_NAMESPACE
