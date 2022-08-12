#include <orm/db.hpp>

#include <tom/application.hpp>

#include "migrations/2014_10_12_000000_create_posts_table.hpp"
#include "migrations/2014_10_12_100000_add_factor_column_to_posts_table.hpp"
#include "migrations/2014_10_12_200000_create_properties_table.hpp"
#include "migrations/2014_10_12_300000_create_phones_table.hpp"

#include "seeders/databaseseeder.hpp"

using Orm::DatabaseManager;
using Orm::DB;

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

        return TomApplication(argc, argv, db, "TOM_EXAMPLE_ENV",
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
            {host_,           qEnvironmentVariable("DB_MYSQL_HOST", H127001)},
            {port_,           qEnvironmentVariable("DB_MYSQL_PORT", P3306)},
            {database_,       qEnvironmentVariable("DB_MYSQL_DATABASE", EMPTY)},
            {username_,       qEnvironmentVariable("DB_MYSQL_USERNAME", EMPTY)},
            {password_,       qEnvironmentVariable("DB_MYSQL_PASSWORD", EMPTY)},
            {charset_,        qEnvironmentVariable("DB_MYSQL_CHARSET", UTF8MB4)},
            {collation_,      qEnvironmentVariable("DB_MYSQL_COLLATION", UTF8MB40900aici)},
            {timezone_,       TZ00},
            {prefix_,         EMPTY},
            {prefix_indexes,  true},
            {strict_,         true},
            {isolation_level, QStringLiteral("REPEATABLE READ")},
            {engine_,         InnoDB},
            {Version,         {}}, // Autodetect
            {options_,        QVariantHash()},
        }},

        // PostgreSQL connection
        {QStringLiteral("tinyorm_tom_postgres"), { // shell:connection
            {driver_,        QPSQL},
            {host_,          qEnvironmentVariable("DB_PGSQL_HOST",     H127001)},
            {port_,          qEnvironmentVariable("DB_PGSQL_PORT",     P5432)},
            {database_,      qEnvironmentVariable("DB_PGSQL_DATABASE", EMPTY)},
            {schema_,        qEnvironmentVariable("DB_PGSQL_SCHEMA",   PUBLIC)},
            {username_,      qEnvironmentVariable("DB_PGSQL_USERNAME", postgres_)},
            {password_,      qEnvironmentVariable("DB_PGSQL_PASSWORD", EMPTY)},
            {charset_,       qEnvironmentVariable("DB_PGSQL_CHARSET",  UTF8)},
            // I don't use timezone types in postgres anyway
            {timezone_,      UTC},
            {prefix_,        EMPTY},
            {prefix_indexes, true},
            // ConnectionFactory provides a default value for this (for reference only)
//            {dont_drop,      QStringList {QStringLiteral("spatial_ref_sys")}},
            {options_,       QVariantHash()},
        }},

        // SQLite connection
        {QStringLiteral("tinyorm_tom_sqlite"), { // shell:connection
            {driver_,                 QSQLITE},
            {database_,               qEnvironmentVariable("DB_SQLITE_DATABASE", {})},
            {foreign_key_constraints, true},
            {check_database_exists,   true},
            {prefix_,                 EMPTY},
        }}
    },
        // MySQL as the default database connection
        QStringLiteral("tinyorm_tom_mysql"));
}

/* Alternative syntax to instantiate migration classes. */
//        return TomApplication(argc, argv, db, "TOM_EXAMPLE_ENV",
//        {
//            std::make_shared<CreatePostsTable>(),
//            std::make_shared<AddFactorColumnToPostsTable>(),
//            std::make_shared<CreatePropertiesTable>(),
//            std::make_shared<CreatePhonesTable>(),
//        })
//            // Fire it up 🔥🚀✨
//            .run();
