#include "orm/exceptions/sqlerror.hpp"

#include <QStringList>

#include "orm/constants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::COMMA;

namespace Orm::Exceptions
{

/* public */

SqlError::SqlError(const char *message, const TSqlError &error)
    : RuntimeError(formatMessage(message, error))
    , m_sqlError(error)
{}

SqlError::SqlError(const QString &message, const TSqlError &error)
    : SqlError(message.toUtf8().constData(), error)
{}

SqlError::SqlError(const char *message, TSqlError &&error)
    : RuntimeError(formatMessage(message, error))
    , m_sqlError(std::move(error))
{}

SqlError::SqlError(const QString &message, TSqlError &&error)
    : SqlError(message.toUtf8().constData(), std::move(error))
{}

/* protected */

// NOLINTNEXTLINE(modernize-pass-by-value)
SqlError::SqlError(const QString &message, const TSqlError &error, const int /*unused*/)
    : RuntimeError(message.toUtf8().constData())
    , m_sqlError(error)
{}

QString SqlError::formatMessage(const char *message, const TSqlError &error)
{
    const auto messageStr = QString::fromUtf8(message);
    auto nativeErrorCode = error.nativeErrorCode();
    auto driverText = error.driverText().trimmed();
    auto databaseText = error.databaseText().trimmed();

    QString result;
    // +32 as a reserve; +4 : 2 * ', '
    result.reserve(messageStr.size() + 11 +
                   nativeErrorCode.size() + driverText.size() + databaseText.size() + 1 +
                   4 + 32);

    result += messageStr;
    result += QStringLiteral("\nQSqlError(");

    QStringList errorText;
    errorText.reserve(3);

    if (!nativeErrorCode.isEmpty())
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        errorText << std::move(nativeErrorCode);
#else
        errorText << nativeErrorCode;
#endif

    if (!driverText.isEmpty())
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        errorText << std::move(driverText);
#else
        errorText << driverText;
#endif

    if (!databaseText.isEmpty())
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        errorText << std::move(databaseText);
#else
        errorText << databaseText;
#endif

    result += errorText.join(COMMA);

    result += QLatin1Char(')');

    return result;
}

} // namespace Orm::Exceptions

TINYORM_END_COMMON_NAMESPACE
