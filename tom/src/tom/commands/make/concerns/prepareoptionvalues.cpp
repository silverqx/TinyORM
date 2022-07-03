#include "tom/commands/make/concerns/prepareoptionvalues.hpp"

#include <QCommandLineParser>

#include "tom/commands/make/modelcommand.hpp"
#include "tom/commands/make/support/preparebtmoptionvalues.hpp"
#include "tom/commands/make/support/prepareforeignkeyvalues.hpp"

using Tom::Commands::Make::Support::PrepareBtmOptionValues;
using Tom::Commands::Make::Support::PrepareForeignKeyValues;
using Tom::Constants::foreign_key;

TINYORM_BEGIN_COMMON_NAMESPACE

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
                                  modelCommand().parser().values(optionName))
            .prepareValuesFor(optionName);
}

std::vector<QStringList> PrepareOptionValues::btmMultiValues(const QString &optionName)
{
    return PrepareBtmOptionValues<
            std::vector<QStringList>>(modelCommand(),
                                      // CUR make model, test values() vs parser().values() silverqx
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
