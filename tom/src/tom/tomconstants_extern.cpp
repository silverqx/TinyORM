#include "tom/tomconstants_extern.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Constants
{

    const QString DateTimePrefix   = QStringLiteral("yyyy_MM_dd_HHmmss");

    // Common strings
    const QString LongOption       = QStringLiteral("--%1");
    const QString LongOptionOnly   = QStringLiteral("    %1");
    const QString LongOptionValue  = QStringLiteral("--%1[=%2]");
    const QString ShortOption      = QStringLiteral("-%1");

    const QString migration_  = QStringLiteral("migration");
    const QString batch_      = QStringLiteral("batch");

    // Common options
    const QString ansi          = QStringLiteral("ansi");
    const QString noansi        = QStringLiteral("no-ansi");
    const QString env           = QStringLiteral("env");
    const QString help          = QStringLiteral("help");
    const QString nointeraction = QStringLiteral("no-interaction");
    const QString quiet         = QStringLiteral("quiet");
    const QString version       = QStringLiteral("version"); // unused
    const QString verbose       = QStringLiteral("verbose"); // unused

    // Commands' positional options
    // help
    const QString command_name = QStringLiteral("command_name");
    // list
    const QString namespace_   = QStringLiteral("namespace");

    // Commands' options
    // Used by more commands
    const QString force        = QStringLiteral("force");
    const QString pretend      = QStringLiteral("pretend");
    const QString seed         = QStringLiteral("seed");
    const QString seeder       = QStringLiteral("seeder");
    const QString step_        = QStringLiteral("step");
    // Default value names
    const QString class_up     = QStringLiteral("CLASS");
    const QString database_up  = QStringLiteral("DATABASE");
    const QString seeder_up    = QStringLiteral("SEEDER");
    const QString step_up      = QStringLiteral("STEP");
    // list
    const QString raw_         = QStringLiteral("raw");
    // db:seed
    const QString class_       = QStringLiteral("class");
    // db:wipe
    const QString drop_views   = QStringLiteral("drop-views");
    const QString drop_types   = QStringLiteral("drop-types");
    // make:migration
    const QString create_      = QStringLiteral("create");
    const QString table_       = QStringLiteral("table");
    const QString path_        = QStringLiteral("path");
    const QString realpath_    = QStringLiteral("realpath");
    const QString fullpath     = QStringLiteral("fullpath");
    // migrate:refresh
    const QString step_migrate = QStringLiteral("step-migrate");

    // Namespace names
    const QString NsGlobal     = QStringLiteral("global");
    const QString NsDb         = QStringLiteral("db");
    const QString NsMake       = QStringLiteral("make");
    const QString NsMigrate    = QStringLiteral("migrate");
    const QString NsNamespaced = QStringLiteral("namespaced");

    // Command names
    const QString DbSeed          = QStringLiteral("db:seed");
    const QString DbWipe          = QStringLiteral("db:wipe");
    const QString Inspire         = QStringLiteral("inspire");
    const QString List            = QStringLiteral("list");
    const QString &Env            = Tom::Constants::env;
    const QString &Help           = Tom::Constants::help;
    const QString MakeMigration   = QStringLiteral("make:migration");
    const QString MakeProject     = QStringLiteral("make:project");
    const QString MakeSeeder      = QStringLiteral("make:seeder");
    const QString &Migrate        = Tom::Constants::NsMigrate;
    const QString MigrateFresh    = QStringLiteral("migrate:fresh");
    const QString MigrateInstall  = QStringLiteral("migrate:install");
    const QString MigrateRollback = QStringLiteral("migrate:rollback");
    const QString MigrateRefresh  = QStringLiteral("migrate:refresh");
    const QString MigrateReset    = QStringLiteral("migrate:reset");
    const QString MigrateStatus   = QStringLiteral("migrate:status");

} // namespace Tom::Constants

TINYORM_END_COMMON_NAMESPACE
