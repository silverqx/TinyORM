#include "tom/commands/make/support/tableguesser.hpp"

#include <QRegularExpression>

TINYORM_BEGIN_COMMON_NAMESPACE

using Qt::StringLiterals::operator""_s;

namespace Tom::Commands::Make::Support
{

/* private */

const QString
TableGuesser::CreatePatterns = uR"(^create(?:_|-)(\w+?)(?:(?:_|-)table)?$)"_s;
const QString
TableGuesser::ChangePatterns =
        uR"((?:_|-)(?:to|from|in)(?:_|-)(\w+?)(?:(?:_|-)table)?$)"_s;

/* public */

std::tuple<QString, bool> TableGuesser::guess(const QString &migration)
{
    {
        /* It's OK to have the same variable names for the local static until they are
           in different block scopes. */
        static const QRegularExpression RegEx(CreatePatterns);

        const auto match = RegEx.match(migration);

        if (match.hasMatch()) {
            Q_ASSERT(RegEx.captureCount() == 1);

            return {match.captured(1), true};
        }
    }

    {
        static const QRegularExpression RegEx(ChangePatterns);

        const auto match = RegEx.match(migration);

        if (match.hasMatch()) {
            Q_ASSERT(RegEx.captureCount() == 1);

            return {match.captured(1), false};
        }
    }

    return {};
}

} // namespace Tom::Commands::Make::Support

TINYORM_END_COMMON_NAMESPACE
