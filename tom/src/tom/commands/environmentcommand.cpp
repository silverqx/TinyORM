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

    using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

    info(u"Current application environment: "_s, false);

    comment(application().environment());

    return EXIT_SUCCESS;
}

} // namespace Tom::Commands

TINYORM_END_COMMON_NAMESPACE
