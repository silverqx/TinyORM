#include "orm/drivers/sqlresult_p.hpp"

#include <QDateTime>
#include <QUuid>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

/* public */

SqlResultPrivate::SqlResultPrivate(const std::weak_ptr<SqlDriver> &driver) noexcept
    : sqldriver(driver)
{
    /* The sqldriver must always be valid during construction, I'm leaving this check
       here even there is another check in the MySqlDriver::createResult() because
       the MySqlResult can still be instantiated manually. */
    Q_ASSERT(!sqldriver.expired());
}

/* This problematic is much more complex. Qt can represent much higher QDate ranges
   than the MySQL server and currently, TinyDrivers doesn't check these ranges that means
   we depend on error codes returned from the DB server.

   MySQL ranges:
    - DATE: '1000-01-01' to '9999-12-31'
    - TIME: '-838:59:59.000000' to '838:59:59.000000'

   Qt ranges, it depends whether Daylight-Saving Time (DST) is used, the ranges are huge
   without the DST and with DST they are around 1970-01-01 to 2037-12-31.

   The QDate is OK there is nothing wrong with it.

   But the QTime isn't good, Qt can't represent QTime outside of the following ranges:
   Hours 0-23, minutes/seconds 0-59, and 0-999 for milliseconds.

   The other thing is that invalid QDateTime and QTime has an empty ISODate so send it
   as the NULL value instead of an empty string.
   I don't like this but we need to be API compatible with QtSql module so I leave this
   logic untouched. */

bool SqlResultPrivate::isVariantNull(const QVariant &value)
{
    // This method is used in prepared bindings only

    if (value.isNull())
        return true;

    switch (value.typeId()) {
    case qMetaTypeId<QString>():
        return static_cast<const QString *>(value.constData())->isNull();

    case qMetaTypeId<QDateTime>():
        /* Invalid QDateTime has an empty ISODate so send it as the NULL value instead
           of an empty string. */
        return !static_cast<const QDateTime *>(value.constData())->isValid();

    case qMetaTypeId<QDate>():
        return static_cast<const QDate *>(value.constData())->isNull();

    case qMetaTypeId<QTime>():
        /* Invalid QTime has an empty ISODate so send it as the NULL value instead
           of an empty string. */
        return !static_cast<const QTime *>(value.constData())->isValid();

    case qMetaTypeId<QByteArray>():
        return static_cast<const QByteArray *>(value.constData())->isNull();

    case qMetaTypeId<QUuid>():
        return static_cast<const QUuid *>(value.constData())->isNull();

    default:
        break;
    }

    return false;
}

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE
