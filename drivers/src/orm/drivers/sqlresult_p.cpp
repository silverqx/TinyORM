#include "orm/drivers/sqlresult_p.hpp"

#include <QDateTime>
#include <QUuid>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

/* public */

SqlResultPrivate::SqlResultPrivate(const std::weak_ptr<SqlDriver> &driver)
    : sqldriver(driver)
{
    // The sqldriver must always be valid during construction
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
