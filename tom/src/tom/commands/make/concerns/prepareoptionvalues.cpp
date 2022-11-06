#include "tom/commands/make/concerns/prepareoptionvalues.hpp"

#include <QCommandLineParser>

#include "tom/commands/make/modelcommand.hpp"
#include "tom/commands/make/support/preparebtmoptionvalues.hpp"
#include "tom/commands/make/support/prepareforeignkeyvalues.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Tom::Commands::Make::Support::PrepareBtmOptionValues;
using Tom::Commands::Make::Support::PrepareForeignKeyValues;
using Tom::Constants::foreign_key;

namespace Tom::Commands::Make::Concerns
{

/* protected */

ForeignKeys PrepareOptionValues::foreignKeyValues()
{
    return PrepareForeignKeyValues(modelCommand(), modelCommand().values(foreign_key))
            .prepareValues();
}

QStringList PrepareOptionValues::btmValues(const QString &optionName)
{
    return PrepareBtmOptionValues(modelCommand(),
                                  modelCommand().values(optionName))
            .prepareValuesFor(optionName);
}

std::vector<QStringList> PrepareOptionValues::btmMultiValues(const QString &optionName)
{
    return PrepareBtmOptionValues<
            std::vector<QStringList>>(modelCommand(),
                                      modelCommand().values(optionName))
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

ModelCommand &PrepareOptionValues::modelCommand() noexcept
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
    return static_cast<ModelCommand &>(*this);
}

} // namespace Tom::Commands::Make::Concerns

TINYORM_END_COMMON_NAMESPACE
