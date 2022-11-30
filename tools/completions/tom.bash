# bash completion for tom                                -*- shell-script -*-

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
