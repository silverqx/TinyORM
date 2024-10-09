#include "tom/tomconstants_extern.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Constants
{

    const QString DateTimePrefix   = u"yyyy_MM_dd_HHmmss"_s;

    // Common chars - QChar(u'') is faster than ''_L1
    const QChar EQ_C = QChar(u'=');

    // Common strings
    const QString EMPTY            = u""_s; // This is the fastest
    const QString LongOption       = u"--%1"_s;
    const QString LongOptionEq     = u"--%1="_s;
    const QString LongOptionOnly   = u"    %1"_s;
    const QString LongOptionValue  = u"--%1[=%2]"_s;
    const QString ShortOption      = u"-%1"_s;
    const QString DoubleDash       = u"--"_s;

    const QString migration_    = u"migration"_s;
    const QString batch_        = u"batch"_s;

    // Templates
    const QString TMPL_RESULT2  = u"%1;%2"_s;
    const QString TMPL_RESULT3  = u"%1;%2;%3"_s;

    // Common options
    const QString ansi          = u"ansi"_s;
    const QString noansi        = u"no-ansi"_s;
    const QString env           = u"env"_s;
    const QString help          = u"help"_s;
    const QString nointeraction = u"no-interaction"_s;
    const QString quiet         = u"quiet"_s;
    const QString verbose       = u"verbose"_s;

    // Commands' positional options
    // help
    const QString command_name = u"command_name"_s;
    // list
    const QString namespace_   = u"namespace"_s;
    // integrate
    const QString shell_       = u"shell"_s;

    // Commands' options
    // Used by more commands
    const QString force        = u"force"_s;
    const QString pretend      = u"pretend"_s;
    const QString seed         = u"seed"_s;
    const QString seeder       = u"seeder"_s;
    const QString step_        = u"step"_s;
    const QString path_        = u"path"_s;
    const QString json_        = u"json"_s;
    const QString pretty       = u"pretty"_s;
    const QString only_        = u"only"_s;
    const QString database_    = u"database"_s;
    // Default value names
    const QString env_up             = u"ENV"_s;
    const QString class_up           = u"CLASS"_s;
    const QString database_up        = u"DATABASE"_s;
    const QString seeder_up          = u"SEEDER"_s;
    const QString batch_up           = u"BATCH"_s;
    const QString step_up            = u"STEP"_s;
    const QString commandline_up     = u"COMMANDLINE"_s;
    const QString position_up        = u"POSITION"_s;
    const QString word_up            = u"WORD"_s;
    const QString cargs_up           = u"CARGS"_s;
    const QString path_up            = u"PATH"_s;
    const QString create_up          = u"CREATE"_s;
    const QString table_up           = u"TABLE"_s;
    const QString primary_key_up     = u"PRIMARYKEY"_s;
    const QString connection_up      = u"CONNECTION"_s;
    const QString with_up            = u"WITH"_s;
    const QString fillable_up        = u"FILLABLE"_s;
    const QString guarded_up         = u"GUARDED"_s;
    const QString dateformat_up      = u"DATEFORMAT"_s;
    const QString dates_up           = u"DATES"_s;
    const QString touches_up         = u"TOUCHES"_s;
    const QString one_to_one_up      = u"OTO"_s;
    const QString one_to_many_up     = u"OTM"_s;
    const QString belongs_to_up      = u"BT"_s;
    const QString belongs_to_many_up = u"BTM"_s;
    const QString pivot_up           = u"PIVOT"_s;
    const QString pivot_inverse_up   = u"PIVOTINVERSE"_s;
    const QString as_up              = u"AS"_s;
    const QString with_pivot_up      = u"WITHPIVOT"_s;
    const QString foreign_key_up     = u"FOREIGNKEY"_s;
    const QString pivot_table_up     = u"PIVOTTABLE"_s;
    const QString visible_up         = u"VISIBLE"_s;
    const QString hidden_up          = u"HIDDEN"_s;
    const QString accessors_up       = u"ACCESSORS"_s;
    const QString appends_up         = u"APPENDS"_s;
    const QString only_up            = u"ONLY"_s;
    const QString shell_up           = u"SHELL"_s;
    // complete
    const QString commandline        = u"commandline"_s;
    const QString position_          = u"position"_s;
    const QString word_              = u"word"_s;
    const QString cargs_             = u"cargs"_s;
    // list
    const QString raw_               = u"raw"_s;
    // db:seed
    const QString class_             = u"class"_s;
    // db:wipe
    const QString drop_views         = u"drop-views"_s;
    const QString drop_types         = u"drop-types"_s;
    // make:migration, make:model, make:seeder
    const QString create_            = u"create"_s;
    const QString table_             = u"table"_s;
    const QString realpath_          = u"realpath"_s;
    const QString fullpath           = u"fullpath"_s;
    const QString from_model         = u"from-model"_s;
    // make:model
    const QString one_to_one           = u"one-to-one"_s;
    const QString one_to_many          = u"one-to-many"_s;
    const QString belongs_to           = u"belongs-to"_s;
    const QString belongs_to_many      = u"belongs-to-many"_s;
    const QString foreign_key          = u"foreign-key"_s;
    const QString pivot_table          = u"pivot-table"_s;
    const QString primary_key          = u"primary-key"_s;
    const QString incrementing         = u"incrementing"_s;
    const QString disable_incrementing = u"disable-incrementing"_s;
    const QString connection_          = u"connection"_s;
    const QString with_                = u"with"_s;
    const QString fillable             = u"fillable"_s;
    const QString guarded              = u"guarded"_s;
    const QString disable_timestamps   = u"disable-timestamps"_s;
    const QString dateformat           = u"dateformat"_s;
    const QString dates                = u"dates"_s;
    const QString touches              = u"touches"_s;
    const QString pivot_model          = u"pivot-model"_s;
    const QString pivot_inverse        = u"pivot-inverse"_s;
    const QString as_                  = u"as"_s;
    const QString with_timestamps      = u"with-timestamps"_s;
    const QString with_pivot           = u"with-pivot"_s;
    const QString preserve_order       = u"preserve-order"_s;
    const QString snake_attributes     = u"snake-attributes"_s;
    const QString disable_snake_attributes
                                       = u"disable-snake-attributes"_s;
    const QString casts_example        = u"casts-example"_s;
    const QString visible              = u"visible"_s;
    const QString hidden               = u"hidden"_s;
    const QString accessors            = u"accessors"_s;
    const QString appends              = u"appends"_s;
    // migrate:refresh
    const QString step_migrate         = u"step-migrate"_s;
    // migrate:status
    const QString pending_             = u"pending"_s;
    // migrate:uninstall
    const QString reset                = u"reset"_s;
    // integrate
    const QString stdout_              = u"stdout"_s;

    // Namespace names
    const QString NsGlobal     = u"global"_s;
    const QString NsComplete   = u"complete"_s;
    const QString NsDb         = u"db"_s;
    const QString NsMake       = u"make"_s;
    const QString NsMigrate    = u"migrate"_s;
    const QString NsNamespaced = u"namespaced"_s;
    const QString NsAll        = u"all"_s;

    // Shell names
    const QString ShBash       = u"bash"_s;
    const QString ShPwsh       = u"pwsh"_s;
    const QString ShZsh        = u"zsh"_s;

    // Command names
    const QString About            = u"about"_s;
    const QString CompleteBash     = u"complete:bash"_s;
    const QString CompletePwsh     = u"complete:pwsh"_s;
    const QString DbSeed           = u"db:seed"_s;
    const QString DbWipe           = u"db:wipe"_s;
    const QString &Env             = Tom::Constants::env;
    const QString &Help            = Tom::Constants::help;
    const QString Inspire          = u"inspire"_s;
    const QString Integrate        = u"integrate"_s;
    const QString List             = u"list"_s;
    const QString MakeMigration    = u"make:migration"_s;
    const QString MakeModel        = u"make:model"_s;
//    const QString MakeProject    = u"make:project"_s;
    const QString MakeSeeder       = u"make:seeder"_s;
    const QString &Migrate         = Tom::Constants::NsMigrate;
    const QString MigrateFresh     = u"migrate:fresh"_s;
    const QString MigrateInstall   = u"migrate:install"_s;
    const QString MigrateRefresh   = u"migrate:refresh"_s;
    const QString MigrateReset     = u"migrate:reset"_s;
    const QString MigrateRollback  = u"migrate:rollback"_s;
    const QString MigrateStatus    = u"migrate:status"_s;
    const QString MigrateUninstall = u"migrate:uninstall"_s;

} // namespace Tom::Constants

TINYORM_END_COMMON_NAMESPACE
