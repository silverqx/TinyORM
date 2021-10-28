#include "orm/exceptions/sqlerror.hpp"

#include <QStringList>

#include "orm/constants.hpp"

using Orm::Constants::COMMA;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Exceptions
{

SqlError::SqlError(const char *message, const QSqlError &error)
    : RuntimeError(formatMessage(message, error))
    , m_sqlError(error)
{}

SqlError::SqlError(const QString &message, const QSqlError &error)
    : SqlError(message.toUtf8().constData(), error)
{}

// NOLINTNEXTLINE(modernize-pass-by-value)
SqlError::SqlError(const QString &message, const QSqlError &error, const int /*unused*/)
    : RuntimeError(message.toUtf8().constData())
    , m_sqlError(error)
{}

const QSqlError &SqlError::getSqlError() const
{
    return m_sqlError;
}

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

    result += QChar(')');

    return result;
}

} // namespace Orm::Exceptions

TINYORM_END_COMMON_NAMESPACE
