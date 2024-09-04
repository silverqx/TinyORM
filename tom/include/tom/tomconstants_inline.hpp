#pragma once
#ifndef TOM_TOMCONSTANTS_INLINE_HPP
#define TOM_TOMCONSTANTS_INLINE_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QString>

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
{
    /*! Alias for the literal operator that creates a QLatin1Char/StringView. */
    using Qt::StringLiterals::operator""_L1;
    /*! Alias for the literal operator that creates a QString. */
    using Qt::StringLiterals::operator""_s;

/*! Namespace contains common strings used in the Tom namespace (migrations). */
namespace Constants
{

    /*! Migration files datetime prefix format. */
    inline const QString DateTimePrefix  = u"yyyy_MM_dd_HHmmss"_s;

    // Common strings
    inline const QString EMPTY           = u""_s; // This is fastest
    inline const QString LongOption      = u"--%1"_s;
    inline const QString LongOptionOnly  = u"    %1"_s;
    inline const QString LongOptionValue = u"--%1[=%2]"_s;
    inline const QString ShortOption     = u"-%1"_s;
    inline const QString DoubleDash      = u"--"_s;

    inline const QString migration_    = u"migration"_s;
    inline const QString batch_        = u"batch"_s;

    // Common options
    inline const QString ansi          = u"ansi"_s;
    inline const QString noansi        = u"no-ansi"_s;
    inline const QString env           = u"env"_s;
    inline const QString help          = u"help"_s;
    inline const QString nointeraction = u"no-interaction"_s;
    inline const QString quiet         = u"quiet"_s;
    inline const QString verbose       = u"verbose"_s;

    // Commands' positional options
    // help
    inline const QString command_name = u"command_name"_s;
    // list
    inline const QString namespace_   = u"namespace"_s;
    inline const QString shell_       = u"shell"_s;

    // Commands' options
    // Used by more commands
    inline const QString force        = u"force"_s;
    inline const QString pretend      = u"pretend"_s;
    inline const QString seed         = u"seed"_s;
    inline const QString seeder       = u"seeder"_s;
    inline const QString step_        = u"step"_s;
    inline const QString path_        = u"path"_s;
    inline const QString json_        = u"json"_s;
    inline const QString pretty       = u"pretty"_s;
    inline const QString only_        = u"only"_s;
    // Default value names
    inline const QString env_up             = u"ENV"_s;
    inline const QString class_up           = u"CLASS"_s;
    inline const QString database_up        = u"DATABASE"_s;
    inline const QString seeder_up          = u"SEEDER"_s;
    inline const QString batch_up           = u"BATCH"_s;
    inline const QString step_up            = u"STEP"_s;
    inline const QString commandline_up     = u"COMMANDLINE"_s;
    inline const QString position_up        = u"POSITION"_s;
    inline const QString word_up            = u"WORD"_s;
    inline const QString cword_up           = u"CWORD"_s;
    inline const QString path_up            = u"PATH"_s;
    inline const QString create_up          = u"CREATE"_s;
    inline const QString table_up           = u"TABLE"_s;
    inline const QString primary_key_up     = u"PRIMARYKEY"_s;
    inline const QString connection_up      = u"CONNECTION"_s;
    inline const QString with_up            = u"WITH"_s;
    inline const QString fillable_up        = u"FILLABLE"_s;
    inline const QString guarded_up         = u"GUARDED"_s;
    inline const QString dateformat_up      = u"DATEFORMAT"_s;
    inline const QString dates_up           = u"DATES"_s;
    inline const QString touches_up         = u"TOUCHES"_s;
    inline const QString one_to_one_up      = u"OTO"_s;
    inline const QString one_to_many_up     = u"OTM"_s;
    inline const QString belongs_to_up      = u"BT"_s;
    inline const QString belongs_to_many_up = u"BTM"_s;
    inline const QString pivot_up           = u"PIVOT"_s;
    inline const QString pivot_inverse_up   = u"PIVOTINVERSE"_s;
    inline const QString as_up              = u"AS"_s;
    inline const QString with_pivot_up      = u"WITHPIVOT"_s;
    inline const QString foreign_key_up     = u"FOREIGNKEY"_s;
    inline const QString pivot_table_up     = u"PIVOTTABLE"_s;
    inline const QString visible_up         = u"VISIBLE"_s;
    inline const QString hidden_up          = u"HIDDEN"_s;
    inline const QString accessors_up       = u"ACCESSORS"_s;
    inline const QString appends_up         = u"APPENDS"_s;
    inline const QString only_up            = u"ONLY"_s;
    // complete
    inline const QString commandline        = u"commandline"_s;
    inline const QString position_          = u"position"_s;
    inline const QString word_              = u"word"_s;
    inline const QString cword_             = u"cword"_s;
    // list
    inline const QString raw_               = u"raw"_s;
    // db:seed
    inline const QString class_             = u"class"_s;
    // db:wipe
    inline const QString drop_views         = u"drop-views"_s;
    inline const QString drop_types         = u"drop-types"_s;
    // make:migration, make:model, make:seeder
    inline const QString create_            = u"create"_s;
    inline const QString table_             = u"table"_s;
    inline const QString realpath_          = u"realpath"_s;
    inline const QString fullpath           = u"fullpath"_s;
    inline const QString from_model         = u"from-model"_s;
    // make:model
    inline const QString one_to_one           = u"one-to-one"_s;
    inline const QString one_to_many          = u"one-to-many"_s;
    inline const QString belongs_to           = u"belongs-to"_s;
    inline const QString belongs_to_many      = u"belongs-to-many"_s;
    inline const QString foreign_key          = u"foreign-key"_s;
    inline const QString pivot_table          = u"pivot-table"_s;
    inline const QString primary_key          = u"primary-key"_s;
    inline const QString incrementing         = u"incrementing"_s;
    inline const QString disable_incrementing = u"disable-incrementing"_s;
    inline const QString connection_          = u"connection"_s;
    inline const QString with_                = u"with"_s;
    inline const QString fillable             = u"fillable"_s;
    inline const QString guarded              = u"guarded"_s;
    inline const QString disable_timestamps   = u"disable-timestamps"_s;
    inline const QString dateformat           = u"dateformat"_s;
    inline const QString dates                = u"dates"_s;
    inline const QString touches              = u"touches"_s;
    inline const QString pivot_model          = u"pivot-model"_s;
    inline const QString pivot_inverse        = u"pivot-inverse"_s;
    inline const QString as_                  = u"as"_s;
    inline const QString with_timestamps      = u"with-timestamps"_s;
    inline const QString with_pivot           = u"with-pivot"_s;
    inline const QString preserve_order       = u"preserve-order"_s;
    inline const QString casts_example        = u"casts-example"_s;
    inline const QString snake_attributes     = u"snake-attributes"_s;
    inline const QString
    disable_snake_attributes                  = u"disable-snake-attributes"_s;
    inline const QString visible              = u"visible"_s;
    inline const QString hidden               = u"hidden"_s;
    inline const QString accessors            = u"accessors"_s;
    inline const QString appends              = u"appends"_s;
    // migrate:refresh
    inline const QString step_migrate         = u"step-migrate"_s;
    // migrate:status
    inline const QString pending_             = u"pending"_s;
    // migrate:uninstall
    inline const QString reset                = u"reset"_s;
    // integrate
    inline const QString stdout_              = u"stdout"_s;

    // Namespace names
    inline const QString NsGlobal     = u"global"_s;
    inline const QString NsDb         = u"db"_s;
    inline const QString NsMake       = u"make"_s;
    inline const QString NsMigrate    = u"migrate"_s;
    inline const QString NsNamespaced = u"namespaced"_s;
    inline const QString NsAll        = u"all"_s;

    // Shell names
    inline const QString ShBash       = u"bash"_s;
    inline const QString ShPwsh       = u"pwsh"_s;
    inline const QString ShZsh        = u"zsh"_s;

    // Command names
    inline const QString About            = u"about"_s;
    inline const QString Complete         = u"complete"_s;
    inline const QString DbSeed           = u"db:seed"_s;
    inline const QString DbWipe           = u"db:wipe"_s;
    inline const QString Inspire          = u"inspire"_s;
    inline const QString List             = u"list"_s;
    inline const QString &Env             = Tom::Constants::env;
    inline const QString &Help            = Tom::Constants::help;
    inline const QString MakeMigration    = u"make:migration"_s;
    inline const QString MakeModel        = u"make:model"_s;
    inline const QString MakeProject      = u"make:project"_s;
    inline const QString MakeSeeder       = u"make:seeder"_s;
    inline const QString &Migrate         = Tom::Constants::NsMigrate;
    inline const QString MigrateFresh     = u"migrate:fresh"_s;
    inline const QString MigrateInstall   = u"migrate:install"_s;
    inline const QString MigrateRollback  = u"migrate:rollback"_s;
    inline const QString MigrateRefresh   = u"migrate:refresh"_s;
    inline const QString MigrateReset     = u"migrate:reset"_s;
    inline const QString MigrateStatus    = u"migrate:status"_s;
    inline const QString MigrateUninstall = u"migrate:uninstall"_s;
    inline const QString Integrate        = u"integrate"_s;

} // namespace Constants
} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_TOMCONSTANTS_INLINE_HPP
