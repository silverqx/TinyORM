#include "orm/drivers/sqlresult.hpp"

#include <orm/constants.hpp>

#include "orm/drivers/sqldriver.hpp"
#include "orm/drivers/sqlresult_p.hpp"

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

bool SqlResult::isValid() const
{
    Q_D(const SqlResult);

    return d->cursor != static_cast<int>(BeforeFirstRow) &&
           d->cursor != static_cast<int>(AfterLastRow);
}

QString SqlResult::query() const
{
    Q_D(const SqlResult);
    return d->query;
}

void SqlResult::setQuery(const QString &query)
{
    Q_D(SqlResult);
    d->query = query;
}

SqlDriverError SqlResult::lastError() const
{
    Q_D(const SqlResult);
    return d->lastError;
}

bool SqlResult::setLastError(const SqlDriverError &error)
{
    Q_D(SqlResult);

    d->lastError = error;

    // To be able call 'return setLastError()' to simplify code
    return false;
}

int SqlResult::at() const
{
    Q_D(const SqlResult);
    return d->cursor;
}

void SqlResult::setAt(const int index)
{
    Q_D(SqlResult);
    d->cursor = index;
}

bool SqlResult::isActive() const
{
    Q_D(const SqlResult);
    return d->isActive;
}

void SqlResult::setActive(const bool value)
{
    Q_D(SqlResult);
    d->isActive = value;
}

bool SqlResult::isSelect() const
{
    Q_D(const SqlResult);
    return d->isSelect;
}

void SqlResult::setSelect(const bool value)
{
    Q_D(SqlResult);
    d->isSelect = value;
}

NumericalPrecisionPolicy SqlResult::numericalPrecisionPolicy() const
{
    Q_D(const SqlResult);
    return d->precisionPolicy;
}

void
SqlResult::setNumericalPrecisionPolicy(const NumericalPrecisionPolicy precision)
{
    Q_D(SqlResult);
    d->precisionPolicy = precision;
}

std::weak_ptr<const SqlDriver> SqlResult::driver() const noexcept
{
    Q_D(const SqlResult);
    return d->sqldriver;
}

/* Prepared queries */

void SqlResult::bindValue(const int index, const QVariant &value,
                          const ParamType /*unused*/)
{
    Q_D(SqlResult);

    /* Resize the underlying vector to assign a value to the given index
       (avoid out of bounds). */
    if (d->boundValues.size() <= index)
        d->boundValues.resize(index + 1);

    d->boundValues[index] = value;
}

void SqlResult::addBindValue(const QVariant &value, const ParamType /*unused*/)
{
    // Append; index is after the last value
    const auto index = boundValuesCount();

    bindValue(index, value, ParamType::In);
}

QVariant SqlResult::boundValue(const int index) const
{
    Q_D(const SqlResult);
    return d->boundValues.value(index);
}

int SqlResult::boundValuesCount() const
{
    Q_D(const SqlResult);
    return d->boundValues.size();
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
    throw std::runtime_error(Orm::Constants::NotImplemented.toUtf8().constData());
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
