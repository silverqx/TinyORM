#include "tom/concerns/confirmable.hpp"

#include "tom/application.hpp"
#include "tom/commands/command.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Tom::Constants::Prod;
using Tom::Constants::Production;
using Tom::Constants::force;

namespace Tom::Concerns
{

/* public */

bool Confirmable::confirmToProceed(const QString &warning,
                                   const std::function<bool()> &callback) const
{
    const auto callback_ = callback ? callback : defaultConfirmCallback();

    const auto shouldConfirm = std::invoke(callback_);

    // Should not confirm (not production) or the 'force' cmd. argument set
    if (!shouldConfirm)
        return true;

    if (const auto &parser = command().parser();
        parser.optionNames().contains(force) && parser.isSet(force)
    )
        return true;

    // Show the alert and confirm logic
    command().alert(warning);

    const auto confirmed = command().confirm(
                               u"Do you really wish to run this command?"_s);

    if (confirmed)
        return true;

    command().comment(u"Command Canceled!"_s);

    return false;
}

/* private */

std::function<bool()> Confirmable::defaultConfirmCallback() const
{
    return [this]
    {
        const auto &environment = command().application().environment();

        return environment == Production || environment == Prod;
    };
}

const Commands::Command &Confirmable::command() const
{
    return dynamic_cast<const Commands::Command &>(*this);
}

} // namespace Tom::Concerns

TINYORM_END_COMMON_NAMESPACE
