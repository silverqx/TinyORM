#compdef tom tom_testdata

# Completion for the TinyORM tom application

__tom_commands() {
    local -a commands=(
        'about:Display basic information about the tom application'
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

__tom_about_sections() {
    _values -s , section 'connections' 'environment' 'macros' 'versions'
}

# Try to infer database connection names if a user is in the right folder and have tagged
# connection names with '// shell:connection' comment
__tom_connections() {
    local -a connections
    local -a lines
    # The documentation for the _values ​​completer function states that these variables should be
    # declared locally by the function that calls this _values completer
    local context state state_descr line
    typeset -A val_args

    [[ -d database/migrations && -f main.cpp ]] || return

    IFS=$'\n' lines=($(/bin/cat main.cpp | grep '// shell:connection'))

    # Nothing found
    [[ $#lines -eq 0 ]] && return

    regex='"([[:alnum:]_.-]+)".*// shell:connection$'

    for line in $lines; do
        if [[ $line =~ $regex ]]; then
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

    seeder_files=($(/bin/ls database/seeders/*seeder.hpp))

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
        '(: -)'{-h,--help}"[Display help for a given command, when no command is given display help for the list command]"
        '(-n --no-interaction)'{-n,--no-interaction}'[Do not ask any interactive question]'
        '(-q --quiet)'{-q,--quiet}'[Do not output any message]'
        '(: -)'{-V,--version}'[Display this application version]'
        '(-v --verbose)'{-v,--verbose}'[Increase verbosity of messages: 1 for normal output, 2 for more verbose output, 3 for debug]'
    )

    _arguments -C \
        $common_options \
        '(-)1: :__tom_commands' \
        '(-)*:: :->args' && return

    case $line[1] in
        about)
            _arguments \
                $common_options \
                '--json[Output the information as JSON]' \
                '--pretty[Enable JSON human readable output]' \
                '--only=[Sections to display (partial match)]:section names:__tom_about_sections'
            ;;

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
                '--path=[The directory location where the completion file is to be created (zsh only)]:folder path:_files -/'
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
                '--path=[The directory location where the migration file is to be created]:folder path:_files -/' \
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
                "--casts-example[Create the u_casts map example]" \
                "--snake-attributes[Enable snake_cased attributes during serialization (default)]" \
                "--disable-snake-attributes[Disable snake_cased attributes during serialization]" \
                '--visible=[The attributes that should be visible during serialization]:attribute names' \
                '--hidden=[The attributes that should be hidden during serialization]:attribute names' \
                '--accessors=[Create accessor methods (merged with appends)]:accessor names' \
                '--appends=[The attributes that should be appended during serialization]:attribute names' \
                '(-o --preserve-order)'{-o,--preserve-order}'[Preserve relations order defined on the command-line]' \
                '--path=[The directory location where the migration file is to be created]:folder path:_files -/' \
                '--realpath[Indicate any provided migration file paths are pre-resolved absolute paths]' \
                '--fullpath[Output the full path of the migration]' \
                '(-f --force)'{-f,--force}'[Overwrite the migration file if already exists]'
            ;;

        make:seeder)
            _arguments \
                $common_options \
                '1::class name:()' \
                '--path=[The directory location where the seeder file is to be created]:folder path:_files -/' \
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
                '--step=[The number of migrations to be reverted & re-run]:number' \
                '--batch=[The batch of migrations (identified by their batch number) to be reverted]:number'
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
