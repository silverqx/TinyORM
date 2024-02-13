#include "orm/drivers/exceptions/sqlerror.hpp"

#ifdef TINYDRIVERS_MYSQL_DRIVER
#  include <QStringList>

#  include "orm/drivers/constants_p.hpp"
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

#ifdef TINYDRIVERS_MYSQL_DRIVER
using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

using Orm::Drivers::Constants::COMMA;
#endif

namespace Orm::Drivers::Exceptions
{

/* public */

#ifdef TINYDRIVERS_MYSQL_DRIVER
SqlError::SqlError(const char *message, MySqlErrorType &&mysqlError)
    : RuntimeError(formatMessage(message, mysqlError))
    , m_errorCode(std::move(mysqlError.errNo))
    , m_databaseText(std::move(mysqlError.errorMessage))
{}

SqlError::SqlError(const QString &message, MySqlErrorType &&mysqlError)
    : SqlError(message.toUtf8().constData(), std::move(mysqlError))
{}
#endif // TINYDRIVERS_MYSQL_DRIVER

/* protected */

#ifdef TINYDRIVERS_MYSQL_DRIVER
SqlError::SqlError(const QString &message, MySqlErrorType &&mysqlError,
                   const int /*unused*/)
    : RuntimeError(message.toUtf8().constData())
    , m_errorCode(std::move(mysqlError.errNo))
    , m_databaseText(std::move(mysqlError.errorMessage))
{}

QString
SqlError::formatMessage(const char *const message, const MySqlErrorType &mysqlError)
{
    const auto messageStr = QString::fromUtf8(message);
    const auto &[errorCode, databaseText] = mysqlError;

    QString result;
    // +32 as a reserve; +2 : 1 * ', '
    result.reserve(messageStr.size() + 7 + errorCode.size() + databaseText.size() +
                   1 + 2 + 32);

    result += messageStr;
    result += u"\nMySQL("_s;

    QStringList errorText;
    errorText.reserve(2);

    if (!errorCode.isEmpty())
        errorText << errorCode;

    if (!databaseText.isEmpty())
        errorText << databaseText;

    result += errorText.join(COMMA);

    result += ')'_L1;

    return result;
}
#endif // TINYDRIVERS_MYSQL_DRIVER

} // namespace Orm::Drivers::Exceptions

TINYORM_END_COMMON_NAMESPACE
