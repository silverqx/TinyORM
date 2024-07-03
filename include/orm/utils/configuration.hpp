#pragma once
#ifndef ORM_UTILS_CONFIGURATION_HPP
#define ORM_UTILS_CONFIGURATION_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QVariantHash>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
    struct QtTimeZoneConfig;

namespace Utils
{

    /*! Database configuration related library class. */
    class TINYORM_EXPORT Configuration
    {
        Q_DISABLE_COPY_MOVE(Configuration)

    public:
        /*! Deleted default constructor, this is a pure library class. */
        Configuration() = delete;
        /*! Deleted destructor. */
        ~Configuration() = delete;

        /*! Determine whether the database config. contains a valid version value. */
        static bool hasValidConfigVersion(const QVariantHash &config);
        /*! Get a valid config. version value. */
        static QString getValidConfigVersion(const QVariantHash &config);

        /*! Prepare the qt_timezone config. option on the base of the current value. */
        static QtTimeZoneConfig
        prepareQtTimeZone(const QVariantHash &config, const QString &connection);
        /*! Prepare the qt_timezone config. option on the base of the current value. */
        static QtTimeZoneConfig
        prepareQtTimeZone(const QVariant &qtTimeZone, const QString &connection);

        /* MySQL section */
        /*! Get a MySQL SSL-related options hash (for TLS connection). */
        static QVariantHash mysqlSslOptions();
        /*! Insert a MySQL SSL-related options to the MySQL's options configuration. */
        static QVariantHash insertMySqlSslOptions(QVariantHash &&options);
        /*! Insert a MySQL SSL-related options to the MySQL's options configuration. */
        static QVariantHash &insertMySqlSslOptions(QVariantHash &options);

        /*! Set connect/read/write timeouts to 1sec (use only for localhost). */
        static QVariantHash &minimizeMySqlTimeouts(QVariantHash &options);

        /* MariaDB section */
        /*! Get a MariaDB SSL-related options hash (for TLS connection). */
        static QVariantHash mariaSslOptions();
        /*! Insert a MariaDB SSL-related options to the MySQL's options configuration. */
        static QVariantHash insertMariaSslOptions(QVariantHash &&options);
        /*! Insert a MariaDB SSL-related options to the MySQL's options configuration. */
        static QVariantHash &insertMariaSslOptions(QVariantHash &options);

        /*! Set connect/read/write timeouts to 1sec (use only for localhost). */
        inline static QVariantHash &minimizeMariaTimeouts(QVariantHash &options);

        /* PostgreSQL section */
        /*! Get a PostgreSQL SSL-related options hash (for TLS connection). */
        static QVariantHash postgresSslOptions();
        /*! Insert a PostgreSQL SSL-related options to the PostgreSQL's options
            configuration. */
        static QVariantHash insertPostgresSslOptions(QVariantHash &&options);
        /*! Insert a PostgreSQL SSL-related options to the PostgreSQL's options
            configuration. */
        static QVariantHash &insertPostgresSslOptions(QVariantHash &options);

    private:
        /*! Prepare IANA ID for the QTimeZone constructor (prepends UTC if needed). */
        static QByteArray
        prepareTimeZoneId(QString &&timezoneId, const QString &connection);
        /*! Throw if the given time zone ianaId is not available. */
        static void
        throwIfBadTimeZoneId(const QByteArray &ianaId, const QString &connection);

        /*! Throw if the given qt_timezone value is deprecated Qt::TimeSpec. */
        static void
        throwIfDeprecatedTimeSpec(int typeId, const QString &connection);
    };

    /* public */

    QVariantHash &Configuration::minimizeMariaTimeouts(QVariantHash &options)
    {
        // Logic is the same as for MySQL server
        return minimizeMySqlTimeouts(options);
    }

} // namespace Utils
} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_UTILS_CONFIGURATION_HPP
