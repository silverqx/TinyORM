#pragma once
#ifndef TOM_TOMCONSTANTS_EXTERN_HPP
#define TOM_TOMCONSTANTS_EXTERN_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QString>

#include <orm/macros/commonnamespace.hpp>
#include <orm/macros/export.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

/*! Namespace constains common strings for the Tom namespace (migrations). */
namespace Tom::Constants
{

    /*! Migration files datetime prefix format. */
    SHAREDLIB_EXPORT extern const QString DateTimePrefix;

    // Common strings
    SHAREDLIB_EXPORT extern const QString LongOption;
    SHAREDLIB_EXPORT extern const QString LongOptionOnly;
    SHAREDLIB_EXPORT extern const QString LongOptionValue;
    SHAREDLIB_EXPORT extern const QString ShortOption;
    SHAREDLIB_EXPORT extern const QString DoubleDash;

    SHAREDLIB_EXPORT extern const QString migration_;
    SHAREDLIB_EXPORT extern const QString batch_;

    // Common options
    SHAREDLIB_EXPORT extern const QString ansi;
    SHAREDLIB_EXPORT extern const QString noansi;
    SHAREDLIB_EXPORT extern const QString env;
    SHAREDLIB_EXPORT extern const QString help;
    SHAREDLIB_EXPORT extern const QString nointeraction;
    SHAREDLIB_EXPORT extern const QString quiet;
    SHAREDLIB_EXPORT extern const QString verbose;

    // Commands' positional options
    // help
    SHAREDLIB_EXPORT extern const QString command_name;
    // list
    SHAREDLIB_EXPORT extern const QString namespace_;
    SHAREDLIB_EXPORT extern const QString shell_;

    // Commands' options
    // Used by more commands
    SHAREDLIB_EXPORT extern const QString force;
    SHAREDLIB_EXPORT extern const QString pretend;
    SHAREDLIB_EXPORT extern const QString seed;
    SHAREDLIB_EXPORT extern const QString seeder;
    SHAREDLIB_EXPORT extern const QString step_;
    SHAREDLIB_EXPORT extern const QString path_;
    // Default value names
    SHAREDLIB_EXPORT extern const QString env_up;
    SHAREDLIB_EXPORT extern const QString class_up;
    SHAREDLIB_EXPORT extern const QString database_up;
    SHAREDLIB_EXPORT extern const QString seeder_up;
    SHAREDLIB_EXPORT extern const QString batch_up;
    SHAREDLIB_EXPORT extern const QString step_up;
    SHAREDLIB_EXPORT extern const QString commandline_up;
    SHAREDLIB_EXPORT extern const QString position_up;
    SHAREDLIB_EXPORT extern const QString word_up;
    SHAREDLIB_EXPORT extern const QString cword_up;
    SHAREDLIB_EXPORT extern const QString path_up;
    SHAREDLIB_EXPORT extern const QString create_up;
    SHAREDLIB_EXPORT extern const QString table_up;
    SHAREDLIB_EXPORT extern const QString primary_key_up;
    SHAREDLIB_EXPORT extern const QString connection_up;
    SHAREDLIB_EXPORT extern const QString with_up;
    SHAREDLIB_EXPORT extern const QString fillable_up;
    SHAREDLIB_EXPORT extern const QString guarded_up;
    SHAREDLIB_EXPORT extern const QString dateformat_up;
    SHAREDLIB_EXPORT extern const QString dates_up;
    SHAREDLIB_EXPORT extern const QString touches_up;
    SHAREDLIB_EXPORT extern const QString one_to_one_up;
    SHAREDLIB_EXPORT extern const QString one_to_many_up;
    SHAREDLIB_EXPORT extern const QString belongs_to_up;
    SHAREDLIB_EXPORT extern const QString belongs_to_many_up;
    SHAREDLIB_EXPORT extern const QString pivot_up;
    SHAREDLIB_EXPORT extern const QString pivot_inverse_up;
    SHAREDLIB_EXPORT extern const QString as_up;
    SHAREDLIB_EXPORT extern const QString with_pivot_up;
    SHAREDLIB_EXPORT extern const QString foreign_key_up;
    SHAREDLIB_EXPORT extern const QString pivot_table_up;
    // complete
    SHAREDLIB_EXPORT extern const QString commandline;
    SHAREDLIB_EXPORT extern const QString position;
    SHAREDLIB_EXPORT extern const QString word_;
    SHAREDLIB_EXPORT extern const QString cword_;
    // list
    SHAREDLIB_EXPORT extern const QString raw_;
    // db:seed
    SHAREDLIB_EXPORT extern const QString class_;
    // db:wipe
    SHAREDLIB_EXPORT extern const QString drop_views;
    SHAREDLIB_EXPORT extern const QString drop_types;
    // make:migration, make:model, make:seeder
    SHAREDLIB_EXPORT extern const QString create_;
    SHAREDLIB_EXPORT extern const QString table_;
    SHAREDLIB_EXPORT extern const QString realpath_;
    SHAREDLIB_EXPORT extern const QString fullpath;
    SHAREDLIB_EXPORT extern const QString from_model;
    // make:model
    SHAREDLIB_EXPORT extern const QString one_to_one;
    SHAREDLIB_EXPORT extern const QString one_to_many;
    SHAREDLIB_EXPORT extern const QString belongs_to;
    SHAREDLIB_EXPORT extern const QString belongs_to_many;
    SHAREDLIB_EXPORT extern const QString foreign_key;
    SHAREDLIB_EXPORT extern const QString pivot_table;
    SHAREDLIB_EXPORT extern const QString primary_key;
    SHAREDLIB_EXPORT extern const QString incrementing;
    SHAREDLIB_EXPORT extern const QString disable_incrementing;
    SHAREDLIB_EXPORT extern const QString connection_;
    SHAREDLIB_EXPORT extern const QString with_;
    SHAREDLIB_EXPORT extern const QString fillable;
    SHAREDLIB_EXPORT extern const QString guarded;
    SHAREDLIB_EXPORT extern const QString disable_timestamps;
    SHAREDLIB_EXPORT extern const QString dateformat;
    SHAREDLIB_EXPORT extern const QString dates;
    SHAREDLIB_EXPORT extern const QString touches;
    SHAREDLIB_EXPORT extern const QString pivot_model;
    SHAREDLIB_EXPORT extern const QString pivot_;
    SHAREDLIB_EXPORT extern const QString pivot_inverse;
    SHAREDLIB_EXPORT extern const QString as_;
    SHAREDLIB_EXPORT extern const QString with_timestamps;
    SHAREDLIB_EXPORT extern const QString with_pivot;
    SHAREDLIB_EXPORT extern const QString preserve_order;
    // migrate:refresh
    SHAREDLIB_EXPORT extern const QString step_migrate;
    // migrate:status
    SHAREDLIB_EXPORT extern const QString pending_;
    // migrate:uninstall
    SHAREDLIB_EXPORT extern const QString reset;
    // integrate
    SHAREDLIB_EXPORT extern const QString stdout_;

    // Namespace names
    SHAREDLIB_EXPORT extern const QString NsGlobal;
    SHAREDLIB_EXPORT extern const QString NsDb;
    SHAREDLIB_EXPORT extern const QString NsMake;
    SHAREDLIB_EXPORT extern const QString NsMigrate;
    SHAREDLIB_EXPORT extern const QString NsNamespaced;
    SHAREDLIB_EXPORT extern const QString NsAll;

    // Shell names
    SHAREDLIB_EXPORT extern const QString ShBash;
    SHAREDLIB_EXPORT extern const QString ShPwsh;
    SHAREDLIB_EXPORT extern const QString ShZsh;

    // Command names
    SHAREDLIB_EXPORT extern const QString Complete;
    SHAREDLIB_EXPORT extern const QString DbSeed;
    SHAREDLIB_EXPORT extern const QString DbWipe;
    SHAREDLIB_EXPORT extern const QString Inspire;
    SHAREDLIB_EXPORT extern const QString List;
    SHAREDLIB_EXPORT extern const QString &Env;
    SHAREDLIB_EXPORT extern const QString &Help;
    SHAREDLIB_EXPORT extern const QString MakeMigration;
    SHAREDLIB_EXPORT extern const QString MakeModel;
    SHAREDLIB_EXPORT extern const QString MakeProject;
    SHAREDLIB_EXPORT extern const QString MakeSeeder;
    SHAREDLIB_EXPORT extern const QString &Migrate;
    SHAREDLIB_EXPORT extern const QString MigrateFresh;
    SHAREDLIB_EXPORT extern const QString MigrateInstall;
    SHAREDLIB_EXPORT extern const QString MigrateRollback;
    SHAREDLIB_EXPORT extern const QString MigrateRefresh;
    SHAREDLIB_EXPORT extern const QString MigrateReset;
    SHAREDLIB_EXPORT extern const QString MigrateStatus;
    SHAREDLIB_EXPORT extern const QString MigrateUninstall;
    SHAREDLIB_EXPORT extern const QString Integrate;

} // namespace Tom::Constants

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_TOMCONSTANTS_EXTERN_HPP
