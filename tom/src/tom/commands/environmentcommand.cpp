#include "tom/commands/environmentcommand.hpp"

#include "tom/application.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands
{

/* public */

EnvironmentCommand::EnvironmentCommand(Application &application,
                                       QCommandLineParser &parser)
    : Command(application, parser)
{}

int EnvironmentCommand::run()
{
    Command::run();

    info(QLatin1String("Current application environment: "), false);

    comment(application().environment());

    return EXIT_SUCCESS;
}

} // namespace Tom::Commands

TINYORM_END_COMMON_NAMESPACE
