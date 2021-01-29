#include "orm/sqlerror.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

SqlError::SqlError(const char *message, const QSqlError &error)
    : OrmRuntimeError(formatMessage(message, error))
    , m_sqlError(error)
{}

SqlError::SqlError(const QString &message, const QSqlError &error)
    : SqlError(message.toUtf8().constData(), error)
{}

SqlError::SqlError(const QString &message, const QSqlError &error, const int)
    : OrmRuntimeError(message.toUtf8().constData())
    , m_sqlError(error)
{}

const QSqlError &SqlError::getSqlError() const
{
    return m_sqlError;
}

QString SqlError::formatMessage(const char *message, const QSqlError &error) const
{
    QString result(message);

    result += QStringLiteral("\nQSqlError(") + error.nativeErrorCode();

    if (const auto driverText = error.driverText();
        !driverText.isEmpty())
        result += QStringLiteral(", ") + driverText;

    if (const auto databaseText = error.databaseText();
        !databaseText.isEmpty())
        result += QStringLiteral(", ") + databaseText;

    result += QChar(')');

    return result;
}

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
