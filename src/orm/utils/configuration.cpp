#include "orm/utils/configuration.hpp"

#include <QVersionNumber>

#include "orm/exceptions/invalidargumenterror.hpp"
#include "orm/ormtypes.hpp"
#include "orm/utils/helpers.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Utils::Helpers;

namespace Orm::Utils
{

/* public */

bool Configuration::hasValidConfigVersion(const QVariantHash &config)
{
    return !getValidConfigVersion(config).isEmpty();
}

QString Configuration::getValidConfigVersion(const QVariantHash &config)
{
    if (config.contains(Version))
        if (const auto version = config.value(Version);
            version.isValid() && !version.isNull() && version.canConvert<QString>()
        ) {
            auto versionValue = version.value<QString>();

            // Validate whether a version number is correctly formatted
            if (const auto versionNumber = QVersionNumber::fromString(versionValue);
                !versionNumber.isNull()
            )
                return versionValue;
        }

    return {};
}

QtTimeZoneConfig
Configuration::prepareQtTimeZone(const QVariantHash &config, const QString &connection)
{
    if (!config.contains(qt_timezone))
        return {QtTimeZoneType::DontConvert, {}};

    return prepareQtTimeZone(config[qt_timezone], connection);
}

QtTimeZoneConfig
Configuration::prepareQtTimeZone(const QVariant &qtTimeZone, const QString &connection)
{
    // Nothing to do
    if (!qtTimeZone.isValid() || qtTimeZone.isNull())
        return {QtTimeZoneType::DontConvert, {}};

    const auto typeId = Helpers::qVariantTypeId(qtTimeZone);

    if (typeId == QMetaType::fromType<Qt::TimeSpec>().id())
        return {QtTimeZoneType::QtTimeSpec, qtTimeZone.value<Qt::TimeSpec>()};

    if (typeId == QMetaType::fromType<QTimeZone>().id())
        return {QtTimeZoneType::QTimeZone,
                QVariant::fromValue(qtTimeZone.value<QTimeZone>())};

    /* Must be a valid IANA timezone ID and must be available on a current system.
       Also if a configuration option is the QString then return the QTimeZone
       right away, to avoid re-computations in the Connection::prepareBinding(). */
    if (typeId == QMetaType::QString)
        return {QtTimeZoneType::QTimeZone,
                QVariant::fromValue(
                        QTimeZone(prepareTimeZoneId(qtTimeZone.value<QString>(),
                                                    connection)))};

    switch (typeId) {
    case QMetaType::Int:
    case QMetaType::UInt:
    case QMetaType::LongLong:
    case QMetaType::ULongLong:
        return {QtTimeZoneType::OffsetFromUtc, qtTimeZone.value<int>()};

    default:
        Q_UNREACHABLE();
    }
}

/* private */

QByteArray
Configuration::prepareTimeZoneId(QString &&timezoneId, const QString &connection)
{
    const auto timezoneIdSize = timezoneId.size();

    // Prepend the UTC if needed (avoid RegEx for performance reasons)
    if (timezoneIdSize == 6 &&
        (timezoneId[0] == QChar('-') || timezoneId[0] == QChar('+')) &&
        // Is numeric != 0
        std::isdigit(timezoneId[1].toLatin1()) != 0 &&
        std::isdigit(timezoneId[2].toLatin1()) != 0 &&
        timezoneId[3] == QChar(':') &&
        std::isdigit(timezoneId[4].toLatin1()) != 0 &&
        std::isdigit(timezoneId[5].toLatin1()) != 0
    )
        timezoneId.prepend(UTC);

    // Append the :00 if needed (avoid RegEx for performance reasons)
    else if (timezoneIdSize == 6 &&
             timezoneId[0] == QChar('U') &&
             timezoneId[1] == QChar('T') &&
             timezoneId[2] == QChar('C') &&
             (timezoneId[3] == QChar('-') || timezoneId[3] == QChar('+')) &&
             // Is numeric != 0
             std::isdigit(timezoneId[4].toLatin1()) != 0 &&
             std::isdigit(timezoneId[5].toLatin1()) != 0
    )
        timezoneId.append(QStringLiteral(":00"));

    // Prepend the UTC and append the :00 if needed (avoid RegEx for performance reasons)
    else if (timezoneIdSize == 3 &&
             (timezoneId[0] == QChar('-') || timezoneId[0] == QChar('+')) &&
             // Is numeric != 0
             std::isdigit(timezoneId[1].toLatin1()) != 0 &&
             std::isdigit(timezoneId[2].toLatin1()) != 0
    )
        timezoneId.prepend(UTC).append(QStringLiteral(":00"));

    auto ianaId = timezoneId.toUtf8();

    // Must be a valid IANA timezone ID and be available on a current system
    throwIfBadTimeZoneId(ianaId, connection);

    return ianaId;
}

void Configuration::throwIfBadTimeZoneId(const QByteArray &ianaId,
                                         const QString &connection)
{
    if (QTimeZone::isTimeZoneIdAvailable(ianaId))
        return;

    throw Exceptions::InvalidArgumentError(
                QStringLiteral(
                    "The 'qt_timezone' configuration option '%1' for the '%2' "
                    "connection is not available on this system in %3().")
                .arg(QString::fromUtf8(ianaId), connection, __tiny_func__));
}

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE
