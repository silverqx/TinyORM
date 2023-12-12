#include "orm/drivers/sqldrivererror.hpp"

#ifndef QT_NO_DEBUG_STREAM
#  include <QDebug>
#endif

#include "orm/drivers/constants_p.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Drivers::Constants::NEWLINE;
using Orm::Drivers::Constants::SPACE;

namespace Orm::Drivers
{

/* public */

SqlDriverError::SqlDriverError(const QString &driverText, const QString &databaseText,
                               const ErrorType errorType, const QString &errorCode)
    : m_driverText(driverText)
    , m_databaseText(databaseText)
    , m_errorType(errorType)
    /* Can't be an integer because eg. the PostgreSQL error code can be something like
       this 25P02, so it can also contain letters. */
    , m_errorCode(errorCode)
{}

bool SqlDriverError::operator==(const SqlDriverError &other) const
{
    return m_errorType == other.m_errorType &&
           m_errorCode == other.m_errorCode;
}

void SqlDriverError::swap(SqlDriverError &other) noexcept
{
    m_driverText.swap(other.m_driverText);
    m_databaseText.swap(other.m_databaseText);
    std::swap(m_errorType, other.m_errorType);
    m_errorCode.swap(other.m_errorCode);
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
QDebug operator<<(QDebug debug,
                  const TINYORM_PREPEND_NAMESPACE(Orm::Drivers::SqlDriverError) &error)
{
    QDebugStateSaver saver(debug);
    debug.noquote().nospace();

    debug << "SqlDriverError("
          << error.nativeErrorCode() << ", "
          << error.driverText()      << ", "
          << error.databaseText()    << ')';

    return debug;
}
#endif
