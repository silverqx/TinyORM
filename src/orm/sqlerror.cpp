#include "orm/sqlerror.hpp"

#include <QStringList>

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

SqlError::SqlError(const char *message, const QSqlError &error)
    : RuntimeError(formatMessage(message, error))
    , m_sqlError(error)
{}

SqlError::SqlError(const QString &message, const QSqlError &error)
    : SqlError(message.toUtf8().constData(), error)
{}

SqlError::SqlError(const QString &message, const QSqlError &error, const int)
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

    if (const auto driverText = error.driverText();
        !driverText.isEmpty()
    )
        errorText << driverText;

    if (const auto databaseText = error.databaseText();
        !databaseText.isEmpty()
    )
        errorText << databaseText;

    result += errorText.join(QStringLiteral(", "));

    result += QChar(')');

    return result;
}

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
