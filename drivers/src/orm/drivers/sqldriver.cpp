#include "orm/drivers/sqldriver.hpp"

#include <orm/constants.hpp>

#include "orm/drivers/sqldriver_p.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::QUOTE;

namespace Orm::Drivers
{

/* protected */

SqlDriver::SqlDriver(std::unique_ptr<SqlDriverPrivate> &&dd)
    : d_ptr(std::move(dd))
{}

/* public */

/* The destructor must be in the cpp file because the d_ptr is unique_ptr.
   If the destructor is inline then the compilation fails because a unique_ptr can't
   destroy an incomplete type. */
SqlDriver::~SqlDriver() = default;

/* Getters / Setters */

bool SqlDriver::isOpen() const
{
    Q_D(const SqlDriver);
    return d->isOpen;
}

bool SqlDriver::isOpenError() const
{
    Q_D(const SqlDriver);
    return d->isOpenError;
}

SqlDriver::DbmsType SqlDriver::dbmsType() const
{
    Q_D(const SqlDriver);
    return d->dbmsType;
}

SqlDriverError SqlDriver::lastError() const
{
    Q_D(const SqlDriver);
    return d->lastError;
}

NumericalPrecisionPolicy SqlDriver::defaultNumericalPrecisionPolicy() const
{
    Q_D(const SqlDriver);
    return d->defaultPrecisionPolicy;
}

void
SqlDriver::setDefaultNumericalPrecisionPolicy(const NumericalPrecisionPolicy precision)
{
    Q_D(SqlDriver);
    d->defaultPrecisionPolicy = precision;
}

std::thread::id SqlDriver::threadId() const noexcept
{
    Q_D(const SqlDriver);
    return d->threadId;
}

/* Others */

int SqlDriver::maximumIdentifierLength(const SqlDriver::IdentifierType /*unused*/) const
{
    return std::numeric_limits<int>::max();
}

/* The following two methods are named wrong, but I can't rename them because
   they are public and virtual. 🤔 */
bool SqlDriver::isIdentifierEscaped(const QString &identifier,
                                    const IdentifierType /*unused*/) const
{
    return identifier.size() > 2 &&
           identifier.startsWith(QUOTE) &&
           identifier.endsWith(QUOTE);
}

QString
SqlDriver::stripDelimiters(const QString &identifier, const IdentifierType type) const
{
    // Nothing to do
    if (!isIdentifierEscaped(identifier, type))
        return identifier;

    return identifier.sliced(1).chopped(1);
}

/* protected */

/* Setters */

void SqlDriver::setOpen(const bool value)
{
    Q_D(SqlDriver);
    d->isOpen = value;
}

void SqlDriver::setOpenError(const bool value)
{
    Q_D(SqlDriver);

    d->isOpenError = value;

    if (value)
        d->isOpen = false;
}

bool SqlDriver::setLastError(const SqlDriverError &error)
{
    Q_D(SqlDriver);

    d->lastError = error;

    // To be able call 'return setLastError()' to simplify code
    return false;
}

bool SqlDriver::setLastOpenError(const SqlDriverError &error)
{
    Q_D(SqlDriver);

    d->lastError = error;
    d->isOpenError = true;

    // To be able call 'return setLastError()' to simplify code
    return false;
}

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE
