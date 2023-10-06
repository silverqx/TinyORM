#include "orm/utils/helpers.hpp"

#include <QDebug>
#include <QLibraryInfo>

#include "orm/db.hpp"
#include "orm/macros/likely.hpp"
#include "orm/utils/string.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using StringUtils = Orm::Utils::String;
using TypeUtils = Orm::Utils::Type;

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

void Helpers::logException(const std::exception &e, const bool fatal)
{
    const QString exceptionMessage(e.what());

    /* Log the QLibraryInfo::PluginsPath if an exception message contains:
       QSqlError(Driver not loaded, Driver not loaded). */
    logPluginsPath(exceptionMessage);

    const auto message = QStringLiteral("\nCaught '")
                         .append(TypeUtils::classPureBasename(e, true))
                         .append(QStringLiteral("' Exception:\n"))
                         .append(exceptionMessage)
                         .append(QChar(QChar::LineFeed));

    if (fatal)
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        qFatal().nospace().noquote() << message;
#else
        qFatal("%s", message.toUtf8().constData());
#endif
    else
        qCritical().nospace().noquote() << message;
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
Helpers::setTimeZone(QDateTime &&datetime, const QtTimeZoneConfig &timezone) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
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

QDateTime Helpers::setTimeZone(QDateTime &&datetime, const QString &connection) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)○
{
    return setTimeZone(datetime, connection);
}

/* private */

void Helpers::logPluginsPath(const QString &exceptionMessage)
{
    const static auto
    DriverNotFound = QStringLiteral("QSqlError(Driver not loaded, Driver not loaded)");

    // Nothing to do
    if (!exceptionMessage.contains(DriverNotFound, Qt::CaseInsensitive))
        return;

    qInfo().nospace() << "Plugins Path: "
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                      << QLibraryInfo::path(QLibraryInfo::PluginsPath);
#else
                      << QLibraryInfo::location(QLibraryInfo::PluginsPath);
#endif
}

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE
