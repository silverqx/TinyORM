#include "tom/tableguesser.hpp"

#include <QRegularExpression>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
{

/* private */

const QString
TableGuesser::CreatePatterns = QStringLiteral(R"(^create_(\w+?)(?:_table)?$)");
const QString
TableGuesser::ChangePatterns = QStringLiteral(R"(_(?:to|from|in)_(\w+?)(?:_table)?$)");

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
        // CUR tom, const QRegularExpression everywhere silverqx
        static const QRegularExpression regex(ChangePatterns);

        const auto match = regex.match(migration);

        if (match.hasMatch()) {
            Q_ASSERT(regex.captureCount() == 1);

            return {match.captured(1), false};
        }
    }

    return {};
}

} // namespace Tom

TINYORM_END_COMMON_NAMESPACE
