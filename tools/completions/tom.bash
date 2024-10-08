# bash completion for tom                                -*- shell-script -*-

# Provide directory path completion for all commands with the --path= option
__tom_complete_dir()
{
    if __tom_guess_command "$tom_command" integrate make:migration make:model make:seeder &&
       [[ $prev == '--path' ]] && [[ $cargs -eq 2 || $cargs -eq 3 ]]
    then
        # Calls _comp_compgen_filedir -d
        _comp_compgen -a filedir -d
    fi
}

# Try to infer database connection names if a user is in the right folder and have tagged
# connection names with the '// shell:connection' comment
__tom_connections()
{
    local main_file="main.cpp"

    [[ -d database/migrations && -f "./$main_file" ]] || return

    # --quiet suppresses printing of the entire file output and |p only prints the replaced part \1
    local regex='s|.*"\([[:alnum:]_.-]\+\)".*// shell:connection$|\1|p'

    sed --quiet --expression="$regex" "$main_file"
}

# Environment names for the --env= option
__tom_environments()
{
    echo 'dev development local prod production test testing staging'
}

# Section names for the --only= option of the about command
__tom_about_sections()
{
    echo 'environment macros versions connections'
}

# Find the currently processed Tom command
__tom_command1()
{
    # Tom command positional argument position on the command-line
    local -i -r tom_command_position=1
    local -i i=-1
    local word

    for word in "${words[@]}"; do
        if [[ $word == -* ]]; then
            continue
        fi

        if (( ++i == tom_command_position )); then
            echo "$word"
            return 0
        fi
    done

    return 1
}
# Find the currently processed Tom command
__tom_command()
{
    local REPLY

    # Find the first positional argument (Tom command) excluding options and the current word!
    if _comp_get_first_arg; then
        echo "$REPLY"
        return 0
    fi

    # Return the current word under the cursor if it's not an option
    if [[ $cur != -* ]]; then
        echo "$cur"
        return 0
    fi

    return 1
}

# Try to guess one Tom command in the global namespace (without namespace eg. migrate)
__tom_guess_command_without_namespace()
{
    # Nothing to do, there can't be a match as the needle contains the : character
    if [[ $command_needle =~ : ]]; then
        return 1
    fi

    # Return 0 on success and 1 on failure
    return $([[ $command =~ ^$command_needle[a-z]*$ ]])
}

# Try to guess one namespaced Tom command (eg. migrate:status)
__tom_guess_command_with_namespace()
{
    # Nothing to do, there can't be a match as the needle doesn't contain the : character,
    # the result would be ambiguous it would match all commands.
    if [[ ! $command_needle =~ : ]]; then
        return 1
    fi

    local -a command_splitted
    local -a needle_splitted

    # Here we must be 100% sure both contain the : character to split properly,
    # $command =~ : is guaranteed by __tom_guess_command_wrapper so no check is needed
    # in the above if statement.
    _comp_split -F : command_splitted "$command"
    _comp_split -F : needle_splitted "$command_needle"

    # Special weird case is db: in this case the needle_splitted[1] will be empty and
    # it will match because of [a-z]*$ but it is desirable because we still get
    # --database=| connections completion triggered because of this. There are no other
    # cases where this can interfere. If this logic is not desirable, I will have to
    # change it, eg. add an ambiguous parameter to control this behavior, or even
    # better do the same as the complete:bash command and that's it to return this state
    # as kFound/kNotFound/kAmbiguous.

    # Return 0 on success and 1 on failure
    return $([[ ${command_splitted[0]} =~ ^${needle_splitted[0]}[a-z]*$ ]] &&
             [[ ${command_splitted[1]} =~ ^${needle_splitted[1]}[a-z]*$ ]])
}

# Try to guess one Tom command helper function (for nicer/simpler code logic)
__tom_guess_command_wrapper()
{
    # Supports the : character in the command name (command with/in the namespace)
    if [[ $command =~ : ]]; then
        return $(__tom_guess_command_with_namespace)
    fi

    return $(__tom_guess_command_without_namespace)
}

# Try to guess a Tom command with or without namespace
# Determine if any command in the commands array ($1) starts with a given command name ($2).
# Matches can be ambiguous as it will return 0 (success) on the first match and it doesn't
# distinguish whether the result is ambiguous or not.
# See the exhaustive comment in the __tom_guess_command_with_namespace function.
__tom_guess_command()
{
    local command_needle=$1

    # Nothing to do, the command needle is empty
    if [ "x${command_needle}" = "x" ]; then
        return 1
    fi

    shift
    # Nothing to do, there are no positional parameters left
    (($#)) || return 1
    local -a commands=("$@")
    local -i exit_code=1
    local command

    # Enable case-insensitve pattern matching
    local nocasematch_original=''
    if ! shopt -q nocasematch; then
        nocasematch_original='set'
        shopt -s nocasematch
    fi

    for command in "${commands[@]}"; do
        if __tom_guess_command_wrapper; then
            exit_code=$?
            break
        fi
    done

    # Restore the original nocasematch value
    [[ $nocasematch_original ]] && shopt -u nocasematch

    return $exit_code
}

#
__tom_get_carg()
{
    local -i carg=-1
    local -i cword_reverse=$cword
    local word

    for word in "${words[@]}"; do
        if [[ $word == -* ]]; then
            continue
        fi

        if (( ++carg, --cword_reverse < 0 )); then
            echo "$carg"
            return 0
        fi
    done

    return 1
}

# Main tab-completion logic for the tom command
_comp_cmd_tom()
{
    # Don't set local -i for cword, _comp_initialize internally calls unset -v cword anyway
    local cur prev words cword was_split comp_args

    # -n removes characters from list of word completion separators ($COMP_WORDBREAKS) but
    # it doesn't actually remove those characters from the $COMP_WORDBREAKS global variable,
    # instead it recomputes $words using the _comp__reassemble_words "$1" words cword.
    # -s implies -n = but that's not enough, the : must also be removed eg. for tom make:m|
    # For the : character to work properly with the set menu-complete-display-prefix on,
    # export COMP_WORDBREAKS=${COMP_WORDBREAKS//:} must also be added to ~/.bashrc. 🤔
    _comp_initialize -s -n : -- "$@" || return

    # Prevent ^M in the complete output on MSYS2/MinGW-w64
    if [ ! "x${MSYSTEM}" = "x" ]; then
        local IFS=$' \t\n\r'
    fi

    # Common options
    case $prev in
        # No completion after the following options
        --help | --version | -!(-*)[hV])
            return
            ;;
        # Complete environment names for the --env option
        --env)
            COMPREPLY=($(compgen -W "$(__tom_environments)" -- "$cur"))
            return
            ;;
    esac

    # Count the number of positional arguments excluding options
    # Default is 1 (for tom.exe |), it only counts positional arguments before the current word
    # excluding the current word!
    # See NOTES.txt[bash completion]
    local REPLY
    _comp_count_args
    local -i cargs=$REPLY

    echo "complete:bash --commandline=\"${words[*]}\" --word=\"$cur\" --cword=$cword" > ~/tmp/tom.txt
    echo "cargs: $cargs" >> ~/tmp/tom.txt
    # echo "cur '$cur' prev '$prev' words '${words[*]}' cword '$cword' was_split '$was_split' comp_args '${was_split[*]}'" > ~/tmp/a.txt
    # echo "'$IFS'" >> ~/tmp/a.txt

    # Find the currently processed Tom command
    local tom_command=$(__tom_command)
    local tom_command1=$(__tom_command1)
    echo "tom_command='$tom_command' '$tom_command1'" >> ~/tmp/tom.txt

    # Complete connection names
    local -a database_commands=(migrate db:seed db:wipe
        migrate:fresh migrate:install migrate:refresh migrate:reset migrate:rollback
        migrate:status migrate:uninstall)

    if [[ $prev == '--database' ]] &&
       [[ -v tom_command ]] && __tom_guess_command "$tom_command" "${database_commands[@]}"
    then
        COMPREPLY=($(compgen -W "$(__tom_connections)" -- "$cur"))
        return
    fi

    # __tom_get_carg
    # local -i carg=$REPLY

    # Complete section names for the --only= option of the about command
    if [[ -v tom_command ]] && [[ 'about' =~ ^$tom_command  ]] && [[ $prev == '--only' ]]; then
        COMPREPLY=($(compgen -W "$(__tom_about_sections)" -- "$cur"))
        return
    fi

    # Accurate/enhanced completion using the tom complete command
    if _comp_have_command tom; then
        # Completion for positional arguments and for long and short options
        if [[ ! $was_split && ($cargs -eq 1 || $cur == -*) ]] ||
            ([[ ! $was_split && -v tom_command && $cargs -eq 2 ]] &&
            __tom_guess_command "$tom_command" help list integrate)
        then
            COMPREPLY=($(compgen -W "$(tom complete:bash --commandline="${words[*]}" \
                 --word="$cur" --cword=$cword 2>/dev/null)" -- "$cur"))
echo "COMPREPLY: '${COMPREPLY[*]}'" >> ~/tmp/tom.txt
            return
        fi

        # Provide directory path completion for all commands with the --path= option
        __tom_complete_dir

        return
    fi

    # Inaccurate completion if the tom command is not on the system path, it doesn't
    # provide all options
    local commands="about env help inspire integrate list migrate db:seed db:wipe \
make:migration make:model make:seeder migrate:fresh migrate:install migrate:refresh \
migrate:reset migrate:rollback migrate:status migrate:uninstall"

    local namespaces='global db make migrate namespaced all'

    local common_options="--ansi --no-ansi --env= --help --no-interaction --quiet \
--version --verbose"

    local shells='bash pwsh zsh'

    # Complete positional arguments for tom commands
    if [[ -v tom_command && $cargs -eq 2 && $cur != -* ]]; then
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
        # echo "in2" >> ~/tmp/a.txt
        COMPREPLY=($(compgen -W "$commands" -- "$cur"))
    fi

    # Provide directory path completion for all commands with the --path= option
    __tom_complete_dir

} &&
    complete -F _comp_cmd_tom tom tom_testdata

# ex: filetype=sh
