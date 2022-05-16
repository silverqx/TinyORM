#pragma once
#ifndef TOM_TOMCONSTANTS_EXTERN_HPP
#define TOM_TOMCONSTANTS_EXTERN_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

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

    SHAREDLIB_EXPORT extern const QString migration_;
    SHAREDLIB_EXPORT extern const QString batch_;

    // Common options
    SHAREDLIB_EXPORT extern const QString ansi;
    SHAREDLIB_EXPORT extern const QString noansi;
    SHAREDLIB_EXPORT extern const QString env;
    SHAREDLIB_EXPORT extern const QString help;
    SHAREDLIB_EXPORT extern const QString nointeraction;
    SHAREDLIB_EXPORT extern const QString quiet;
    SHAREDLIB_EXPORT extern const QString version; // unused
    SHAREDLIB_EXPORT extern const QString verbose; // unused

    // Commands' positional options
    // help
    SHAREDLIB_EXPORT extern const QString command_name;
    // list
    SHAREDLIB_EXPORT extern const QString namespace_;

    // Commands' options
    // Used by more commands
    SHAREDLIB_EXPORT extern const QString force;
    SHAREDLIB_EXPORT extern const QString pretend;
    SHAREDLIB_EXPORT extern const QString seed;
    SHAREDLIB_EXPORT extern const QString seeder;
    SHAREDLIB_EXPORT extern const QString step_;
    // Default value names
    SHAREDLIB_EXPORT extern const QString class_up;
    SHAREDLIB_EXPORT extern const QString database_up;
    SHAREDLIB_EXPORT extern const QString seeder_up;
    SHAREDLIB_EXPORT extern const QString step_up;
    // list
    SHAREDLIB_EXPORT extern const QString raw_;
    // db:seed
    SHAREDLIB_EXPORT extern const QString class_;
    // db:wipe
    SHAREDLIB_EXPORT extern const QString drop_views;
    SHAREDLIB_EXPORT extern const QString drop_types;
    // make:migration
    SHAREDLIB_EXPORT extern const QString create_;
    SHAREDLIB_EXPORT extern const QString table_;
    SHAREDLIB_EXPORT extern const QString path_;
    SHAREDLIB_EXPORT extern const QString realpath_;
    SHAREDLIB_EXPORT extern const QString fullpath;
    // migrate:refresh
    SHAREDLIB_EXPORT extern const QString step_migrate;

    // Namespace names
    SHAREDLIB_EXPORT extern const QString NsGlobal;
    SHAREDLIB_EXPORT extern const QString NsDb;
    SHAREDLIB_EXPORT extern const QString NsMake;
    SHAREDLIB_EXPORT extern const QString NsMigrate;
    SHAREDLIB_EXPORT extern const QString NsNamespaced;

    // Command names
    SHAREDLIB_EXPORT extern const QString DbSeed;
    SHAREDLIB_EXPORT extern const QString DbWipe;
    SHAREDLIB_EXPORT extern const QString Inspire;
    SHAREDLIB_EXPORT extern const QString List;
    SHAREDLIB_EXPORT extern const QString &Env;
    SHAREDLIB_EXPORT extern const QString &Help;
    SHAREDLIB_EXPORT extern const QString MakeMigration;
    SHAREDLIB_EXPORT extern const QString MakeProject;
    SHAREDLIB_EXPORT extern const QString &Migrate;
    SHAREDLIB_EXPORT extern const QString MigrateFresh;
    SHAREDLIB_EXPORT extern const QString MigrateInstall;
    SHAREDLIB_EXPORT extern const QString MigrateRollback;
    SHAREDLIB_EXPORT extern const QString MigrateRefresh;
    SHAREDLIB_EXPORT extern const QString MigrateReset;
    SHAREDLIB_EXPORT extern const QString MigrateStatus;

} // namespace Tom::Constants

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_TOMCONSTANTS_EXTERN_HPP
