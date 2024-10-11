# bash completion for tom                                  -*- shell-script -*-

# Basic completion functions
# ---

# Basic completion - initialize variables
__tom_basic_initialize()
{
    tom_commands="about env help inspire integrate list migrate db:seed db:wipe \
make:migration make:model make:seeder migrate:fresh migrate:install migrate:refresh \
migrate:reset migrate:rollback migrate:status migrate:uninstall"

    namespaces='global db make migrate namespaced all'

    common_long_options="--ansi --no-ansi --env= --help --no-interaction --quiet \
--version --verbose"

    common_short_options='-h -n -q -V -v -vv -vvv'

    shells='bash pwsh zsh'
}

# Basic completion - complete positional arguments for Tom commands (integrate, help, list)
__tom_basic_complete_positional_arguments()
{
    # Nothing to do
    ! [[ ! $was_split && $tom_command && $cur != -* && $cargs -eq 2 ]] && return 1

    if __tom_guess_command_without_namespace "$tom_command" 'integrate'; then
        __tom_compgen -- "$shells"
        return 0
    fi

    if __tom_guess_command_without_namespace "$tom_command" 'help'; then
        __tom_compgen -- "$tom_commands"
        return 0
    fi

    if __tom_guess_command_without_namespace "$tom_command" 'list'; then
        __tom_compgen -- "$namespaces"
        return 0
    fi

    return 1
}

# Basic completion - complete Tom commands
__tom_basic_complete_commands()
{
    # Nothing to do
    ! [[ ! $was_split && $cargs -eq 1 && $cur != -* ]] && return 1

    __tom_compgen -- "$tom_commands"
}

# Basic completion - complete long options
__tom_basic_complete_long_options()
{
    # Nothing to do
    ! ([[ ! $was_split && $cur == --* ]] && __tom_cargs_between 1 "$CARGS_MAX") && return 1

    __tom_compgen -n -- "$common_long_options"
}

# Basic completion - complete short options
__tom_basic_complete_short_options()
{
    # Nothing to do
    ! ([[ ! $was_split && $cur == -* ]] && __tom_cargs_between 1 "$CARGS_MAX") && return 1

    __tom_compgen -- "$common_short_options"
}

# Support functions
# ---

# Trim spaces in the current word ($cur)
__tom_trim_cur()
{
    cur=${cur##+([[:space:]])}
    cur=${cur%%+([[:space:]])}
}

# No completion after some options (--help, -h, --version, -V)
__tom_block_completion()
{
    # Get all words before the current word excluding the current word
    local -r cwords=$(__tom_get_words_before_cur)

    # Return 0 on success and 1 on failure (|) means end of the line, the same as |$) regex)
    [[ $cwords == *[[:space:]]@(--@(help|version)|-[hV])?([[:space:]]*|) ]]
}

# Count the number of positional arguments excluding options
__tom_count_args()
{
    local -i REPLY=0

    # Default is 1 (for tom.exe |), it only counts positional arguments before the current word
    # excluding the current word!
    # See NOTES.txt[bash completion]
    _comp_count_args

    # Should never happen, to avoid trillions 😁
    (( REPLY < 0 )) && REPLY=0

    printf '%u' "$REPLY"
}

# Don't append the space after the long option with a multi-/value (eg. --only=)
__tom_compopt_nospace()
{
    # Change to the [[:alnum:]] if there will be any option with a number in the option name,
    # currently I don't have any.
    # [[ $was_split ]] && # This check is not needed as the pattern below guarantees/implies this
    [[ ${COMPREPLY-} == --+([[:alpha:]])*(-+([[:alpha:]]))= ]] && compopt -o nospace
}

# Get all words before the current word excluding the current word
__tom_get_words_before_cur()
{
    local -a cwords

    for word in "${words[@]}"; do
        cwords+=("$word")
    done

    # Printing as one string separated by spaces is sufficient (quotes are important here)
    printf -- '%s' "${cwords[*]}"
}

# Determine whether the given value is between min/max (inclusive)
__tom_between()
{
    local -r value=$1
    local -r -i min=$2
    local -r -i max=$3

    (( value >= min && value <= max ))
}

# Determine whether the $cargs is between min/max (inclusive)
__tom_cargs_between()
{
    local -r -i min=$1
    local -r -i max=$2

    __tom_between cargs $min $max
}

# Set the $COMPREPLY using the compgen -W $1 -- $cur builtin command
#
# Options:
#     -n  Set compopt -o nospace
# @param $1 words  The wordlist for the compgen -W function (can't be empty)
#   The wordlist must be given as one string and will be split using the characters in the IFS
#   special variable as delimiters.
# @return  An exit status of the mapfile function, or 2 when the usage is wrong.
__tom_compgen()
{
    local -r funcname=${FUNCNAME[0]}
    local nospace=''

    # Handle options
    local flag='' OPTIND=1 OPTARG='' OPTERR=0
    while getopts 'n' flag "$@"; do
        case $flag in
            n) nospace='set' ;;
            *)
                printf 'tom: %s: usage error\n' "$funcname" >&2
                return 2
                ;;
        esac
    done
    # Patch arguments (remove already handled options)
    shift "$(( OPTIND - 1 ))"

    # Validate required positional arguments, there is no case when the WORDS are empty,
    # so they are required.
    if (( $# == 0 )); then
        printf 'tom: %s: unexpected number of arguments\n' "$funcname" >&2
        printf 'usage: %s [-n] [--] WORDS...' "$funcname" >&2
        return 2
    fi

    local -r words=$1
    local -i exit_code=1

    # Doesn't invoke the word splitting and glob expansion, this is the best I have found
    # after 12 hours of playing with this crazy shell. The other solution would be to use
    # shopt -s lastpipe, set +o monitor, and pipe the compgen to the mapfile, then restore
    # the shopt and set.
    # Also, I don't need advanced abstraction as I only need the -W option.
    # The same can be done using the _comp_compgen -- -W "$common_long_options", but
    # I abandoned it as there is a whole machinery inside and the solution below will be
    # faster.
    mapfile -t COMPREPLY < <(compgen -W "$words" -- "$cur")
    exit_code=$?

    # Don't append the space after the long option with a multi-/value
    [[ $nospace && $exit_code -eq 0 ]] && __tom_compopt_nospace

    # It CANNOT return the exit status of the compgen command because it returns 1 if
    # no matches were generated! Our code expects the return 0 in all cases.
    # If the completion condition was true then don't invoke the next completion function, if
    # the completion condition was false then return 1 in the PARENT function (which invoked
    # this function) and invoke the next completion function.
    # Edge case is when the mapfile returns 1, in this special case return 1 and the next
    # completion function will be invoked, that's OK. 😰
    return $exit_code
}

# Current Tom command
# ---

# Find the currently processed Tom command
__tom_command()
{
    local REPLY=''

    # Find the first positional argument (Tom command) excluding options and the current word!
    if _comp_get_first_arg; then
        printf '%s' "$REPLY"
        return 0
    fi

    # Return the current word under the cursor if it's not an option
    if [[ $cur != -* ]]; then
        printf '%s' "$cur"
        return 0
    fi

    return 1
}

# Try to guess Tom command with or without the namespace
# Determine if any command in the commands array ($1) starts with the given command name ($2).
# Matches can be ambiguous as it will return 0 (success) on the first match and it doesn't
# distinguish whether the result is ambiguous or not.
# See the exhaustive comment in the __tom_guess_command_with_namespace function.
__tom_guess_command()
{
    local -r command_needle=$1

    # Nothing to do, the command needle is empty
    if [[ ! $command_needle ]]; then
        return 1
    fi

    shift
    # Nothing to do, there are no positional arguments left
    (( $# )) || return 1
    local -r -a commands=("$@")
    local -i exit_code=1
    local command=''

    # Enable case-insensitive pattern matching
    local -r restore_nocasematch_cmd=$(__tom_enable_nocasematch)

    for command in "${commands[@]}"; do
        if __tom_guess_command_wrapper "$command_needle" "$command"; then
            exit_code=$?
            break
        fi
    done

    # Restore the original nocasematch value (contains a command with a previous value or
    # an empty string).
    $restore_nocasematch_cmd

    return $exit_code
}

# Try to guess one Tom command helper function (for nicer/simpler code logic)
__tom_guess_command_wrapper()
{
    local -r command_needle=$1
    local -r command=$2

    # Supports the : character in the command name (command with/in the namespace)
    if [[ $command == *:* ]]; then
        __tom_guess_command_with_namespace "$command_needle" "$command"
    else
        __tom_guess_command_without_namespace "$command_needle" "$command"
    fi

    return $?
}

# Try to guess one namespaced Tom command (eg. migrate:status)
__tom_guess_command_with_namespace()
{
    local -r command_needle=$1
    local -r command=$2

    local -a command_split
    local -a needle_split

    # Here we must be at 100% sure both contain the : character to split properly,
    # The __tom_guess_command_wrapper guarantees $command == *:* so no if statement check
    # before the following is needed.
    _comp_split -F : command_split "$command"
    _comp_split -F : needle_split "$command_needle"

    # Special weird case is db: in this case the needle_split[1] will be empty and
    # it will match because of [a-z]*$ aka *([a-z]) but it is desirable because we still
    # get --database=| connections completion triggered because of this. There are no
    # other cases where this can interfere. If this logic is not desirable, I will have to
    # change it, eg. add an ambiguous parameter to control this behavior, or even
    # better do the same as the complete:bash command and that's it to return this state
    # as kFound/kNotFound/kAmbiguous.
    # Also, $command_split[0] can never be empty, that's why the check above exists.
    # Don't user [[:lower:]] as it doesn't obey nocasematch=on, I want to have this
    # case-insensitive. Also, the __tom_enable_nocasematch is still needed because of that
    # needle_split[X] matching.

    # Return 0 on success and 1 on failure
    [[ ${command_split[0]} ]] &&
    [[ ${command_split[0]} == ${needle_split[0]}*([[:alpha:]]) ]] &&
    [[ ${command_split[1]} == ${needle_split[1]}*([[:alpha:]]) ]]
}

# Try to guess one Tom command in the global namespace (without the namespace eg. migrate)
__tom_guess_command_without_namespace()
{
    local -r command_needle=$1
    local -r command=$2

    # Return 0 on success and 1 on failure
    [[ $command_needle && $command == $command_needle*([[:alpha:]]) ]]
}

# Enable case-insensitive pattern matching (used by the __tom_guess_command)
__tom_enable_nocasematch()
{
    # Nothing to do, nocasematch is already enabled (returns an empty output '')
    shopt -q nocasematch && return 1

    # Obtain the current value as the callable command
    local -r restore_cmd=$(shopt -p nocasematch)

    shopt -s nocasematch # set

    printf '%s' "$restore_cmd"
}

# Complete paths
# ---

# Provide directory path completion for all commands with the --path= option
__tom_complete_dir()
{
    local -a allowed_commands
    read -ra allowed_commands < <(__tom_commands_with_path_option)

    # Nothing to do
    # All four commands defined in the __tom_commands_with_path_option function have
    # one optional positional argument, so $cargs must be between 2 and 3.
    ! ([[ $was_split && $prev == '--path' ]] && __tom_cargs_between 2 "$CARGS_MAX" &&
       __tom_guess_command "$tom_command" "${allowed_commands[@]}") &&
       return 1

    # Internally calls _comp_compgen_filedir -d
    _comp_compgen -a filedir -d
}

# Tom commands with the --path= option (used to complete directory paths)
__tom_commands_with_path_option()
{
    printf 'integrate make:migration make:model make:seeder'
}

# Complete functions
# ---

# Complete environment names for the --env option
__tom_complete_env_option()
{
    # Nothing to do
    ! ([[ $was_split && $prev == '--env' ]] && __tom_cargs_between 1 "$CARGS_MAX") && return 1

    __tom_compgen -- "$(__tom_environments)"
}

# Complete database connection names from the main.cpp file
__tom_complete_database_option()
{
    local -a allowed_commands
    read -ra allowed_commands < <(__tom_commands_with_database_option)

    # Nothing to do
    ! ([[ $was_split && $prev == '--database' ]] &&
       ([[ $cargs -eq 2 ]] && __tom_guess_command "$tom_command" "${allowed_commands[@]}" ||
        # This is a little overkill but it's exact 😎
        (__tom_cargs_between 2 "$CARGS_MAX" &&
         __tom_guess_command_with_namespace "$tom_command" 'db:seed'))) &&
       return 1

    __tom_compgen -- "$(__tom_connections)"
}

# Complete section names for the --only= option of the about command
__tom_complete_about_only_option()
{
    # Nothing to do
    ! ([[ $was_split && $tom_command && $prev == '--only' ]] && (( cargs == 2 )) &&
       __tom_guess_command_without_namespace "$tom_command" 'about') &&
       return 1

    __tom_compgen -- "$(__tom_about_sections)"
}

# Environment names for the --env= option (all our/known commands)
__tom_environments()
{
    printf 'dev development local prod production test testing staging'
}

# Tom commands with the --database= option (used to complete connection names)
__tom_commands_with_database_option()
{
    # Re-use %s for all strings, that's why the first two ends with the space
    printf '%s' \
        'migrate db:seed db:wipe ' \
        'migrate:fresh migrate:install migrate:refresh migrate:reset migrate:rollback ' \
        'migrate:status migrate:uninstall'
}

# Try to infer database connection names from the main.cpp file
# Lines containing connection names must be tagged with the '// shell:connection' comment.
__tom_connections()
{
    local -r main_file='main.cpp'

    # Nothing to do, the main.cpp file isn't readable or doesn't exist
    [[ -r "./$main_file" ]] || return 1

    # --quiet suppresses printing of the entire file and |p prints only the replaced part \1
    local -r regex='s|.*"([[:alnum:]_.-]+)".*// shell:connection$|\1|p'

    command sed --quiet --regexp-extended --expression="$regex" "$main_file"
}

# Section names for the --only= option of the about command
__tom_about_sections()
{
    printf 'environment macros versions connections'
}

# Main section related functions
# ---

# Inaccurate completion if the tom command is not on the system PATH (doesn't provide all options)
__tom_basic_completion()
{
    local tom_commands='' namespaces='' common_long_options='' common_short_options='' shells=''

    # Initialize variables for the inaccurate/basic completion
    __tom_basic_initialize

    # Complete positional arguments for Tom commands (integrate, help, list)
    __tom_basic_complete_positional_arguments && return 0

    # Complete Tom commands
    __tom_basic_complete_commands && return 0

    # Don't use a more specific matches below eg. -@(*([a-zA-Z])|+(v)), but it doesn't matter much
    # as these are some of the last commands invoked, so I will not describe the reasons why.

    # Complete long options
    __tom_basic_complete_long_options && return 0

    # Complete short options
    __tom_basic_complete_short_options && return 0

    return 1
}

# Accurate/enhanced completion using the tom complete:bash command
__tom_advanced_completion()
{
    local -a allowed_commands
    read -ra allowed_commands < <(__tom_advanced_commands_with_positional_arguments)

    # Nothing to do
       # Don't complete option values (eg. --only=|)
    ! ([[ ! $was_split ]] &&
        # Tom command or long/short option
       ([[ $cargs -eq 1 || $cur == -* ]] ||
         # Positional arguments after the known/our Tom command, currently, there is no command
         # with more than 3 positional arguments, so $cargs == 2 is OK.
        ([[ $cargs -eq 2 ]] && __tom_guess_command "$tom_command" "${allowed_commands[@]}"))) &&
        return 1

    # Complete positional arguments and long/short options
    local -r is_long_option=$([[ $cur == --* ]] && printf 'set')

    __tom_compgen ${is_long_option:+'-n'} -- \
        "$(command tom complete:bash --commandline="${words[*]}" --word="$cur" --cargs="$cargs")"
}

# Basic completion - Tom commands with positional arguments to complete
__tom_advanced_commands_with_positional_arguments()
{
    printf 'help list integrate'
}

# Invoke the accurate/enhanced or inaccurate/basic completion
# Complete both positional arguments and long/short options.
__tom_complete()
{
    # Don't use A && B || C to avoid SC2015

    # Accurate/enhanced - completion using the tom complete:bash command
    if _comp_have_command tom; then
        __tom_advanced_completion

    # Inaccurate completion - tom command isn't on the PATH (doesn't provide all options)
    else
        __tom_basic_completion
    fi

    return $?
}

# Main section
# ---

# Main tab-completion logic for the tom command
_comp_cmd_tom()
{
    # Don't set local -i for cword as _comp_initialize internally calls unset -v cword anyway.
    # cword is 0-based (tom.exe is at position 0).
    # The was_split indicates that the current word is the long option with a multi-/value
    # delimited with the = character, which means it was split. The -s option (that implies -n =)
    # for the _comp_initialize function controls this behavior.
    # The comp_args contains the word preceding the word being completed
    # on the current command line, but it's the original word passed to this function
    # by the complete -F _comp_cmd_tom tom tom_testdata function call all the way down.
    # It's practically the same as the $prev variable but the $prev variable is already processed
    # by the _comp_initialize function ($3 == comp_args == $prev), which means it can be different
    # in some cases, but I don't remember when or how it's different.
    # I have removed the following variables as I don't need them: cword comp_args
    local cur='' prev='' was_split=''
    local -a words

    # -n removes characters from list of word completion separators ($COMP_WORDBREAKS) but
    # it doesn't actually remove those characters from the $COMP_WORDBREAKS global variable,
    # instead, it recomputes $words using: _comp__reassemble_words "$1" words cword
    # -s implies -n = but that's not enough, the : must also be removed eg. for tom make:m|
    # For the : character to work properly with the set menu-complete-display-prefix on,
    # export COMP_WORDBREAKS=${COMP_WORDBREAKS//:} must also be added to ~/.bashrc. 🤔
    _comp_initialize -s -n : -- "$@" || return 0

    # Trim spaces in the current word ($cur) because bash completion takes eg. tom   | --ansi
    # as $cur == '  ' and even the tom complete:bash returns the correct result eg. abc\nxyz
    # it doesn't match because bash internally also tries to match the $cur against abc\nxyz
    # result, it would only match if all words would begin with the same amount of spaces. 🤔
    __tom_trim_cur

    # Don't sort the completion output for both in/accurate completions, the completion output
    # is already sorted for the accurate completion by the complete:bash command and I want to
    # have the output unsorted for the inaccurate completion because the sort order is defined
    # during variables initialization.
    compopt -o nosort

    # Prevent ^M in the complete output on MSYS2/MinGW-w64
    if [[ $MSYSTEM ]]; then
        local IFS=$' \t\n\r'
    fi

    # No completion after some options (--help, -h, --version, -V)
    __tom_block_completion && return 0

    # Count the number of positional arguments excluding options
    # Default is 1 (for tom.exe |), it only counts positional arguments before the current word
    # excluding the current word!
    # See NOTES.txt[bash completion]
    local -r -i cargs=$(__tom_count_args)
    # Maximum number of positional arguments among all commands
    local -r -i CARGS_MAX=3

    # Find the currently processed Tom command
    local -r tom_command=$(__tom_command)

    # Complete environment names for the --env option
    __tom_complete_env_option && return 0

    # Complete database connection names from the main.cpp file
    __tom_complete_database_option && return 0

    # Complete section names for the --only= option of the about command
    __tom_complete_about_only_option && return 0

    # Invoke the accurate/enhanced or inaccurate/basic completion (based if tom is on $PATH)
    __tom_complete && return 0

    # Provide directory path completion for all commands with the --path= option
    __tom_complete_dir

    # Bash man pages tells nothing about return value from the complete function
} &&
    complete -F _comp_cmd_tom tom tom_testdata

# ex: filetype=sh
