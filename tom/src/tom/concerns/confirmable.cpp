#include "tom/concerns/confirmable.hpp"

#include "tom/application.hpp"
#include "tom/commands/command.hpp"
#include "tom/tomconstants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Tom::Constants::force;

namespace Tom::Concerns
{

/* public */

Confirmable::Confirmable(Command &command, const int /*unused*/)
    : m_command(command)
{}

bool Confirmable::confirmToProceed(const QString &warning,
                                   const std::function<bool()> &callback) const
{
    const auto callback_ = callback ? callback : defaultConfirmCallback();

    const auto shouldConfirm = std::invoke(callback_);

    // Should not confirm (not production) or the 'force' cmd. argument set
    if (!shouldConfirm)
        return true;

    if (const auto &parser = m_command.get().parser();
        parser.optionNames().contains(force) && parser.isSet(force)
    )
        return true;

    // Show the alert and confirm logic
    m_command.get().alert(warning);

    const auto confirmed =
            m_command.get().confirm(
                QStringLiteral("Do you really wish to run this command?"));

    if (confirmed)
        return true;

    m_command.get().comment(QStringLiteral("Command Canceled!"));

    return false;
}

/* protected */

std::function<bool()> Confirmable::defaultConfirmCallback() const
{
    return [this]
    {
        const auto &environment = m_command.get().application().environment();

        return environment == QLatin1String("production") ||
                environment == QLatin1String("prod");
    };
}

} // namespace Tom::Concerns

TINYORM_END_COMMON_NAMESPACE
