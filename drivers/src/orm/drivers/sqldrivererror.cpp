#include "orm/drivers/sqldrivererror.hpp"

#ifndef QT_NO_DEBUG_STREAM
#  include <QDebug>
#endif

#include <orm/constants.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::NEWLINE;
using Orm::Constants::SPACE;

namespace Orm::Drivers
{

/* public */

// CUR drivers errorCode to quint64? silverqx
SqlDriverError::SqlDriverError(const QString &driverText, const QString &databaseText,
                               const ErrorType errorType, const QString &errorCode)
    : m_driverText(driverText)
    , m_databaseText(databaseText)
    , m_errorType(errorType)
    , m_errorCode(errorCode)
{}

bool SqlDriverError::operator==(const SqlDriverError &other) const
{
    return m_errorType == other.m_errorType &&
           m_errorCode == other.m_errorCode;
}

QString SqlDriverError::text() const
{
    auto result = m_databaseText;

    if (!m_driverText.isEmpty() && !m_databaseText.isEmpty() &&
        !m_databaseText.endsWith(NEWLINE)
    )
        result += SPACE;

    result += m_driverText;

    return result;
}

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#ifndef QT_NO_DEBUG_STREAM
#  ifdef TINYORM_COMMON_NAMESPACE
SHAREDLIB_EXPORT QDebug operator<<(QDebug debug, const TINYORM_COMMON_NAMESPACE::Orm::Drivers::SqlDriverError &error)
#  else
SHAREDLIB_EXPORT QDebug operator<<(QDebug debug, const Orm::Drivers::SqlDriverError &error)
#  endif
{
    QDebugStateSaver saver(debug);
    debug.nospace();

    debug << QStringLiteral("SqlDriverError(") << error.nativeErrorCode() << ", "
          << error.driverText() << ", "
          << error.databaseText() << ')';

    return debug;
}
#endif
