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
    const auto splitted = QStringView(value).split(DASH, Qt::KeepEmptyParts);

    if (splitted.size() != 3)
        return false;

    // Year
    if (const auto year = splitted.at(0);
        year.size() != 4 || !StringUtils::isNumber(year)
    )
        return false;

    // Month
    const auto month = splitted.at(1);
    const auto monthSize = month.size();
    if ((monthSize != 1 && monthSize != 2) || !StringUtils::isNumber(month))
        return false;

    // Day
    const auto day = splitted.at(2);
    const auto daySize = day.size();
    return (daySize == 1 || daySize == 2) && StringUtils::isNumber(day);
}

/* Don't throw in the next two methods as the qt_timezone value or value passed
   to the DatabaseConnection::setQtTimeZone() has already been validated
   in ConfigUtils::prepareQtTimeZone(), it throws there if a value is invalid. */

QDateTime
Helpers::convertTimeZone(const QDateTime &datetime, const QtTimeZoneConfig &timezone)
{
    const auto &[timezoneType, timezoneValue] = timezone;

    switch (timezoneType) {
    T_LIKELY
    case QtTimeZoneType::QTimeZone:
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        if (const auto typeId = timezoneValue.typeId();
            typeId == QMetaType::fromType<QTimeZone::Initialization>().id()
        )
            return datetime.toTimeZone(timezoneValue.value<QTimeZone::Initialization>());

        else if (typeId == QMetaType::fromType<QTimeZone>().id())
#endif
            return datetime.toTimeZone(timezoneValue.value<QTimeZone>());
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        else {
            Q_UNREACHABLE(); // See note above
            break; // Avoid the C26819 warning (MSVC)
        }
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
    T_LIKELY
    case QtTimeZoneType::QtTimeSpec:
        return datetime.toTimeSpec(timezoneValue.value<Qt::TimeSpec>());
#endif

    T_UNLIKELY
    case QtTimeZoneType::OffsetFromUtc:
        return datetime.toOffsetFromUtc(timezoneValue.value<int>());

    T_UNLIKELY
    default:
        Q_UNREACHABLE(); // See note above
    }

    /* Don't throw here as the qt_timezone value or value passed
       to the DatabaseConnection::setQtTimeZone() has already been validated
       in the ConfigUtils::prepareQtTimeZone(), it throws there if a value is invalid. */
    return datetime;
 }

QDateTime &
Helpers::setTimeZone(QDateTime &datetime, const QtTimeZoneConfig &timezone)
{
    const auto &[timezoneType, timezoneValue] = timezone;

    switch (timezoneType) {
    T_LIKELY
    case QtTimeZoneType::QTimeZone:
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        if (const auto typeId = timezoneValue.typeId();
            typeId == QMetaType::fromType<QTimeZone::Initialization>().id()
        )
            datetime.setTimeZone(timezoneValue.value<QTimeZone::Initialization>());
        else if (typeId == QMetaType::fromType<QTimeZone>().id())
#endif
            datetime.setTimeZone(timezoneValue.value<QTimeZone>());
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        else
            Q_UNREACHABLE(); // See note above
#endif
        break;

#if QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
    T_LIKELY
    case QtTimeZoneType::QtTimeSpec:
        datetime.setTimeSpec(timezoneValue.value<Qt::TimeSpec>());
        break;

    T_UNLIKELY
    case QtTimeZoneType::OffsetFromUtc:
        datetime.setOffsetFromUtc(timezoneValue.value<int>());
        break;
#endif

    T_UNLIKELY
    default:
        Q_UNREACHABLE(); // See note above
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
                      << QLibraryInfo::path(QLibraryInfo::PluginsPath);
}

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE
