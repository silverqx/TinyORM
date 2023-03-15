#include "orm/utils/configuration.hpp"

#include <QVersionNumber>

#include "orm/exceptions/invalidargumenterror.hpp"
#include "orm/ormconcepts.hpp" // IWYU pragma: keep
#include "orm/ormtypes.hpp"
#include "orm/utils/helpers.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

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

/* MySQL section */

namespace
{
    /*! Insert a new item to the DB options hash. */
    template<QVariantConcept T>
    void insertDbOption(QVariantHash &options, const QString &key, T &&value)
    {
        if (value.isEmpty())
            return;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        options.insert(key, value);
#else
        options.emplace(key, std::forward<T>(value));
#endif
    }

    /*! MySQL SSL options hash. */
    QVariantHash getMySqlSslOptions()
    {
        QVariantHash options;
        options.reserve(3);

        /* Don't make any special validation here like both the KEY and CERT has to be
           set at once, simply set what is set in the environment variables. */
        insertDbOption(options, SSL_CERT, qEnvironmentVariable("DB_MYSQL_SSL_CERT"));
        insertDbOption(options, SSL_KEY,  qEnvironmentVariable("DB_MYSQL_SSL_KEY"));
        insertDbOption(options, SSL_CA,   qEnvironmentVariable("DB_MYSQL_SSL_CA"));

        return options;
    }
} // namespace

QVariantHash Configuration::mysqlSslOptions()
{
    QVariantHash options;
    options.reserve(8);

    options.insert(getMySqlSslOptions());

    return options;
}

QVariantHash Configuration::insertMySqlSslOptions(QVariantHash &&options)
{
    options.insert(getMySqlSslOptions());

    return std::move(options);
}

QVariantHash &Configuration::insertMySqlSslOptions(QVariantHash &options)
{
    options.insert(getMySqlSslOptions());

    return options;
}

QVariantHash &Configuration::minimizeMySqlTimeouts(QVariantHash &options)
{
    options.insert({{QStringLiteral("MYSQL_OPT_CONNECT_TIMEOUT"), 1},
                    {QStringLiteral("MYSQL_OPT_READ_TIMEOUT"),    1},
                    {QStringLiteral("MYSQL_OPT_WRITE_TIMEOUT"),   1}});

    return options;
}

/* MariaDB section */

namespace
{
    /*! MariaDB SSL options hash. */
    QVariantHash getMariaSslOptions()
    {
        QVariantHash options;
        options.reserve(3);

        /* Don't make any special validation here like both the KEY and CERT has to be
           set at once, simply set what is set in the environment variables. */
        insertDbOption(options, SSL_CERT, qEnvironmentVariable("DB_MARIA_SSL_CERT"));
        insertDbOption(options, SSL_KEY,  qEnvironmentVariable("DB_MARIA_SSL_KEY"));
        insertDbOption(options, SSL_CA,   qEnvironmentVariable("DB_MARIA_SSL_CA"));

        return options;
    }
} // namespace

QVariantHash Configuration::mariaSslOptions()
{
    QVariantHash options;
    options.reserve(8);

    options.insert(getMariaSslOptions());

    return options;
}

QVariantHash Configuration::insertMariaSslOptions(QVariantHash &&options)
{
    options.insert(getMariaSslOptions());

    return std::move(options);
}

QVariantHash &Configuration::insertMariaSslOptions(QVariantHash &options)
{
    options.insert(getMariaSslOptions());

    return options;
}

/* PostgreSQL section */

namespace
{
    /*! PostgreSQL SSL options hash. */
    QVariantHash getPostgresSslOptions()
    {
        QVariantHash options;
        options.reserve(4);

        insertDbOption(options, sslmode_,    qEnvironmentVariable("DB_PGSQL_SSLMODE"));
        insertDbOption(options, sslcert,     qEnvironmentVariable("DB_PGSQL_SSLCERT"));
        insertDbOption(options, sslkey,      qEnvironmentVariable("DB_PGSQL_SSLKEY"));
        insertDbOption(options, sslrootcert,
                       qEnvironmentVariable("DB_PGSQL_SSLROOTCERT"));

        return options;
    }
} // namespace

QVariantHash Configuration::postgresSslOptions()
{
    QVariantHash options;
    options.reserve(9);

    options.insert(getPostgresSslOptions());

    return options;
}

QVariantHash Configuration::insertPostgresSslOptions(QVariantHash &&options)
{
    options.insert(getPostgresSslOptions());

    return std::move(options);
}

QVariantHash &Configuration::insertPostgresSslOptions(QVariantHash &options)
{
    options.insert(getPostgresSslOptions());

    return options;
}

/* private */

QByteArray
Configuration::prepareTimeZoneId(QString &&timezoneId, const QString &connection)
{
    const auto timezoneIdSize = timezoneId.size();

    // Prepend the UTC if needed (avoid RegEx for performance reasons)
    if (timezoneIdSize == 6 &&
        (timezoneId[0] == MINUS || timezoneId[0] == PLUS) &&
        // Is numeric != 0
        std::isdigit(timezoneId[1].toLatin1()) != 0 &&
        std::isdigit(timezoneId[2].toLatin1()) != 0 &&
        timezoneId[3] == COLON &&
        std::isdigit(timezoneId[4].toLatin1()) != 0 &&
        std::isdigit(timezoneId[5].toLatin1()) != 0
    )
        timezoneId.prepend(UTC);

    // Append the :00 if needed (avoid RegEx for performance reasons)
    else if (timezoneIdSize == 6 &&
             timezoneId[0] == QLatin1Char('U') &&
             timezoneId[1] == QLatin1Char('T') &&
             timezoneId[2] == QLatin1Char('C') &&
             (timezoneId[3] == MINUS || timezoneId[3] == PLUS) &&
             // Is numeric != 0
             std::isdigit(timezoneId[4].toLatin1()) != 0 &&
             std::isdigit(timezoneId[5].toLatin1()) != 0
    )
        timezoneId.append(QStringLiteral(":00"));

    // Prepend the UTC and append the :00 if needed (avoid RegEx for performance reasons)
    else if (timezoneIdSize == 3 &&
             (timezoneId[0] == MINUS || timezoneId[0] == PLUS) &&
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
