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

bool SqlResultPrivate::isVariantNull(const QVariant &value)
{
    if (value.isNull())
        return true;

    switch (value.typeId()) {
    case qMetaTypeId<QString>():
        return static_cast<const QString *>(value.constData())->isNull();

    case qMetaTypeId<QByteArray>():
        return static_cast<const QByteArray *>(value.constData())->isNull();

    case qMetaTypeId<QDateTime>():
        // CUR drivers check this if I send invalid date to the DB what happens, I think that problem will be in this and then update the comment appropriately, the same for the QTime few lines below; the isVariantNull() is used in prepared bindings only silverqx
        // We treat invalid date-time as null, since its ISODate would be empty.
        return !static_cast<const QDateTime *>(value.constData())->isValid();

    case qMetaTypeId<QDate>():
        return static_cast<const QDate *>(value.constData())->isNull();

    case qMetaTypeId<QTime>():
        // As for QDateTime, QTime can be invalid without being null.
        return !static_cast<const QTime *>(value.constData())->isValid();

    case qMetaTypeId<QUuid>():
        return static_cast<const QUuid *>(value.constData())->isNull();

    default:
        break;
    }

    return false;
}

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE
