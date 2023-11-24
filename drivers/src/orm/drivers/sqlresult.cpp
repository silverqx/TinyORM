#include "orm/drivers/sqlresult.hpp"

#include "orm/drivers/sqldriver.hpp"
#include "orm/drivers/sqlresult_p.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using namespace Qt::StringLiterals;

namespace Orm::Drivers
{

/* protected */

SqlResult::SqlResult(std::unique_ptr<SqlResultPrivate> &&dd)
    : d_ptr(std::move(dd))
{
    Q_D(SqlResult);

    // CUR drivers why checked so late? silverqx
    // The d->sqldriver must always be set
    Q_ASSERT(d->sqldriver != nullptr);

    setNumericalPrecisionPolicy(d->sqldriver->defaultNumericalPrecisionPolicy());
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

    return d->cursor != QSql::BeforeFirstRow &&
           d->cursor != QSql::AfterLastRow;
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
    return d->active;
}

void SqlResult::setActive(const bool value)
{
    Q_D(SqlResult);
    d->active = value;
}

bool SqlResult::isSelect() const
{
    Q_D(const SqlResult);
    return d->select;
}

void SqlResult::setSelect(const bool value)
{
    Q_D(SqlResult);
    d->select = value;
}

QSql::NumericalPrecisionPolicy SqlResult::numericalPrecisionPolicy() const
{
    Q_D(const SqlResult);
    return d->precisionPolicy;
}

void
SqlResult::setNumericalPrecisionPolicy(const QSql::NumericalPrecisionPolicy precision)
{
    Q_D(SqlResult);
    d->precisionPolicy = precision;
}

const SqlDriver *SqlResult::driver() const noexcept
{
    Q_D(const SqlResult);
    return d->sqldriver;
}

/* Prepared queries */

void SqlResult::bindValue(const int index, const QVariant &value,
                          const QSql::ParamType /*unused*/)
{
    Q_D(SqlResult);

    /* Resize the underlying vector to assign a value to the given index
       (avoid out of bounds). */
    if (d->values.size() <= index)
        d->values.resize(index + 1);

    d->values[index] = value;
}

void SqlResult::addBindValue(const QVariant &value, const QSql::ParamType /*unused*/)
{
    // Append; index is after the last value
    const auto index = boundValuesCount();

    bindValue(index, value, QSql::In);
}

QVariant SqlResult::boundValue(const int index) const
{
    Q_D(const SqlResult);
    return d->values.value(index);
}

int SqlResult::boundValuesCount() const
{
    Q_D(const SqlResult);
    return d->values.size();
}

QVariantList SqlResult::boundValues() const
{
    Q_D(const SqlResult);
    return d->values;
}

QVariantList &SqlResult::boundValues()
{
    Q_D(SqlResult);
    return d->values;
}

void SqlResult::clearValues()
{
    Q_D(SqlResult);
    d->values.clear();
}

/* Result sets */

bool SqlResult::fetchPrevious()
{
    return fetch(at() - 1);
}

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE
