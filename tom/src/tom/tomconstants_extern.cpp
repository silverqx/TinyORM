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
    const QString DoubleDash       = QStringLiteral("--");

    const QString migration_  = QStringLiteral("migration");
    const QString batch_      = QStringLiteral("batch");

    // Common options
    const QString ansi          = QStringLiteral("ansi");
    const QString noansi        = QStringLiteral("no-ansi");
    const QString env           = QStringLiteral("env");
    const QString help          = QStringLiteral("help");
    const QString nointeraction = QStringLiteral("no-interaction");
    const QString quiet         = QStringLiteral("quiet");
    const QString verbose       = QStringLiteral("verbose");

    // Commands' positional options
    // help
    const QString command_name = QStringLiteral("command_name");
    // list
    const QString namespace_   = QStringLiteral("namespace");
    const QString shell_       = QStringLiteral("shell");

    // Commands' options
    // Used by more commands
    const QString force        = QStringLiteral("force");
    const QString pretend      = QStringLiteral("pretend");
    const QString seed         = QStringLiteral("seed");
    const QString seeder       = QStringLiteral("seeder");
    const QString step_        = QStringLiteral("step");
    const QString path_        = QStringLiteral("path");
    const QString json_        = QStringLiteral("json");
    const QString pretty       = QStringLiteral("pretty");
    const QString only_        = QStringLiteral("only");
    // Default value names
    const QString env_up             = QStringLiteral("ENV");
    const QString class_up           = QStringLiteral("CLASS");
    const QString database_up        = QStringLiteral("DATABASE");
    const QString seeder_up          = QStringLiteral("SEEDER");
    const QString batch_up           = QStringLiteral("BATCH");
    const QString step_up            = QStringLiteral("STEP");
    const QString commandline_up     = QStringLiteral("COMMANDLINE");
    const QString position_up        = QStringLiteral("POSITION");
    const QString word_up            = QStringLiteral("WORD");
    const QString cword_up           = QStringLiteral("CWORD");
    const QString path_up            = QStringLiteral("PATH");
    const QString create_up          = QStringLiteral("CREATE");
    const QString table_up           = QStringLiteral("TABLE");
    const QString primary_key_up     = QStringLiteral("PRIMARYKEY");
    const QString connection_up      = QStringLiteral("CONNECTION");
    const QString with_up            = QStringLiteral("WITH");
    const QString fillable_up        = QStringLiteral("FILLABLE");
    const QString guarded_up         = QStringLiteral("GUARDED");
    const QString dateformat_up      = QStringLiteral("DATEFORMAT");
    const QString dates_up           = QStringLiteral("DATES");
    const QString touches_up         = QStringLiteral("TOUCHES");
    const QString one_to_one_up      = QStringLiteral("OTO");
    const QString one_to_many_up     = QStringLiteral("OTM");
    const QString belongs_to_up      = QStringLiteral("BT");
    const QString belongs_to_many_up = QStringLiteral("BTM");
    const QString pivot_up           = QStringLiteral("PIVOT");
    const QString pivot_inverse_up   = QStringLiteral("PIVOTINVERSE");
    const QString as_up              = QStringLiteral("AS");
    const QString with_pivot_up      = QStringLiteral("WITHPIVOT");
    const QString foreign_key_up     = QStringLiteral("FOREIGNKEY");
    const QString pivot_table_up     = QStringLiteral("PIVOTTABLE");
    const QString visible_up         = QStringLiteral("VISIBLE");
    const QString hidden_up          = QStringLiteral("HIDDEN");
    const QString accessors_up       = QStringLiteral("ACCESSORS");
    const QString appends_up         = QStringLiteral("APPENDS");
    const QString only_up            = QStringLiteral("ONLY");
    // complete
    const QString commandline        = QStringLiteral("commandline");
    const QString position           = QStringLiteral("position");
    const QString word_              = QStringLiteral("word");
    const QString cword_             = QStringLiteral("cword");
    // list
    const QString raw_               = QStringLiteral("raw");
    // db:seed
    const QString class_             = QStringLiteral("class");
    // db:wipe
    const QString drop_views         = QStringLiteral("drop-views");
    const QString drop_types         = QStringLiteral("drop-types");
    // make:migration, make:model, make:seeder
    const QString create_            = QStringLiteral("create");
    const QString table_             = QStringLiteral("table");
    const QString realpath_          = QStringLiteral("realpath");
    const QString fullpath           = QStringLiteral("fullpath");
    const QString from_model         = QStringLiteral("from-model");
    // make:model
    const QString one_to_one           = QStringLiteral("one-to-one");
    const QString one_to_many          = QStringLiteral("one-to-many");
    const QString belongs_to           = QStringLiteral("belongs-to");
    const QString belongs_to_many      = QStringLiteral("belongs-to-many");
    const QString foreign_key          = QStringLiteral("foreign-key");
    const QString pivot_table          = QStringLiteral("pivot-table");
    const QString primary_key          = QStringLiteral("primary-key");
    const QString incrementing         = QStringLiteral("incrementing");
    const QString disable_incrementing = QStringLiteral("disable-incrementing");
    const QString connection_          = QStringLiteral("connection");
    const QString with_                = QStringLiteral("with");
    const QString fillable             = QStringLiteral("fillable");
    const QString guarded              = QStringLiteral("guarded");
    const QString disable_timestamps   = QStringLiteral("disable-timestamps");
    const QString dateformat           = QStringLiteral("dateformat");
    const QString dates                = QStringLiteral("dates");
    const QString touches              = QStringLiteral("touches");
    const QString pivot_model          = QStringLiteral("pivot-model");
    const QString pivot_inverse        = QStringLiteral("pivot-inverse");
    const QString as_                  = QStringLiteral("as");
    const QString with_timestamps      = QStringLiteral("with-timestamps");
    const QString with_pivot           = QStringLiteral("with-pivot");
    const QString preserve_order       = QStringLiteral("preserve-order");
    const QString snake_attributes     = QStringLiteral("snake-attributes");
    const QString disable_snake_attributes
                                       = QStringLiteral("disable-snake-attributes");
    const QString casts_example        = QStringLiteral("casts-example");
    const QString visible              = QStringLiteral("visible");
    const QString hidden               = QStringLiteral("hidden");
    const QString accessors            = QStringLiteral("accessors");
    const QString appends              = QStringLiteral("appends");
    // migrate:refresh
    const QString step_migrate         = QStringLiteral("step-migrate");
    // migrate:status
    const QString pending_             = QStringLiteral("pending");
    // migrate:uninstall
    const QString reset                = QStringLiteral("reset");
    // integrate
    const QString stdout_              = QStringLiteral("stdout");

    // Namespace names
    const QString NsGlobal     = QStringLiteral("global");
    const QString NsDb         = QStringLiteral("db");
    const QString NsMake       = QStringLiteral("make");
    const QString NsMigrate    = QStringLiteral("migrate");
    const QString NsNamespaced = QStringLiteral("namespaced");
    const QString NsAll        = QStringLiteral("all");

    // Shell names
    const QString ShBash       = QStringLiteral("bash");
    const QString ShPwsh       = QStringLiteral("pwsh");
    const QString ShZsh        = QStringLiteral("zsh");

    // Command names
    const QString About            = QStringLiteral("about");
    const QString Complete         = QStringLiteral("complete");
    const QString DbSeed           = QStringLiteral("db:seed");
    const QString DbWipe           = QStringLiteral("db:wipe");
    const QString Inspire          = QStringLiteral("inspire");
    const QString List             = QStringLiteral("list");
    const QString &Env             = Tom::Constants::env;
    const QString &Help            = Tom::Constants::help;
    const QString MakeMigration    = QStringLiteral("make:migration");
    const QString MakeModel        = QStringLiteral("make:model");
    const QString MakeProject      = QStringLiteral("make:project");
    const QString MakeSeeder       = QStringLiteral("make:seeder");
    const QString &Migrate         = Tom::Constants::NsMigrate;
    const QString MigrateFresh     = QStringLiteral("migrate:fresh");
    const QString MigrateInstall   = QStringLiteral("migrate:install");
    const QString MigrateRollback  = QStringLiteral("migrate:rollback");
    const QString MigrateRefresh   = QStringLiteral("migrate:refresh");
    const QString MigrateReset     = QStringLiteral("migrate:reset");
    const QString MigrateStatus    = QStringLiteral("migrate:status");
    const QString MigrateUninstall = QStringLiteral("migrate:uninstall");
    const QString Integrate        = QStringLiteral("integrate");

} // namespace Tom::Constants

TINYORM_END_COMMON_NAMESPACE
