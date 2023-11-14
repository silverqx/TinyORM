#include "orm/drivers/sqldriver.hpp"

#include "orm/drivers/sqldriver_p.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

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

QSql::NumericalPrecisionPolicy SqlDriver::defaultNumericalPrecisionPolicy() const
{
    Q_D(const SqlDriver);
    return d->defaultPrecisionPolicy;
}

void SqlDriver::setDefaultNumericalPrecisionPolicy(
        const QSql::NumericalPrecisionPolicy precision)
{
    Q_D(SqlDriver);
    d->defaultPrecisionPolicy = precision;
}

int SqlDriver::maximumIdentifierLength(const SqlDriver::IdentifierType /*unused*/) const
{
    return std::numeric_limits<int>::max();
}

bool SqlDriver::isIdentifierEscaped(const QString &identifier,
                                    const IdentifierType /*unused*/) const
{
    return identifier.size() > 2 &&
           identifier.startsWith(QChar('"')) &&
           identifier.endsWith(QChar('"'));
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

void SqlDriver::setLastError(const SqlDriverError &error)
{
    Q_D(SqlDriver);
    d->lastError = error;
}

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE
