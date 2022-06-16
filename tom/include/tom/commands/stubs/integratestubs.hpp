#pragma once
#ifndef TOM_COMMANDS_STUBS_INTEGRATESTUBS_HPP
#define TOM_COMMANDS_STUBS_INTEGRATESTUBS_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Stubs
{

    /*! Register-ArgumentCompleter command content. */
    inline const auto *const RegisterArgumentCompleter =
R"(
# TinyORM tom tab-completion
# ---

Register-ArgumentCompleter -Native -CommandName tom -ScriptBlock {
    param($wordToComplete, $commandAst, $cursorPosition)
    [Console]::InputEncoding = [Console]::OutputEncoding = $OutputEncoding = [System.Text.Utf8Encoding]::new()
    $Local:word = $wordToComplete.Replace('"', '""')
    $Local:ast = $commandAst.ToString().Replace('"', '""')
    tom complete --word="$Local:word" --commandline="$Local:ast" --position=$cursorPosition
        | ForEach-Object {
            $completionText, $listText, $toolTip = $_ -split ';', 3
            $listText ??= $completionText
            $toolTip ??= $completionText
            [System.Management.Automation.CompletionResult]::new(
                $completionText, $listText, 'ParameterValue', $toolTip)
        }
}

)";

#if defined(__linux__) || defined(__MINGW32__)
/*! TinyORM tom bash completion file content. */
inline const auto *const TomBashCompletionContent =
R"TTT(# bash completion for tom                                -*- shell-script -*-

# Provide file/dir completion for the following commands
__tom_filedir()
{
    if [[ ${1-} == @(make:migration|make:seeder) && ${2-} -eq 2 ]]; then
        _filedir
    fi
}

_tom()
{
    local cur prev words cword

    _init_completion || return

    COMP_WORDBREAKS=${COMP_WORDBREAKS//:}

    # No completion after the following options
    case $prev in
        --help | --version | -!(-*)[hV])
            return
            ;;
    esac

    # Count the number of arguments excluding options
    # Default is 1 and it only count arguments before current word!
    local args
    _count_args

    # Find currently active tom command
    local tom_command i
    for ((i = 1; i < ${#words[@]}; i++)); do
        if [[ ${words[i]} != -* ]]; then
            # Command found but is actually completing now
            if [[ $i -ne $cword ]]; then
                tom_command=${words[i]}
            fi
            break
        fi
    done

    # Accurate completion using the tom complete command
    if _have tom; then
        # Completion for positional arguments and for long and short options
        if [[ $args -eq 1 || $cur == -* ||
            (-v tom_command && $args -eq 2 && $tom_command == @(help|list|integrate)) ]];
        then
            COMPREPLY=($(compgen -W "$(tom complete --word="$cur" \
                --commandline="${words[*]}" --cword=$cword 2>/dev/null)" -- "$cur"))
        fi

        # Provide file/dir completion for the following commands
        __tom_filedir "$tom_command" $args

        return
    fi

    # Inaccurate completion if the tom command is not on the system path, it doesn't
    # provide all options
    commands='env help inspire integrate list migrate db:seed db:wipe
        make:migration make:seeder migrate:fresh migrate:install
        migrate:refresh migrate:reset migrate:rollback migrate:status'

    namespaces='global db make migrate namespaced all'

    common_options='--ansi --no-ansi --env= --help --no-interaction --quiet
        --version --verbose'

    shells='bash pwsh zsh'

    # Complete positional arguments for tom commands
    if [[ -v tom_command && $args -eq 2 && $cur != -* ]]; then
        case $tom_command in
            integrate)
                COMPREPLY=($(compgen -W "$shells" -- "$cur"))
                return
                ;;
            help)
                COMPREPLY=($(compgen -W "$commands" -- "$cur"))
                return
                ;;
            list)
                COMPREPLY=($(compgen -W "$namespaces" -- "$cur"))
                return
                ;;
        esac
    fi

    # Complete long and short options
    if [[ $cur == -* ]]; then
        COMPREPLY=($(compgen -W "$common_options" -- "$cur"))
        return
    fi

    # Complete tom commands
    if [[ ! -v tom_command && $cur != -* ]]; then
        COMPREPLY=($(compgen -W "$commands" -- "$cur"))
    fi

    # Provide file/dir completion for the following commands
    __tom_filedir $tom_command $args

} &&
    complete -F _tom tom

# ex: filetype=sh

)TTT";

/*! TinyORM tom zsh completion file content. */
inline const auto *const TomZshCompletionContent =
R"TTT(#compdef tom

# Completion for the TinyORM tom application

__tom_commands() {
    local -a commands=(
        'env:Display the current framework environment'
        'help:Display help for a command'
        'inspire:Display an inspiring quote'
        'integrate:Enable tab-completion for the given shell'
        'list:List commands'
        'migrate:Run the database migrations'
        'db\:seed:Seed the database with records'
        'db\:wipe:Drop all tables, views, and types'
        'make\:migration:Create a new migration file'
        'make\:seeder:Create a new seeder class'
        'migrate\:fresh:Drop all tables and re-run all migrations'
        'migrate\:install:Create the migration repository'
        'migrate\:refresh:Rollback and re-run all migrations'
        'migrate\:reset:Rollback all database migrations'
        'migrate\:rollback:Rollback the last database migration'
        'migrate\:status:Show the status of each migration'
    )

    _describe -t commands command commands
}

__tom_environments() {
    _values environment 'dev' 'development' 'local' 'prod' 'production' 'test' 'testing' 'staging'
}

__tom_namespaces() {
    _values namespace 'global' 'db' 'make' 'migrate' 'namespaced' 'all'
}

# Try to infer database connection names if a user is in the right folder and have tagged
# connection names with '// zsh:connection' comment
__tom_connections() {
    local -a connections
    local -a lines

    [[ -d database/migrations && -f main.cpp ]] || return

    IFS=$'\n' lines=($(/bin/cat main.cpp | grep '// zsh:connection'))

    # Nothing found
    [[ $#lines -eq 0 ]] && return

    for line in $lines; do
        if [[ $line =~ '.*"(\w+)".*// zsh:connection' ]]; then
            connections+=$match[1]
        fi
    done

    _values connection $connections
}

# Try to infer seeder class names if a user is in the right folder
__tom_seeders() {
    local namespace seeder namespace_grep seeder_grep content
    local -a seeders
    local -a seeder_files

    [[ -d database/seeders && -f main.cpp ]] || return

    seeder_files=($(/bin/ls database/seeders/*seeder.hpp)) #*/ msvc compile bug workaround

    # Nothing found
    [[ $#seeder_files -eq 0 ]] && return

    for seeder_file in $seeder_files; do
        content=$(/bin/cat $seeder_file)

        IFS=$'\n' namespace_grep=($(echo $content | grep -E '^ *namespace *\w+' -))
        IFS=$'\n' seeder_grep=($(echo $content | grep -E -z -o " *(struct|class) *\w+\
( *final)?\s*:(\s*(public|private|protected))?\s*Seeder" -))

        # Nothing found
        [[ $#seeder_grep -eq 0 ]] && continue

        # Get a namespace and seeder class name
        if [[ $#namespace_grep -ne 0 ]] &&
           [[ $namespace_grep[1] =~ '\b(namespace) *(\w+)\b' ]]
        then
            namespace=$match[2]
        fi

        if [[ $seeder_grep[1] =~ '\b(struct|class) *(\w+)\b' ]]; then
            seeder=$match[2]
        fi

        # Nothing found
        [[ -z $seeder ]] && continue

        # tom prepends the Seeders namespace automatically if not present
        if [[ -n $namespace && $namespace != 'Seeders' ]]; then
            seeder="$namespace\:\:$seeder"
        fi

        seeders+=$seeder
    done

    # Nothing to complete
    [[ $#seeders -eq 0 ]] && return

    _values seeder $seeders
}

_tom() {
    local line

    local -a common_options=(
        '--ansi[Force ANSI output]'
        '--no-ansi[Disable ANSI output]'
        '--env=[The environment the command should run under]: :__tom_environments'
        '(: -)'{-h,--help}"[Display help for the given command. When no command is given display help for the list command]"
        '(-n --no-interaction)'{-n,--no-interaction}'[Do not ask any interactive question]'
        '(-q --quiet)'{-q,--quiet}'[Do not output any message]'
        '(: -)'{-V,--version}'[Display this application version]'
        '(-v --verbose)'{-v,--verbose}'[Increase the verbosity of messages: 1 for normal output, 2 for more verbose output and 3 for debug]'
    )

    _arguments -C \
        $common_options \
        '(-)1: :__tom_commands' \
        '(-)*:: :->args' && return

    case $line[1] in
        (env|inspire)
            _arguments $common_options
            ;;

        help)
            _arguments \
                $common_options \
                '1:: :__tom_commands'
            ;;

        integrate)
            _arguments \
                $common_options \
                '1::shell:(bash pwsh zsh)' \
                '--stdout[Print content of the integrate command (instead of writing to disk)]' \
                '--path=[The location where the completion file should be created (zsh only)]:folder path:_files -/'
            ;;

        list)
            _arguments \
                $common_options \
                '1::namepace:__tom_namespaces'
            ;;

        migrate)
            _arguments \
                $common_options \
                '--database=[The database connection to use]:connection:__tom_connections' \
                '--force[Force the operation to run when in production]' \
                '--pretend[Dump the SQL queries that would be run]' \
                '--seed[Indicates if the seed task should be re-run]' \
                '--step[Force the migrations to be run so they can be rolled back individually]'
            ;;

        db:seed)
            _arguments \
                $common_options \
                '(--class)1::class name:__tom_seeders' \
                '(1)--class=[The class name of the root seeder \[default: "Seeders::DatabaseSeeder"\]]:class name:__tom_seeders' \
                '--database=[The database connection to use]:connection:__tom_connections' \
                '--force[Force the operation to run when in production]'
            ;;

        db:wipe)
            _arguments \
                $common_options \
                '--database=[The database connection to use]:connection:__tom_connections' \
                '--drop-views[Drop all tables and views]' \
                '--drop-types[Drop all tables and types (Postgres only)]' \
                '--force[Force the operation to run when in production]'
            ;;

        make:migration)
            _arguments \
                $common_options \
                '1::migration:()' \
                '--create=[The table to be created]:table name' \
                '--table=[The table to migrate]:table name' \
                '--path=[The location where the migration file should be created]:folder path:_files -/' \
                '--realpath[Indicate any provided migration file paths are pre-resolved absolute paths]' \
                '--fullpath[Output the full path of the migration]'
            ;;

        make:seeder)
            _arguments \
                $common_options \
                '1::class name:()' \
                '--path=[The location where the migration file should be created]:folder path:_files -/' \
                '--realpath[Indicate any provided migration file paths are pre-resolved absolute paths]' \
                '--fullpath[Output the full path of the migration]'
            ;;

        migrate:fresh)
            _arguments \
                $common_options \
                '--database=[The database connection to use]:connection:__tom_connections' \
                '--drop-views[Drop all tables and views]' \
                '--drop-types[Drop all tables and types (Postgres only)]' \
                '--force[Force the operation to run when in production]' \
                '--seed[Indicates if the seed task should be re-run]' \
                '--seeder=[The class name of the root seeder]:class name:__tom_seeders' \
                '--step[Force the migrations to be run so they can be rolled back individually]'
            ;;

        migrate:install)
            _arguments \
                $common_options \
                '--database=[The database connection to use]:connection:__tom_connections'
            ;;

        migrate:refresh)
            _arguments \
                $common_options \
                '--database=[The database connection to use]:connection:__tom_connections' \
                '--force[Force the operation to run when in production]' \
                '--seed[Indicates if the seed task should be re-run]' \
                '--seeder=[The class name of the root seeder]:class name:__tom_seeders' \
                '--step=[The number of migrations to be reverted & re-run]:number' \
                '--step-migrate[Force the migrations to be run so they can be rolled back individually]'
            ;;

        migrate:reset)
            _arguments \
                $common_options \
                '--database=[The database connection to use]:connection:__tom_connections' \
                '--force[Force the operation to run when in production]' \
                '--pretend[Dump the SQL queries that would be run]'
            ;;

        migrate:rollback)
            _arguments \
                $common_options \
                '--database=[The database connection to use]:connection:__tom_connections' \
                '--force[Force the operation to run when in production]' \
                '--pretend[Dump the SQL queries that would be run]' \
                '--step=[The number of migrations to be reverted & re-run]:number'
            ;;

        migrate:status)
            _arguments \
                $common_options \
                '--database=[The database connection to use]:connection:__tom_connections'
            ;;
    esac
}

)TTT";
#endif

} // namespace Tom::Commands::Stubs

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_STUBS_INTEGRATESTUBS_HPP
