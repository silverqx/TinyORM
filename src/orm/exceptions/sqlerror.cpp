#include "orm/exceptions/sqlerror.hpp"

#include <QStringList>

#include "orm/constants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::COMMA;

namespace Orm::Exceptions
{

/* public */

SqlError::SqlError(const char *message, const QSqlError &error)
    : RuntimeError(formatMessage(message, error))
    , m_sqlError(error)
{}

SqlError::SqlError(const QString &message, const QSqlError &error)
    : SqlError(message.toUtf8().constData(), error)
{}

/* protected */

// NOLINTNEXTLINE(modernize-pass-by-value)
SqlError::SqlError(const QString &message, const QSqlError &error, const int /*unused*/)
    : RuntimeError(message.toUtf8().constData())
    , m_sqlError(error)
{}

QString SqlError::formatMessage(const char *message, const QSqlError &error) const
{
    QString result(message);

    result += QStringLiteral("\nQSqlError(");

    QStringList errorText;

    if (const auto nativeErrorCode = error.nativeErrorCode();
        !nativeErrorCode.isEmpty()
    )
        errorText << nativeErrorCode;

    if (const auto driverText = error.driverText().trimmed();
        !driverText.isEmpty()
    )
        errorText << driverText;

    if (const auto databaseText = error.databaseText().trimmed();
        !databaseText.isEmpty()
    )
        errorText << databaseText;

    result += errorText.join(COMMA);

    result += QLatin1Char(')');

    return result;
}

} // namespace Orm::Exceptions

TINYORM_END_COMMON_NAMESPACE
