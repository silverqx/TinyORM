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

    /* Parse needed also here because InteractiveIO constructor calls isSet().
       Also, no error handling needed here, it will be handled in the Command::run(),
       this is only pre-parse because of isSet() in the InteractiveIO constructor. */
    auto currentArguments = application.arguments();
    parser.parse(currentArguments);

    return application.createCommand(command, parser)
            ->runWithArguments(
                createCommandLineArguments(command, std::move(arguments),
                                           std::move(currentArguments)));
}

QStringList
CallsCommands::createCommandLineArguments(
        const QString &command, QStringList &&arguments, QStringList &&currentArguments) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
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
    currentArguments.remove(0, 2);

    // Get common allowed command-line arguments from the current command-line arguments
    newArguments << getCommonArguments(currentArguments);

    // Append passed arguments
    std::ranges::move(ranges::actions::remove_if(std::move(arguments),
                                                 [](auto &&v) { return v.isEmpty(); }),
                      std::back_inserter(newArguments));

    return newArguments;
}

QStringList CallsCommands::getCommonArguments(const QStringList &arguments)
{
    // This way I'm able to re-use global constants
    /*! Create a long command-line option from the option name (--xyz). */
    const auto o = [](const auto &optionName)
    {
        return LongOption.arg(optionName);
    };

    using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

    static const std::unordered_set<QString> allowed {
        o(ansi),
        o(noansi),
        o(nointeraction), u"-n"_s,
        o(quiet),         u"-q"_s,
        o(verbose),       u"-v"_s, u"-vv"_s,
                          u"-vvv"_s,
    };

    return arguments
            | ranges::views::filter([&allowed = allowed](const QString &argument)
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
