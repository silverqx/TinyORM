#include <orm/db.hpp>
#include <orm/utils/configuration.hpp>

#include <tom/application.hpp>

#include "migrations/2014_10_12_000000_create_posts_table.hpp"
#include "migrations/2014_10_12_100000_add_factor_column_to_posts_table.hpp"
#include "migrations/2014_10_12_200000_create_properties_table.hpp"
#include "migrations/2014_10_12_300000_create_phones_table.hpp"

#include "seeders/databaseseeder.hpp"

using Orm::DatabaseManager;
using Orm::DB;

using ConfigUtils = Orm::Utils::Configuration;

using TomApplication = Tom::Application;

using namespace Migrations; // NOLINT(google-build-using-namespace)
using namespace Seeders;    // NOLINT(google-build-using-namespace)

/*! Build the database manager instance and add a database connection. */
std::shared_ptr<DatabaseManager> setupManager();

/*! c++ main function. */
int main(int argc, char *argv[])
{
    try {
        // Ownership of the shared_ptr()
        auto db = setupManager();

        return TomApplication(argc, argv, std::move(db), "TOM_EXAMPLE_ENV",
                              QStringLiteral("migrations_example"))
                /* Default migrations path for the make:migration command, the path
                   can be absolute or relative (to the pwd at runtime). */
//                .migrationsPath("database/migrations")
//                .migrationsPath(std::filesystem::current_path() / "database" / "migrations")
                /* Migration classes can be named in two formats, StudlyCase without
                   the datetime prefix and snake_case with the datetime prefix.
                   If the StudlyCase name is used then the T_MIGRATION macro has to be
                   also used in the migration class. */
                .migrations<CreatePostsTable,
//                            _2014_10_12_000000_create_posts_table,
                            AddFactorColumnToPostsTable,
                            CreatePropertiesTable,
                            CreatePhonesTable>()
                /* Seeder classes, the DatabaseSeeder is the default/root seeder class,
                   it must always exist if the --class command-line argument is not
                   provided, or you can provide a custom name through the --class
                   argument.
                   The order of seeder classes doesn't matter, they will be called
                   in the order defined by the call<>() method inside the seeders
                   themselves. */
                .seeders<DatabaseSeeder>()
                // Fire it up 🔥🚀✨
                .run();

    } catch (const std::exception &e) {

        TomApplication::logException(e);
    }

    return EXIT_FAILURE;
}

std::shared_ptr<DatabaseManager> setupManager()
{
    using namespace Orm::Constants; // NOLINT(google-build-using-namespace)

    // Ownership of the shared_ptr()
    return DB::create({
        // MySQL connection
        {QStringLiteral("tinyorm_tom_mysql"), { // shell:connection
            {driver_,         QMYSQL},
            {host_,           qEnvironmentVariable("DB_MYSQL_HOST",      H127001)},
            {port_,           qEnvironmentVariable("DB_MYSQL_PORT",      P3306)},
            {database_,       qEnvironmentVariable("DB_MYSQL_DATABASE",  EMPTY)},
            {username_,       qEnvironmentVariable("DB_MYSQL_USERNAME",  EMPTY)},
            {password_,       qEnvironmentVariable("DB_MYSQL_PASSWORD",  EMPTY)},
            {charset_,        qEnvironmentVariable("DB_MYSQL_CHARSET",   UTF8MB4)},
            {collation_,      qEnvironmentVariable("DB_MYSQL_COLLATION", UTF8MB40900aici)},
            // SYSTEM - set the time zone to your local MySQL server time zone
            {timezone_,       TZ00},
            /* Specifies what time zone all QDateTime-s will have, the overridden default
               is the Qt::UTC, set to the Qt::LocalTime or QtTimeZoneType::DontConvert to
               use the system local time. */
            {qt_timezone,     QVariant::fromValue(Qt::UTC)},
            {prefix_,         EMPTY},
            {prefix_indexes,  false},
            {strict_,         true},
            {isolation_level, QStringLiteral("REPEATABLE READ")}, // MySQL default is REPEATABLE READ for InnoDB
            {engine_,         InnoDB},
            {Version,         {}}, // Autodetect
            {options_,        ConfigUtils::mysqlSslOptions()},
        }},

        // MariaDB connection
        {QStringLiteral("tinyorm_tom_maria"), { // shell:connection
            {driver_,         QMYSQL},
            {host_,           qEnvironmentVariable("DB_MARIA_HOST",      H127001)},
            {port_,           qEnvironmentVariable("DB_MARIA_PORT",      P3306)},
            {database_,       qEnvironmentVariable("DB_MARIA_DATABASE",  EMPTY)},
            {username_,       qEnvironmentVariable("DB_MARIA_USERNAME",  EMPTY)},
            {password_,       qEnvironmentVariable("DB_MARIA_PASSWORD",  EMPTY)},
            {charset_,        qEnvironmentVariable("DB_MARIA_CHARSET",   UTF8MB4)},
            {collation_,      qEnvironmentVariable("DB_MARIA_COLLATION",
                                                   UTF8MB4Unicode520ci)},
            // SYSTEM - set the time zone to your local MySQL server time zone
            {timezone_,       TZ00},
            /* Specifies what time zone all QDateTime-s will have, the overridden default
               is the Qt::UTC, set to the Qt::LocalTime or QtTimeZoneType::DontConvert to
               use the system local time. */
            {qt_timezone,     QVariant::fromValue(Qt::UTC)},
            {prefix_,         EMPTY},
            {prefix_indexes,  false},
            {strict_,         true},
            {isolation_level, QStringLiteral("REPEATABLE READ")}, // MySQL default is REPEATABLE READ for InnoDB
            {engine_,         InnoDB},
            {Version,         {}}, // Autodetect
            {options_,        ConfigUtils::mariaSslOptions()},
        }},

        // PostgreSQL connection
        {QStringLiteral("tinyorm_tom_postgres"), { // shell:connection
            {driver_,          QPSQL},
            {application_name, QStringLiteral("tom")},
            {host_,            qEnvironmentVariable("DB_PGSQL_HOST",       H127001)},
            {port_,            qEnvironmentVariable("DB_PGSQL_PORT",       P5432)},
            {database_,        qEnvironmentVariable("DB_PGSQL_DATABASE",   EMPTY)},
            {search_path,      qEnvironmentVariable("DB_PGSQL_SEARCHPATH", PUBLIC)},
            {username_,        qEnvironmentVariable("DB_PGSQL_USERNAME",   postgres_)},
            {password_,        qEnvironmentVariable("DB_PGSQL_PASSWORD",   EMPTY)},
            {charset_,         qEnvironmentVariable("DB_PGSQL_CHARSET",    UTF8)},
            // LOCAL/DEFAULT - set the time zone to your local PostgreSQL server time zone
            {timezone_,        UTC},
            /* Specifies what time zone all QDateTime-s will have, the overridden default
               is the Qt::UTC, set to the Qt::LocalTime or QtTimeZoneType::DontConvert to
               use the system local time. */
            {qt_timezone,      QVariant::fromValue(Qt::UTC)},
            // Examples of qt_timezone
//            {qt_timezone,        QVariant::fromValue(QTimeZone("Europe/Bratislava"))},
//            {qt_timezone,        "Europe/Prague"}, // Will be converted to QTimeZone("Europe/Prague")
//            {qt_timezone,        QVariant::fromValue(QTimeZone("UTC+04"))},
//            {qt_timezone,        "-03:00"},
//            {qt_timezone,        3600}, // Offset from UTC
//            {qt_timezone,        QVariant::fromValue(Qt::LocalTime)},
//            {qt_timezone,        {}}, // The same as Qt::LocalTime
            {prefix_,            EMPTY},
            {prefix_indexes,     false},
//            {isolation_level,    QStringLiteral("REPEATABLE READ")}, // Postgres default is READ COMMITTED
//            {synchronous_commit, QStringLiteral("off")}, // Postgres default is on
            // ConnectionFactory provides a default value for this (for reference only)
//            {dont_drop,          QStringList {spatial_ref_sys}},
            {options_,           ConfigUtils::postgresSslOptions()},
        }},

        // SQLite connection
        {QStringLiteral("tinyorm_tom_sqlite"), { // shell:connection
            {driver_,                 QSQLITE},
            {database_,               qEnvironmentVariable("DB_SQLITE_DATABASE", {})},
            {foreign_key_constraints, true},
            {check_database_exists,   true},
            /* Specifies what time zone all QDateTime-s will have, the overridden default
               is the Qt::UTC, set to the Qt::LocalTime or QtTimeZoneType::DontConvert to
               use the system local time. */
            {qt_timezone,             QVariant::fromValue(Qt::UTC)},
            /* Return a QDateTime with the correct time zone instead of the QString,
               only works when the qt_timezone isn't set to the DontConvert. */
            {return_qdatetime,        true},
            {prefix_,                 EMPTY},
            {prefix_indexes,          false},
        }}
    },
        // MySQL as the default database connection
        QStringLiteral("tinyorm_tom_mysql"));
}

/* Alternative syntax to instantiate migration classes. */
//        return TomApplication(argc, argv, std::move(db), "TOM_EXAMPLE_ENV",
//                              QStringLiteral("migrations_table")
//        {
//            std::make_shared<CreatePostsTable>(),
//            std::make_shared<AddFactorColumnToPostsTable>(),
//            std::make_shared<CreatePropertiesTable>(),
//            std::make_shared<CreatePhonesTable>(),
//        })
//            // Fire it up 🔥🚀✨
//            .run();
