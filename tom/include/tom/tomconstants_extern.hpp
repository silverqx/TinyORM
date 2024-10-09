#pragma once
#ifndef TOM_TOMCONSTANTS_EXTERN_HPP
#define TOM_TOMCONSTANTS_EXTERN_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QString>

#include <orm/macros/commonnamespace.hpp>
#include <orm/macros/export.hpp>

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
    TINYORM_EXPORT extern const QString DateTimePrefix;

    // Common chars - QChar(u'') is faster than ''_L1
    TINYORM_EXPORT extern const QChar EQ_C;

    // Common strings
    TINYORM_EXPORT extern const QString EMPTY;
    TINYORM_EXPORT extern const QString LongOption;
    TINYORM_EXPORT extern const QString LongOptionEq;
    TINYORM_EXPORT extern const QString LongOptionOnly;
    TINYORM_EXPORT extern const QString LongOptionValue;
    TINYORM_EXPORT extern const QString ShortOption;
    TINYORM_EXPORT extern const QString DoubleDash;

    TINYORM_EXPORT extern const QString migration_;
    TINYORM_EXPORT extern const QString batch_;

    // Templates
    TINYORM_EXPORT extern const QString TMPL_RESULT2;
    TINYORM_EXPORT extern const QString TMPL_RESULT3;

    // Common options
    TINYORM_EXPORT extern const QString ansi;
    TINYORM_EXPORT extern const QString noansi;
    TINYORM_EXPORT extern const QString env;
    TINYORM_EXPORT extern const QString help;
    TINYORM_EXPORT extern const QString nointeraction;
    TINYORM_EXPORT extern const QString quiet;
    TINYORM_EXPORT extern const QString verbose;

    // Commands' positional options
    // help
    TINYORM_EXPORT extern const QString command_name;
    // list
    TINYORM_EXPORT extern const QString namespace_;
    // integrate
    TINYORM_EXPORT extern const QString shell_;

    // Commands' options
    // Used by more commands
    TINYORM_EXPORT extern const QString force;
    TINYORM_EXPORT extern const QString pretend;
    TINYORM_EXPORT extern const QString seed;
    TINYORM_EXPORT extern const QString seeder;
    TINYORM_EXPORT extern const QString step_;
    TINYORM_EXPORT extern const QString path_;
    TINYORM_EXPORT extern const QString json_;
    TINYORM_EXPORT extern const QString pretty;
    TINYORM_EXPORT extern const QString only_;
    TINYORM_EXPORT extern const QString database_;
    // Default value names
    TINYORM_EXPORT extern const QString env_up;
    TINYORM_EXPORT extern const QString class_up;
    TINYORM_EXPORT extern const QString database_up;
    TINYORM_EXPORT extern const QString seeder_up;
    TINYORM_EXPORT extern const QString batch_up;
    TINYORM_EXPORT extern const QString step_up;
    TINYORM_EXPORT extern const QString commandline_up;
    TINYORM_EXPORT extern const QString position_up;
    TINYORM_EXPORT extern const QString word_up;
    TINYORM_EXPORT extern const QString cargs_up;
    TINYORM_EXPORT extern const QString path_up;
    TINYORM_EXPORT extern const QString create_up;
    TINYORM_EXPORT extern const QString table_up;
    TINYORM_EXPORT extern const QString primary_key_up;
    TINYORM_EXPORT extern const QString connection_up;
    TINYORM_EXPORT extern const QString with_up;
    TINYORM_EXPORT extern const QString fillable_up;
    TINYORM_EXPORT extern const QString guarded_up;
    TINYORM_EXPORT extern const QString dateformat_up;
    TINYORM_EXPORT extern const QString dates_up;
    TINYORM_EXPORT extern const QString touches_up;
    TINYORM_EXPORT extern const QString one_to_one_up;
    TINYORM_EXPORT extern const QString one_to_many_up;
    TINYORM_EXPORT extern const QString belongs_to_up;
    TINYORM_EXPORT extern const QString belongs_to_many_up;
    TINYORM_EXPORT extern const QString pivot_up;
    TINYORM_EXPORT extern const QString pivot_inverse_up;
    TINYORM_EXPORT extern const QString as_up;
    TINYORM_EXPORT extern const QString with_pivot_up;
    TINYORM_EXPORT extern const QString foreign_key_up;
    TINYORM_EXPORT extern const QString pivot_table_up;
    TINYORM_EXPORT extern const QString visible_up;
    TINYORM_EXPORT extern const QString hidden_up;
    TINYORM_EXPORT extern const QString accessors_up;
    TINYORM_EXPORT extern const QString appends_up;
    TINYORM_EXPORT extern const QString only_up;
    TINYORM_EXPORT extern const QString shell_up;
    // complete
    TINYORM_EXPORT extern const QString commandline;
    TINYORM_EXPORT extern const QString position_;
    TINYORM_EXPORT extern const QString word_;
    TINYORM_EXPORT extern const QString cargs_;
    // list
    TINYORM_EXPORT extern const QString raw_;
    // db:seed
    TINYORM_EXPORT extern const QString class_;
    // db:wipe
    TINYORM_EXPORT extern const QString drop_views;
    TINYORM_EXPORT extern const QString drop_types;
    // make:migration, make:model, make:seeder
    TINYORM_EXPORT extern const QString create_;
    TINYORM_EXPORT extern const QString table_;
    TINYORM_EXPORT extern const QString realpath_;
    TINYORM_EXPORT extern const QString fullpath;
    TINYORM_EXPORT extern const QString from_model;
    // make:model
    TINYORM_EXPORT extern const QString one_to_one;
    TINYORM_EXPORT extern const QString one_to_many;
    TINYORM_EXPORT extern const QString belongs_to;
    TINYORM_EXPORT extern const QString belongs_to_many;
    TINYORM_EXPORT extern const QString foreign_key;
    TINYORM_EXPORT extern const QString pivot_table;
    TINYORM_EXPORT extern const QString primary_key;
    TINYORM_EXPORT extern const QString incrementing;
    TINYORM_EXPORT extern const QString disable_incrementing;
    TINYORM_EXPORT extern const QString connection_;
    TINYORM_EXPORT extern const QString with_;
    TINYORM_EXPORT extern const QString fillable;
    TINYORM_EXPORT extern const QString guarded;
    TINYORM_EXPORT extern const QString disable_timestamps;
    TINYORM_EXPORT extern const QString dateformat;
    TINYORM_EXPORT extern const QString dates;
    TINYORM_EXPORT extern const QString touches;
    TINYORM_EXPORT extern const QString pivot_model;
    TINYORM_EXPORT extern const QString pivot_inverse;
    TINYORM_EXPORT extern const QString as_;
    TINYORM_EXPORT extern const QString with_timestamps;
    TINYORM_EXPORT extern const QString with_pivot;
    TINYORM_EXPORT extern const QString preserve_order;
    TINYORM_EXPORT extern const QString snake_attributes;
    TINYORM_EXPORT extern const QString disable_snake_attributes;
    TINYORM_EXPORT extern const QString casts_example;
    TINYORM_EXPORT extern const QString visible;
    TINYORM_EXPORT extern const QString hidden;
    TINYORM_EXPORT extern const QString accessors;
    TINYORM_EXPORT extern const QString appends;
    // migrate:refresh
    TINYORM_EXPORT extern const QString step_migrate;
    // migrate:status
    TINYORM_EXPORT extern const QString pending_;
    // migrate:uninstall
    TINYORM_EXPORT extern const QString reset;
    // integrate
    TINYORM_EXPORT extern const QString stdout_;

    // Namespace names
    TINYORM_EXPORT extern const QString NsGlobal;
    TINYORM_EXPORT extern const QString NsComplete;
    TINYORM_EXPORT extern const QString NsDb;
    TINYORM_EXPORT extern const QString NsMake;
    TINYORM_EXPORT extern const QString NsMigrate;
    TINYORM_EXPORT extern const QString NsNamespaced;
    TINYORM_EXPORT extern const QString NsAll;

    // Shell names
    TINYORM_EXPORT extern const QString ShBash;
    TINYORM_EXPORT extern const QString ShPwsh;
    TINYORM_EXPORT extern const QString ShZsh;

    // Command names
    TINYORM_EXPORT extern const QString About;
    TINYORM_EXPORT extern const QString CompleteBash;
    TINYORM_EXPORT extern const QString CompletePwsh;
    TINYORM_EXPORT extern const QString DbSeed;
    TINYORM_EXPORT extern const QString DbWipe;
    TINYORM_EXPORT extern const QString &Env;
    TINYORM_EXPORT extern const QString &Help;
    TINYORM_EXPORT extern const QString Inspire;
    TINYORM_EXPORT extern const QString Integrate;
    TINYORM_EXPORT extern const QString List;
    TINYORM_EXPORT extern const QString MakeMigration;
    TINYORM_EXPORT extern const QString MakeModel;
//    TINYORM_EXPORT extern const QString MakeProject;
    TINYORM_EXPORT extern const QString MakeSeeder;
    TINYORM_EXPORT extern const QString &Migrate;
    TINYORM_EXPORT extern const QString MigrateFresh;
    TINYORM_EXPORT extern const QString MigrateInstall;
    TINYORM_EXPORT extern const QString MigrateRefresh;
    TINYORM_EXPORT extern const QString MigrateReset;
    TINYORM_EXPORT extern const QString MigrateRollback;
    TINYORM_EXPORT extern const QString MigrateStatus;
    TINYORM_EXPORT extern const QString MigrateUninstall;

} // namespace Constants
} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_TOMCONSTANTS_EXTERN_HPP
