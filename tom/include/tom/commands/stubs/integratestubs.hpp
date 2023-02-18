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

Register-ArgumentCompleter -Native -CommandName tom,tom_testdata -ScriptBlock {
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

# Try to infer database connection names if a user is in the right folder and have tagged
# connection names with '// shell:connection' comment
__tom_connections() {
    declare -a connections
    declare -a lines

    [[ -d database/migrations && -f main.cpp ]] || return

    IFS=$'\n' lines=($(/bin/cat main.cpp | grep '// shell:connection'))

    # Nothing found
    [[ ${#lines[@]} -eq 0 ]] && return

    regex='.*"(\w+)".*// shell:connection'

    for line in "${lines[@]}"; do
        if [[ $line =~ $regex ]]; then
            connections[${#connections[@]}]=${BASH_REMATCH[1]}
        fi
    done

    echo "${connections[*]}"
}

__tom_environments() {
    echo 'dev development local prod production test testing staging'
}

_tom()
{
    local cur prev words cword split

    _init_completion -s || return

    COMP_WORDBREAKS=${COMP_WORDBREAKS//:}

    # No completion after the following options
    case $prev in
        --help | --version | -!(-*)[hV])
            return
            ;;
        --env)
            COMPREPLY=($(compgen -W "$(__tom_environments)" -- "$cur"))
            return
            ;;
    esac

    # Count the number of arguments excluding options
    # Default is 1 and it only count arguments before current word!
    # $1 = because without it it counts tom --env=dev as 3 instead of 1+
    local args
    _count_args =

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

    # Complete connection names
    if [[ -v tom_command ]] &&
       [[ $tom_command == 'migrate' || $tom_command =~ ^migrate: || $tom_command =~ ^db: ]] &&
       [[ $prev == '--database' ]]
    then
        COMPREPLY=($(compgen -W "$(__tom_connections)" -- "$cur"))
        return
    fi

    # Accurate completion using the tom complete command
    if _have tom; then
        # Completion for positional arguments and for long and short options
        if [[ $split == false && ($args -eq 1 || $cur == -*) ]] ||
           [[ $split == false &&
              (-v tom_command && $args -eq 2 && $tom_command == @(help|list|integrate)) ]]
        then
            COMPREPLY=($(compgen -W "$(tom complete --word="$cur" \
                --commandline="${words[*]}" --cword=$cword 2>/dev/null)" -- "$cur"))
            return
        fi

        # Provide file/dir completion for the following commands
        __tom_filedir "$tom_command" $args

        return
    fi

    # Inaccurate completion if the tom command is not on the system path, it doesn't
    # provide all options
    commands='env help inspire integrate list migrate db:seed db:wipe
        make:migration make:model make:seeder migrate:fresh migrate:install
        migrate:refresh migrate:reset migrate:rollback migrate:status
        migrate:uninstall'

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
    complete -F _tom tom tom_testdata

# ex: filetype=sh

)TTT";

/*! TinyORM tom zsh completion file content. */
inline const auto *const TomZshCompletionContent =
R"TTT(#compdef tom tom_testdata

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
        'make\:model:Create a new model class'
        'make\:seeder:Create a new seeder class'
        'migrate\:fresh:Drop all tables and re-run all migrations'
        'migrate\:install:Create the migration repository'
        'migrate\:refresh:Rollback and re-run all migrations'
        'migrate\:reset:Rollback all database migrations'
        'migrate\:rollback:Rollback the last database migration'
        'migrate\:status:Show the status of each migration'
        'migrate\:uninstall:Drop the migration repository with an optional reset'
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
# connection names with '// shell:connection' comment
__tom_connections() {
    local -a connections
    local -a lines

    [[ -d database/migrations && -f main.cpp ]] || return

    IFS=$'\n' lines=($(/bin/cat main.cpp | grep '// shell:connection'))

    # Nothing found
    [[ $#lines -eq 0 ]] && return

    for line in $lines; do
        if [[ $line =~ '.*"(\w+)".*// shell:connection' ]]; then
            connections+=$match[1]
        fi
    done

    _values -s , connection $connections
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
                '1::shell name:(bash pwsh zsh)' \
                '--stdout[Print content of the integrate command (instead of writing to disk)]' \
                '--path=[The location where the completion file should be created (zsh only)]:folder path:_files -/'
            ;;

        list)
            _arguments \
                $common_options \
                '1::namepace name:__tom_namespaces'
            ;;

        migrate)
            _arguments \
                $common_options \
                '--database=[The database connection to use]:connection:__tom_connections' \
                '(-f --force)'{-f,--force}'[Force the operation to run when in production]' \
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
                '(-f --force)'{-f,--force}'[Force the operation to run when in production]'
            ;;

        db:wipe)
            _arguments \
                $common_options \
                '--database=[The database connection to use]:connection:__tom_connections' \
                '--drop-views[Drop all tables and views]' \
                '--drop-types[Drop all tables and types (Postgres only)]' \
                '(-f --force)'{-f,--force}'[Force the operation to run when in production]'
            ;;

        make:migration)
            _arguments \
                $common_options \
                '1::migration name:()' \
                '--create=[The table to be created]:table name' \
                '--table=[The table to migrate]:table name' \
                '--path=[The location where the migration file should be created]:folder path:_files -/' \
                '--realpath[Indicate that any provided migration file paths are pre-resolved absolute paths]' \
                '--fullpath[Output the full path of the created migration]' \
                '(-f --force)'{-f,--force}'[Overwrite the model class if already exists]'
            ;;

        make:model)
            _arguments \
                $common_options \
                '1::class name:()' \
                '(-m --migration)'{-m,--migration}'[Create a new migration file for the model]' \
                '(-s --seeder)'{-s,--seeder}'[Create a new seeder for the model]' \
                '*--one-to-one=[Create one-to-one relation to the given model]:class name' \
                '*--one-to-many=[Create one-to-many relation to the given model]:class name' \
                '*--belongs-to=[Create belongs-to relation to the given model]:class name' \
                '*--belongs-to-many=[Create many-to-many relation to the given model]:class name' \
                '*--foreign-key=[The foreign key name]:column name' \
                '*--pivot-table=[The pivot table name]:table name' \
                '*--pivot=[The class name of the pivot class for the belongs-to-many relationship]:class name' \
                '*--pivot-inverse=[The class name of the pivot class for the belongs-to-many inverse relationship]:class name' \
                '*--as=[The name for the pivot relation]:relation name' \
                '*--with-pivot=[Extra attributes for the pivot model]:column names' \
                '*--with-timestamps[Pivot table with timestamps]' \
                '--table=[The table associated with the model]:table name' \
                '--primary-key=[The primary key associated with the table]:column name' \
                "--incrementing[Enable auto-incrementing for the model's primary key]" \
                "--disable-incrementing[Disable auto-incrementing for the model's primary key]" \
                '--connection=[The connection name for the model]:connection name' \
                '--with=[The relations to eager load on every query]:relationship names' \
                '--fillable=[The attributes that are mass assignable]:attribute names' \
                "--guarded=[The guarded attributes that aren't mass assignable]:attribute names" \
                '--disable-timestamps[Disable timestamping of the model]' \
                "--dateformat=[The storage format of the model's date columns]:date format" \
                '--dates=[The attributes that should be mutated to dates]:attribute names' \
                '--touches=[All of the relationships to be touched]:relationship names' \
                '(-o --preserve-order)'{-o,--preserve-order}'[Preserve relations order defined on the command-line]' \
                '--path=[The location where the migration file should be created]:folder path:_files -/' \
                '--realpath[Indicate any provided migration file paths are pre-resolved absolute paths]' \
                '--fullpath[Output the full path of the migration]' \
                '(-f --force)'{-f,--force}'[Overwrite the migration file if already exists]'
            ;;

        make:seeder)
            _arguments \
                $common_options \
                '1::class name:()' \
                '--path=[The location where the seeder file should be created]:folder path:_files -/' \
                '--realpath[Indicate that any provided seeder file paths are pre-resolved absolute paths]' \
                '--fullpath[Output the full path of the created seeder]' \
                '(-f --force)'{-f,--force}'[Overwrite the seeder class if already exists]'
            ;;

        migrate:fresh)
            _arguments \
                $common_options \
                '--database=[The database connection to use]:connection:__tom_connections' \
                '--drop-views[Drop all tables and views]' \
                '--drop-types[Drop all tables and types (Postgres only)]' \
                '(-f --force)'{-f,--force}'[Force the operation to run when in production]' \
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
                '(-f --force)'{-f,--force}'[Force the operation to run when in production]' \
                '--seed[Indicates if the seed task should be re-run]' \
                '--seeder=[The class name of the root seeder]:class name:__tom_seeders' \
                '--step=[The number of migrations to be reverted & re-run]:number' \
                '--step-migrate[Force the migrations to be run so they can be rolled back individually]'
            ;;

        migrate:reset)
            _arguments \
                $common_options \
                '--database=[The database connection to use]:connection:__tom_connections' \
                '(-f --force)'{-f,--force}'[Force the operation to run when in production]' \
                '--pretend[Dump the SQL queries that would be run]'
            ;;

        migrate:rollback)
            _arguments \
                $common_options \
                '--database=[The database connection to use]:connection:__tom_connections' \
                '(-f --force)'{-f,--force}'[Force the operation to run when in production]' \
                '--pretend[Dump the SQL queries that would be run]' \
                '--step=[The number of migrations to be reverted & re-run]:number'
            ;;

        migrate:status)
            _arguments \
                $common_options \
                '--database=[The database connection to use]:connection:__tom_connections' \
                '--pending[Only list pending migrations]'
            ;;

        migrate:uninstall)
            _arguments \
                $common_options \
                '--database=[The database connection to use]:connection:__tom_connections' \
                '--force[Force the operation to run when in production]' \
                '--pretend[Dump the SQL queries that would be run]'
            ;;
    esac
}

)TTT";
#endif

} // namespace Tom::Commands::Stubs

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_STUBS_INTEGRATESTUBS_HPP
