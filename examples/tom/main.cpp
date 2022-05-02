#include <orm/db.hpp>

#include <tom/application.hpp>

#include "migrations/2014_10_12_000000_create_posts_table.hpp"
#include "migrations/2014_10_12_100000_add_factor_column_to_posts_table.hpp"
#include "migrations/2014_10_12_200000_create_properties_table.hpp"
#include "migrations/2014_10_12_300000_create_phones_table.hpp"

using Orm::Constants::EMPTY;
using Orm::Constants::H127001;
using Orm::Constants::P3306;
using Orm::Constants::QMYSQL;
using Orm::Constants::SYSTEM;
using Orm::Constants::UTF8MB4;
using Orm::Constants::charset_;
using Orm::Constants::collation_;
using Orm::Constants::database_;
using Orm::Constants::driver_;
using Orm::Constants::engine_;
using Orm::Constants::host_;
using Orm::Constants::InnoDB;
using Orm::Constants::isolation_level;
using Orm::Constants::options_;
using Orm::Constants::password_;
using Orm::Constants::port_;
using Orm::Constants::prefix_;
using Orm::Constants::prefix_indexes;
using Orm::Constants::strict_;
using Orm::Constants::timezone_;
using Orm::Constants::username_;

using Orm::DatabaseManager;
using Orm::DB;

using TomApplication = Tom::Application;

using namespace Migrations; // NOLINT(google-build-using-namespace)

/*! Build the database manager instance and add a database connection. */
std::shared_ptr<DatabaseManager> setupManager();

/*! c++ main function. */
int main(int argc, char *argv[])
{
    try {
        // Ownership of the shared_ptr()
        auto db = setupManager();

        return TomApplication(argc, argv, db, "TOM_EXAMPLE_ENV")
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
                // Fire it up 🔥🚀✨
                .run();

    } catch (const std::exception &e) {

        TomApplication::logException(e);
    }

    return EXIT_FAILURE;
}

std::shared_ptr<DatabaseManager> setupManager()
{
    // Ownership of the shared_ptr()
    return DB::create({
        {driver_,         QMYSQL},
        {host_,           qEnvironmentVariable("DB_MYSQL_HOST", H127001)},
        {port_,           qEnvironmentVariable("DB_MYSQL_PORT", P3306)},
        {database_,       qEnvironmentVariable("DB_MYSQL_DATABASE", EMPTY)},
        {username_,       qEnvironmentVariable("DB_MYSQL_USERNAME", EMPTY)},
        {password_,       qEnvironmentVariable("DB_MYSQL_PASSWORD", EMPTY)},
        {charset_,        qEnvironmentVariable("DB_MYSQL_CHARSET", UTF8MB4)},
        {collation_,      qEnvironmentVariable("DB_MYSQL_COLLATION",
                                               QStringLiteral("utf8mb4_0900_ai_ci"))},
        {timezone_,       SYSTEM},
        {prefix_,         EMPTY},
        {prefix_indexes,  true},
        {strict_,         true},
        {isolation_level, QStringLiteral("REPEATABLE READ")},
        {engine_,         InnoDB},
        {options_,        QVariantHash()},
    },
        QStringLiteral("tinyorm_tom"));
}

/* Alternative syntax to instantiate migration classes. */
//        return TomApplication(argc, argv, db, "TOM_EXAMPLE_ENV",
//        {
//            std::make_shared<_2014_10_12_000000_create_posts_table>(),
//            std::make_shared<_2014_10_12_100000_add_factor_column_to_posts_table>(),
//            std::make_shared<_2014_10_12_200000_create_properties_table>(),
//            std::make_shared<_2014_10_12_300000_create_phones_table>(),
//        })
//            // Fire it up 🔥🚀✨
//            .run();
