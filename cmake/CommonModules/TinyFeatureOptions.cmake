include_guard(GLOBAL)

include(FeatureSummary) # For add_feature_info()

# This is the only file that contains our functions and macros without the tiny_ prefix,
# reason for this is to have nicer syntax as these functions are heavily used.

# Helper function for coupling option() and add_feature_info()
function(feature_option name description default)

    # Arguments checks
    # Required value/s
    if("${description}" STREQUAL "" OR "${default}" STREQUAL "")
        message(FATAL_ERROR "The 'description' and 'default' arguments cannot be empty \
in ${CMAKE_CURRENT_FUNCTION}().")
    endif()

    # Body
    string(CONCAT _description "${description} (default: ${default})")

    option(${name} "${_description}" "${default}") # Revisited, the quoted default value is correct in all cases, even if empty

    add_feature_info(${name} ${name} "${_description}")

endfunction()

# Helper function for coupling option() and add_feature_info() and use the default value
# from the given environment variable if defined, otherwise, use a value from the given
# 'default' argument. The 'default' argument must be of the boolean type and
# can't be empty!
function(feature_option_environment name description environment_variable_name default)

    # Arguments checks
    # Required value/s
    if("${default}" STREQUAL "")
        message(FATAL_ERROR "The 'default' argument cannot be empty \
in ${CMAKE_CURRENT_FUNCTION}().")
    endif()

    # Body
    # Don't early exit here if the ${name} is already defined, obey the default option()
    # command behavior.

    # If an environment variable is defined then use its value otherwise use
    # the <default>. This function sets the defaultValue value.
    tiny_get_boolean_value_from_environment(
        defaultValue ${environment_variable_name} ${default} # Revisited, unquoted is OK, default must be of the boolean type
    )

    feature_option(${name} "${description}" ${defaultValue})

endfunction()

include(CMakeDependentOption)
# Helper function for coupling cmake_dependent_option() and add_feature_info()
function(feature_option_dependent name description default depends force)

    # Arguments checks
    # Required value/s
    if("${description}" STREQUAL "" OR "${default}" STREQUAL "")
        message(FATAL_ERROR "The 'description' and 'default' arguments cannot be empty \
in ${CMAKE_CURRENT_FUNCTION}().")
    endif()

    # Body
    string(CONCAT _description
        "${description} (default: ${default}; depends on condition: ${depends})")

    cmake_dependent_option(${name} "${_description}" "${default}" "${depends}" "${force}")

    add_feature_info(${name} ${name} "${_description}")

endfunction()

# Macro to provide a STRING type option dependent on other options.
# This macro works the same and as the cmake_dependent_option() but it's for the STRING
# type options instead of BOOL option-s.
# It also sets the set_property(CACHE <option> PROPERTY STRINGS <strings>) to populate
# drop-down combo box for CMake GUI-s.
macro(tiny_dependent_string_option option strings doc default depends force)

    set(${option}_AVAILABLE YES)

    # Determine whether the given option should be provided and visible (using the full
    # Condition Syntax (CMP0127 implementation))
    foreach(depend ${depends}) # Don't use ITEMS keyword
        # Don't use the if(NOT ${depend}) without else() block here, the ${depend} can
        # contain complex condition and it can break meaning of this condition
        cmake_language(EVAL CODE "
            if(${depend})
            else()
                set(${option}_AVAILABLE NO)
            endif()"
        )
    endforeach()

    if(${option}_AVAILABLE) # Quotes not needed, will be _AVAILABLE at least, so FALSE on empty/undefined ${option}
        # Restore the previous option value from the INTERNAL cache variable saved earlier
        if(DEFINED CACHE{${option}})
            set(${option} "${${option}}" CACHE STRING "${doc}" FORCE)

        # Use the given <default> value if there is no INTERNAL cache variable defined
        else()
            set(${option} "${default}" CACHE STRING "${doc}" FORCE)
        endif()

        # Populate drop-down combo box for CMake GUI-s
        set_property(CACHE ${option} PROPERTY STRINGS ${strings})

    else()
        # Save the current option value to restore it later (if defined) and
        # hide the option from a user using the INTERNAL cache variable
        if(DEFINED CACHE{${option}})
            set(${option} "${${option}}" CACHE INTERNAL "${doc}")
        endif()

        # Set option value to the <force> value until the option is hidden
        set(${option} "${force}")
    endif()

    unset(${option}_AVAILABLE)

endmacro()

# Helper function for coupling tiny_dependent_string_option() and add_feature_info()
macro(feature_string_option_dependent name strings description default depends force)

    # Arguments checks
    # Required value/s
    if("${description}" STREQUAL "" OR "${default}" STREQUAL "")
        message(FATAL_ERROR "The 'description' and 'default' arguments cannot be empty \
in ${CMAKE_CURRENT_FUNCTION}().")
    endif()

    # Body
    set(allowedValues "")

    string(JOIN ", " allowedValues ${strings})
    string(CONCAT _description
        "${description} (allowed values: ${allowedValues}; default: ${default}; \
depends on condition: ${depends})")

    tiny_dependent_string_option(
        ${name} "${strings}" "${_description}" "${default}" "${depends}" "${force}"
    )

    add_feature_info(${name} ${name} "${_description}")

    unset(_description)
    unset(allowedValues)

endmacro()

# Add the boolean build option and compile definition/s for a target in one shot.
#
# Synopsis:
# target_optional_compile_definitions(<target> <scope> [ADVANCED] [FEATURE]
#   NAME <name> DESCRIPTION <description> DEFAULT <default-value>
#   [DEFAULT_FROM_ENVIRONMENT <environment-variable-name>]
#   [ENABLED [<enabled-compile-definitions>...]]
#   [DISABLED [<disabled-compile-definitions>...]]
# )
#
# <target> name to operate on.
# <scope> for the target_compile_definitions() command.
# NAME of a variable to process, it's directly passed to the option() and
# add_feature_info() commands.
# DESCRIPTION for the option() and add_feature_info() commands.
# DEFAULT initial value for the option() command, used if the <environment-variable-name>
# is empty or undefined.
# ADVANCED call the mark_as_advanced(<name>) command.
# FEATURE pass the <name> and <description> values also to the add_feature_info() command.
# ENABLED <enabled-compile-definitions> to set on the <target> when option is enabled.
# DISABLED <disabled-compile-definitions> to set on the <target> when option is disabled.
# ENABLED and DISABLE are passed directly to target_compile_definitions(<scope>) command.
# DEFAULT_FROM_ENVIRONMENT get a default value for the option() command from the given
# <environment-variable-name> if defined, otherwise use the <default-value>.
# DEFAULT and DEFAULT_FROM_ENVIRONMENT can't be empty if they are passed and they must be
# of the boolean type.
#
# Call the option() command if an option <name> is not defined yet and initialize its
# default value from the <environment-variable-name> if given, otherwise
# from the <default-value>. Call the target_compile_definitions(<scope>)
# with the <enabled-compile-definitions> if an option is enabled (ON), otherwise use
# the <disabled-compile-definitions>. Call the add_feature_info() and mark_as_advanced()
# functions if FEATURE or ADVANCED is given.
function(target_optional_compile_definitions target scope)

    # Arguments
    set(options ADVANCED FEATURE)
    set(oneValueArgs NAME DESCRIPTION DEFAULT DEFAULT_FROM_ENVIRONMENT)
    set(multiValueArgs ENABLED DISABLED)
    cmake_parse_arguments(PARSE_ARGV 2 TINY
        "${options}" "${oneValueArgs}" "${multiValueArgs}"
    )

    # Arguments checks
    if(DEFINED TINY_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "The ${CMAKE_CURRENT_FUNCTION}() was passed extra arguments: \
${TINY_UNPARSED_ARGUMENTS}")
    endif()

    # Cannot be empty if defined
    if("DEFAULT_FROM_ENVIRONMENT" IN_LIST TINY_KEYWORDS_MISSING_VALUES OR
            # May be it only correctly works if CMake >=3.31 (CMP0174)?
            # Doesn't matter, not a big deal (DEFAULT_FROM_ENVIRONMENT "").
            (DEFINED TINY_DEFAULT_FROM_ENVIRONMENT AND
                "${TINY_DEFAULT_FROM_ENVIRONMENT}" STREQUAL "")
    )
        message(FATAL_ERROR "The DEFAULT_FROM_ENVIRONMENT keyword argument is missing \
value or its value is empty in ${CMAKE_CURRENT_FUNCTION}().")
    endif()

    # Required value/s
    if("${TINY_DEFAULT}" STREQUAL "" OR "${TINY_DESCRIPTION}" STREQUAL "" OR
            "${TINY_NAME}" STREQUAL ""
    )
        message(FATAL_ERROR "The ${CMAKE_CURRENT_FUNCTION}() is missing single-valued \
keyword or its value is empty: DEFAULT, DESCRIPTION, NAME")
    endif()

    # Body
    # Convert to the boolean value (ON/OFF)
    tiny_to_bool(TINY_DEFAULT ${TINY_DEFAULT} FORMAT "OO") # Don't quote, must fail if undefined
    # If an environment variable is defined then use its value otherwise use
    # the <default-value>. This function sets the defaultValue value.
    tiny_get_boolean_value_from_environment(defaultValue
        "${TINY_DEFAULT_FROM_ENVIRONMENT}" ${TINY_DEFAULT} # Revisited, quotes needed for TINY_DEFAULT_FROM_ENVIRONMENT, unquoted TINY_DEFAULT is OK
    )

    string(CONCAT description "${TINY_DESCRIPTION} (default: ${defaultValue})")

    option(${TINY_NAME} "${description}" ${defaultValue}) # No need to quote the defaultValue as it can't be empty, is unquoted everywhere

    # No need to check for empty TINY_ENABLED/DISABLED values
    # for target_compile_definitions() because it ignores empty values
    if(${${TINY_NAME}}) # Quotes not needed (OT returns FALSE for variable name with ; character)
        target_compile_definitions(${target} ${scope} ${TINY_ENABLED}) # Don't quote TINY_ENABLED (even if it also works for quoted list values)
    else()
        target_compile_definitions(${target} ${scope} ${TINY_DISABLED})
    endif()

    if(TINY_FEATURE)
        add_feature_info(${TINY_NAME} ${TINY_NAME} "${description}")
    endif()

    if(TINY_ADVANCED)
        mark_as_advanced(${TINY_NAME})
    endif()

endfunction()
