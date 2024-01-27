#include "orm/drivers/sqlresult.hpp"

#include "orm/drivers/constants_p.hpp"
#include "orm/drivers/exceptions/runtimeerror.hpp"
#include "orm/drivers/sqldriver.hpp"
#include "orm/drivers/sqlresult_p.hpp"

using Orm::Drivers::Constants::NotImplemented;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

/* protected */

SqlResult::SqlResult(std::unique_ptr<SqlResultPrivate> &&dd)
    : d_ptr(std::move(dd))
{
    Q_D(SqlResult);

    setNumericalPrecisionPolicy(
                d->sqldriver.lock()->defaultNumericalPrecisionPolicy());
}

/* public */

/* The destructor must be in the cpp file because the d_ptr is unique_ptr.
   If the destructor is inline then the compilation fails because a unique_ptr can't
   destroy an incomplete type. */
SqlResult::~SqlResult() = default;

/* protected */

/* Getters / Setters */

bool SqlResult::isValid() const noexcept
{
    Q_D(const SqlResult);

    return d->cursor != static_cast<size_type>(BeforeFirstRow) &&
           d->cursor != static_cast<size_type>(AfterLastRow);
}

QString SqlResult::query() const noexcept
{
    Q_D(const SqlResult);
    return d->query;
}

void SqlResult::setQuery(const QString &query) noexcept
{
    Q_D(SqlResult);
    d->query = query;
}

SqlError SqlResult::lastError() const noexcept
{
    Q_D(const SqlResult);
    return d->lastError;
}

bool SqlResult::setLastError(const SqlError &error) noexcept
{
    Q_D(SqlResult);

    d->lastError = error;

    // To be able call 'return setLastError()' to simplify code
    return false;
}

bool SqlResult::setLastError(SqlError &&error) noexcept
{
    Q_D(SqlResult);

    d->lastError = std::move(error);

    // To be able call 'return setLastError()' to simplify code
    return false;
}

void SqlResult::resetLastError() noexcept
{
    Q_D(SqlResult);

    d->lastError = SqlError::NoError;
}

SqlResult::size_type SqlResult::at() const noexcept
{
    Q_D(const SqlResult);
    return d->cursor;
}

void SqlResult::setAt(const size_type index) noexcept
{
    Q_D(SqlResult);
    d->cursor = index;
}

bool SqlResult::isActive() const noexcept
{
    Q_D(const SqlResult);
    return d->isActive;
}

void SqlResult::setActive(const bool value) noexcept
{
    Q_D(SqlResult);
    d->isActive = value;
}

bool SqlResult::isSelect() const noexcept
{
    Q_D(const SqlResult);
    return d->isSelect;
}

void SqlResult::setSelect(const bool value) noexcept
{
    Q_D(SqlResult);
    d->isSelect = value;
}

NumericalPrecisionPolicy SqlResult::numericalPrecisionPolicy() const noexcept
{
    Q_D(const SqlResult);
    return d->precisionPolicy;
}

void
SqlResult::setNumericalPrecisionPolicy(const NumericalPrecisionPolicy precision) noexcept
{
    Q_D(SqlResult);
    d->precisionPolicy = precision;
}

/* See the note near the DatabaseManager/SqlQuery::driver() method about
   driver() vs driverWeak(). */

const SqlDriver *SqlResult::driver() const noexcept
{
    Q_D(const SqlResult);
    return d->sqldriver.lock().get();
}

std::weak_ptr<const SqlDriver> SqlResult::driverWeak() const noexcept
{
    Q_D(const SqlResult);
    return d->sqldriver;
}

/* Prepared queries */

void SqlResult::bindValue(const size_type index, const QVariant &value,
                          const ParamType /*unused*/)
{
    Q_D(SqlResult);

    /* Resize the underlying vector to assign a value to the given index
       (avoid out of bounds). */
    if (d->boundValues.size() <= index)
        d->boundValues.resize(index + 1);

    d->boundValues[index] = value;
}

void SqlResult::bindValue(const size_type index, QVariant &&value,
                          const ParamType /*unused*/)
{
    Q_D(SqlResult);

    /* Resize the underlying vector to assign a value to the given index
       (avoid out of bounds). */
    if (d->boundValues.size() <= index)
        d->boundValues.resize(index + 1);

    d->boundValues[index] = std::move(value);
}

void SqlResult::addBindValue(const QVariant &value, const ParamType /*unused*/)
{
    // Append; index is after the last value
    const auto index = boundValuesCount();

    bindValue(index, value, ParamType::In);
}

void SqlResult::addBindValue(QVariant &&value, const ParamType /*unused*/)
{
    // Append; index is after the last value
    const auto index = boundValuesCount();

    bindValue(index, std::move(value), ParamType::In);
}

QVariant SqlResult::boundValue(const size_type index) const
{
    Q_D(const SqlResult);
    return d->boundValues.value(index);
}

SqlResult::size_type SqlResult::boundValuesCount() const
{
    Q_D(const SqlResult);
    return static_cast<size_type>(d->boundValues.size());
}

QVariantList SqlResult::boundValues() const
{
    Q_D(const SqlResult);
    return d->boundValues;
}

QVariantList &SqlResult::boundValues()
{
    Q_D(SqlResult);
    return d->boundValues;
}

void SqlResult::clearBoundValues()
{
    Q_D(SqlResult);
    d->boundValues.clear();
}

/* Result sets */

bool SqlResult::fetchPrevious()
{
    return fetch(at() - 1);
}

bool SqlResult::nextResult()
{
    throw Exceptions::RuntimeError(NotImplemented);
}

/* private */

/* Leave the non-const driver() private, it's correct, it's not needed anywhere and is
   for special cases only. */

std::weak_ptr<SqlDriver> SqlResult::driver() noexcept
{
    Q_D(const SqlResult);
    return d->sqldriver;
}

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE
