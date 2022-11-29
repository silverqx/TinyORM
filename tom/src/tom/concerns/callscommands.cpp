#include "tom/concerns/callscommands.hpp"

#include <unordered_set>

#include <range/v3/action/remove_if.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/move.hpp>

#include "tom/application.hpp"
#include "tom/commands/command.hpp"
#include "tom/tomconstants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Tom::Constants::ansi;
using Tom::Constants::noansi;
using Tom::Constants::nointeraction;
using Tom::Constants::quiet;
using Tom::Constants::verbose;
using Tom::Constants::LongOption;

namespace Tom::Concerns
{

/* protected */

int CallsCommands::runCommand(const QString &command, QStringList &&arguments) const
{
    QCommandLineParser parser;

    auto &application = this->command().application();

    application.initializeParser(parser);

    /* Parse needed also here because InteractiveIO ctor calls isSet(). Also no error
       handling needed here, it will be handled in the Command::run(), this is only
       pre-parse because of isSet() in the InteractiveIO ctor. */
    auto currentArguments = application.arguments();
    parser.parse(currentArguments);

    return application.createCommand(command, parser)
            ->runWithArguments(
                createCommandLineArguments(command, std::move(arguments),
                                           std::move(currentArguments)));
}

QStringList
CallsCommands::createCommandLineArguments(
            const QString &command, QStringList &&arguments,
            QStringList &&currentArguments) const
{
    // Must contain a command exe name and tom's command name
    Q_ASSERT(currentArguments.size() >= 2);

    /* First create a new arguments list that starts with an executable name followed
       by a command name to execute, then obtain common arguments which were passed
       on the current command-line, and as the last thing append passed arguments. */

    QStringList newArguments;
    newArguments.reserve(currentArguments.size() + arguments.size());

    // Absolute path of the exe name
    newArguments << std::move(currentArguments.first());
    // Command name
    newArguments << command;

    // Remove a command exe name and tom's command name
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    currentArguments.remove(0, 2);
#else
    currentArguments.removeFirst();
    currentArguments.removeFirst();
#endif

    // Get common allowed command-line arguments from the current command-line arguments
    newArguments << getCommonArguments(std::move(currentArguments));

    // Append passed arguments
    std::ranges::move(ranges::actions::remove_if(std::move(arguments),
                                                 [](auto &&v) { return v.isEmpty(); }),
                      std::back_inserter(newArguments));

    return newArguments;
}

QStringList CallsCommands::getCommonArguments(QStringList &&arguments) const
{
    // This way I'm able to re-use global constants
    /*! Create a long command-line option from the option name (--xyz). */
    const auto o = [](const auto &optionName)
    {
        return LongOption.arg(optionName);
    };

    static const std::unordered_set<QString> allowed {
        o(ansi),
        o(noansi),
        o(nointeraction), QLatin1String("-n"),
        o(quiet),         QLatin1String("-q"),
        o(verbose),       QLatin1String("-v"), QLatin1String("-vv"),
                          QLatin1String("-vvv"),
    };

    return ranges::views::move(arguments)
            | ranges::views::filter([&allowed = allowed](auto &&argument)
    {
        return allowed.contains(argument);
    })
            | ranges::to<QStringList>();
}

/* private */

const Commands::Command &CallsCommands::command() const noexcept
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
    return static_cast<const Commands::Command &>(*this);
}

} // namespace Tom::Concerns

TINYORM_END_COMMON_NAMESPACE
