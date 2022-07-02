#include "tom/commands/make/concerns/prepareoptionvalues.hpp"

#include <QCommandLineParser>

#include "tom/commands/make/modelcommand.hpp"
#include "tom/commands/make/support/preparebtmoptionvalues.hpp"

using Tom::Commands::Make::Support::PrepareBtmOptionValues;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make::Concerns
{

/* protected */

QStringList PrepareOptionValues::btmValues(const QString &optionName)
{
    return PrepareBtmOptionValues(modelCommand(),
                                  modelCommand().parser().values(optionName))
            .prepareValuesFor(optionName);
}

std::vector<QStringList> PrepareOptionValues::btmMultiValues(const QString &optionName)
{
    return PrepareBtmOptionValues<
            std::vector<QStringList>>(modelCommand(),
                                      modelCommand().parser().values(optionName))
            .prepareValuesFor(optionName);
}

std::vector<bool> PrepareOptionValues::btmBoolValues(const QString &optionName)
{
    return PrepareBtmOptionValues<
            std::vector<bool>, std::vector<bool>>(modelCommand(),
                                                  modelCommand().isSetAll(optionName))
            .prepareValuesFor(optionName);
}

/* private */

ModelCommand &PrepareOptionValues::modelCommand()
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
    return static_cast<ModelCommand &>(*this);
}

} // namespace Tom::Commands::Make::Concerns

TINYORM_END_COMMON_NAMESPACE
