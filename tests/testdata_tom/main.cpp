#include <orm/db.hpp>

#include <tom/application.hpp>

#include "migrations/2022_05_11_170000_create_users_table.hpp"
#include "migrations/2022_05_11_170100_create_roles_table.hpp"
#include "migrations/2022_05_11_170200_create_role_user_table.hpp"
#include "migrations/2022_05_11_170300_create_user_phones_table.hpp"
#include "migrations/2022_05_11_170400_create_settings_table.hpp"
#include "migrations/2022_05_11_170500_create_torrents_table.hpp"
#include "migrations/2022_05_11_170600_create_torrent_peers_table.hpp"
#include "migrations/2022_05_11_170700_create_torrent_previewable_files_table.hpp"
#include "migrations/2022_05_11_170800_create_torrent_previewable_file_properties_table.hpp"
#include "migrations/2022_05_11_170900_create_file_property_properties_table.hpp"
#include "migrations/2022_05_11_171000_create_torrent_tags_table.hpp"
#include "migrations/2022_05_11_171100_create_tag_torrent_table.hpp"
#include "migrations/2022_05_11_171200_create_tag_properties_table.hpp"

using Orm::Constants::EMPTY;
using Orm::Constants::H127001;
using Orm::Constants::P3306;
using Orm::Constants::QMYSQL;
using Orm::Constants::SYSTEM;
using Orm::Constants::UTF8MB4;
using Orm::Constants::UTF8MB40900aici;
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

        return TomApplication(argc, argv, db, "TOM_TESTDATA_ENV")
                .migrations<CreateUsersTable,
                            CreateRolesTable,
                            CreateRoleUserTable,
                            CreateUserPhonesTable,
                            CreateSettingsTable,
                            CreateTorrentsTable,
                            CreateTorrentPeersTable,
                            CreateTorrentPreviewableFilesTable,
                            CreateTorrentPreviewableFilePropertiesTable,
                            CreateFilePropertyPropertiesTable,
                            CreateTorrentTagsTable,
                            CreateTagTorrentTable,
                            CreateTagPropertiesTable>()
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
        {collation_,      qEnvironmentVariable("DB_MYSQL_COLLATION", UTF8MB40900aici)},
        {timezone_,       SYSTEM},
        {prefix_,         EMPTY},
        {prefix_indexes,  true},
        {strict_,         true},
        {isolation_level, QStringLiteral("REPEATABLE READ")},
        {engine_,         InnoDB},
        {options_,        QVariantHash()},
    },
        QStringLiteral("tinyorm_testdata_tom"));
}
