#pragma once
#ifndef TOM_TOMCONSTANTS_INLINE_HPP
#define TOM_TOMCONSTANTS_INLINE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>

#include "orm/macros/commonnamespace.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

/*! Namespace constains common strings for the Tom namespace (migrations). */
namespace Tom::Constants
{

    /*! Migration files datetime prefix format. */
    inline const QString DateTimePrefix  = QStringLiteral("yyyy_MM_dd_HHmmss");

    // Common strings
    inline const QString LongOption      = QStringLiteral("--%1");
    inline const QString LongOptionOnly  = QStringLiteral("    %1");
    inline const QString LongOptionValue = QStringLiteral("--%1[=%2]");
    inline const QString ShortOption     = QStringLiteral("-%1");
    inline const QString DoubleDash      = QStringLiteral("--");

    inline const QString migration_  = QStringLiteral("migration");
    inline const QString batch_      = QStringLiteral("batch");

    // Common options
    inline const QString ansi          = QStringLiteral("ansi");
    inline const QString noansi        = QStringLiteral("no-ansi");
    inline const QString env           = QStringLiteral("env");
    inline const QString help          = QStringLiteral("help");
    inline const QString nointeraction = QStringLiteral("no-interaction");
    inline const QString quiet         = QStringLiteral("quiet");
    inline const QString version       = QStringLiteral("version"); // unused
    inline const QString verbose       = QStringLiteral("verbose"); // unused

    // Commands' positional options
    // help
    inline const QString command_name = QStringLiteral("command_name");
    // list
    inline const QString namespace_   = QStringLiteral("namespace");
    inline const QString shell_       = QStringLiteral("shell");

    // Commands' options
    // Used by more commands
    inline const QString force        = QStringLiteral("force");
    inline const QString pretend      = QStringLiteral("pretend");
    inline const QString seed         = QStringLiteral("seed");
    inline const QString seeder       = QStringLiteral("seeder");
    inline const QString step_        = QStringLiteral("step");
    // Default value names
    inline const QString class_up       = QStringLiteral("CLASS");
    inline const QString database_up    = QStringLiteral("DATABASE");
    inline const QString seeder_up      = QStringLiteral("SEEDER");
    inline const QString step_up        = QStringLiteral("STEP");
    inline const QString commandline_up = QStringLiteral("COMMANDLINE");
    inline const QString position_up    = QStringLiteral("POSITION");
    inline const QString word_up        = QStringLiteral("WORD");
    inline const QString cword_up       = QStringLiteral("CWORD");
    // complete
    inline const QString commandline  = QStringLiteral("commandline");
    inline const QString position     = QStringLiteral("position");
    inline const QString word_        = QStringLiteral("word");
    inline const QString cword_       = QStringLiteral("cword");
    // list
    inline const QString raw_         = QStringLiteral("raw");
    // db:seed
    inline const QString class_       = QStringLiteral("class");
    // db:wipe
    inline const QString drop_views   = QStringLiteral("drop-views");
    inline const QString drop_types   = QStringLiteral("drop-types");
    // make:migration
    inline const QString create_      = QStringLiteral("create");
    inline const QString table_       = QStringLiteral("table");
    inline const QString path_        = QStringLiteral("path");
    inline const QString realpath_    = QStringLiteral("realpath");
    inline const QString fullpath     = QStringLiteral("fullpath");
    // migrate:refresh
    inline const QString step_migrate = QStringLiteral("step-migrate");

    // Namespace names
    inline const QString NsGlobal     = QStringLiteral("global");
    inline const QString NsDb         = QStringLiteral("db");
    inline const QString NsMake       = QStringLiteral("make");
    inline const QString NsMigrate    = QStringLiteral("migrate");
    inline const QString NsNamespaced = QStringLiteral("namespaced");
    inline const QString NsAll        = QStringLiteral("all");

    // Shell names
    inline const QString ShBash       = QStringLiteral("bash");
    inline const QString ShPwsh       = QStringLiteral("pwsh");
    inline const QString ShZsh        = QStringLiteral("zsh");

    // Command names
    inline const QString Complete        = QStringLiteral("complete");
    inline const QString DbSeed          = QStringLiteral("db:seed");
    inline const QString DbWipe          = QStringLiteral("db:wipe");
    inline const QString Inspire         = QStringLiteral("inspire");
    inline const QString List            = QStringLiteral("list");
    inline const QString &Env            = Tom::Constants::env;
    inline const QString &Help           = Tom::Constants::help;
    inline const QString MakeMigration   = QStringLiteral("make:migration");
    inline const QString MakeProject     = QStringLiteral("make:project");
    inline const QString MakeSeeder      = QStringLiteral("make:seeder");
    inline const QString &Migrate        = Tom::Constants::NsMigrate;
    inline const QString MigrateFresh    = QStringLiteral("migrate:fresh");
    inline const QString MigrateInstall  = QStringLiteral("migrate:install");
    inline const QString MigrateRollback = QStringLiteral("migrate:rollback");
    inline const QString MigrateRefresh  = QStringLiteral("migrate:refresh");
    inline const QString MigrateReset    = QStringLiteral("migrate:reset");
    inline const QString MigrateStatus   = QStringLiteral("migrate:status");
    inline const QString Integrate       = QStringLiteral("integrate");

} // namespace Tom::Constants

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_TOMCONSTANTS_INLINE_HPP
