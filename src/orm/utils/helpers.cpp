#include "orm/utils/helpers.hpp"

#include "orm/db.hpp"
#include "orm/macros/likely.hpp"
#include "orm/utils/string.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using StringUtils = Orm::Utils::String;

namespace Orm::Utils
{

/* public */

int Helpers::qVariantTypeId(const QVariant &value)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return value.typeId();
#else
    return value.userType();
#endif
}

/* QDateTime related */

bool Helpers::isStandardDateFormat(const QString &value)
{
    // Avoid RegEx for performance reasons
    const auto splitted = value.split(DASH, Qt::KeepEmptyParts);

    if (splitted.size() != 3)
        return false;

    // Year
    if (const auto &year = splitted.at(0);
        year.size() != 4 || !StringUtils::isNumber(year)
    )
        return false;

    // Month
    const auto &month = splitted.at(1);
    const auto monthSize = month.size();
    if ((monthSize != 1 && monthSize != 2) || !StringUtils::isNumber(month))
        return false;

    // Day
    const auto &day = splitted.at(2);
    const auto daySize = day.size();
    return (daySize == 1 || daySize == 2) && StringUtils::isNumber(day);
}

QDateTime
Helpers::convertTimeZone(const QDateTime &datetime, const QtTimeZoneConfig &timezone)
{
    const auto &[timezoneType, timezoneValue] = timezone;

    switch (timezoneType) {
    T_LIKELY
    case QtTimeZoneType::QtTimeSpec:
        return datetime.toTimeSpec(timezoneValue.value<Qt::TimeSpec>());

    T_LIKELY
    case QtTimeZoneType::QTimeZone:
        return datetime.toTimeZone(timezoneValue.value<QTimeZone>());

    T_UNLIKELY
    case QtTimeZoneType::OffsetFromUtc:
        return datetime.toOffsetFromUtc(timezoneValue.value<int>());

    T_UNLIKELY
    default:
        Q_UNREACHABLE();
    }
}

QDateTime &
Helpers::setTimeZone(QDateTime &datetime, const QtTimeZoneConfig &timezone)
{
    const auto &[timezoneType, timezoneValue] = timezone;

    switch (timezoneType) {
    T_LIKELY
    case QtTimeZoneType::QtTimeSpec:
        datetime.setTimeSpec(timezoneValue.value<Qt::TimeSpec>());
        break;

    T_LIKELY
    case QtTimeZoneType::QTimeZone:
        datetime.setTimeZone(timezoneValue.value<QTimeZone>());
        break;

    T_UNLIKELY
    case QtTimeZoneType::OffsetFromUtc:
        datetime.setOffsetFromUtc(timezoneValue.value<int>());
        break;

    T_UNLIKELY
    default:
        Q_UNREACHABLE();
    }

    return datetime;
}

QDateTime
Helpers::setTimeZone(QDateTime &&datetime, const QtTimeZoneConfig &timezone)
{
    return setTimeZone(datetime, timezone);
}

QDateTime
Helpers::convertTimeZone(const QDateTime &datetime, const QString &connection)
{
    return convertTimeZone(datetime, DB::qtTimeZone(connection));
}

QDateTime &
Helpers::setTimeZone(QDateTime &datetime, const QString &connection)
{
    return setTimeZone(datetime, DB::qtTimeZone(connection));
}

QDateTime Helpers::setTimeZone(QDateTime &&datetime, const QString &connection)
{
    return setTimeZone(datetime, connection);
}

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE
