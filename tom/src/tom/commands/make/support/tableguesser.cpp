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
        static const QRegularExpression regex(CreatePatterns);

        const auto match = regex.match(migration);

        if (match.hasMatch()) {
            Q_ASSERT(regex.captureCount() == 1);

            return {match.captured(1), true};
        }
    }

    {
        static const QRegularExpression regex(ChangePatterns);

        const auto match = regex.match(migration);

        if (match.hasMatch()) {
            Q_ASSERT(regex.captureCount() == 1);

            return {match.captured(1), false};
        }
    }

    return {};
}

} // namespace Tom::Commands::Make::Support

TINYORM_END_COMMON_NAMESPACE
