# Helper function for coupling option() and add_feature_info()
function(feature_option name description default)

    string(CONCAT desc "${description} (default: ${default})")

    option(${name} "${desc}" "${default}")

    add_feature_info(${name} ${name} "${desc}")

endfunction()

# Helper function for coupling option() and add_feature_info() and use the default value
# from the given environment variable if defined, otherwise, use a value from the given
# default CMake variable. The 'default' CMake variable must be of the boolean type and
# can't be empty!
function(feature_option_environment name description environment_variable_name default)

    set(defaultValue "")

    tiny_get_default_value_from_environment(
        defaultValue ${environment_variable_name} ${default}
    )

    feature_option(${name} "${description}" ${defaultValue})

endfunction()

include(CMakeDependentOption)
# Helper function for coupling cmake_dependent_option() and add_feature_info()
macro(feature_option_dependent name description default depends force)

    string(CONCAT desc
        "${description} (default: ${default}; depends on condition: ${depends})")

    cmake_dependent_option(${name} "${desc}" "${default}" "${depends}" "${force}")

    add_feature_info(${name} ${name} "${desc}")

endmacro()

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

    set(allowedValues "")

    string(JOIN ", " allowedValues ${strings})
    string(CONCAT desc
        "${description} (allowed values: ${allowedValues}; default: ${default}; \
depends on condition: ${depends})")

    tiny_dependent_string_option(
        ${name} "${strings}" "${desc}" "${default}" "${depends}" "${force}"
    )

    add_feature_info(${name} ${name} "${desc}")

    unset(desc)
    unset(allowedValues)

endmacro()

# Add a simple build option which controls compile definition(s) for a target.
#
# Synopsis:
# target_optional_compile_definitions(<target> <scope> [ADVANCED] [FEATURE]
#   NAME <name> DESCRIPTION <description> DEFAULT <default_value>
#   DEFAULT_FROM_ENVIRONMENT <environment_variable_name>
#   [ENABLED [enabled_compile_definitions...]]
#   [DISABLED [disabled_compile_definitions...]]
# )
#
# NAME, DESCRIPTION and DEFAULT are passed to option() command.
# If FEATURE is given, they are also passed to add_feature_info() command.
# ADVANCED calls the mark_as_advanced(<NAME>) command.
# <scope> determines the scope for the following compile definitions.
# ENABLED lists compile definitions that will be set on <target> when option is enabled.
# DISABLED lists compile definitions that will be set on <target> when option is disabled.
# ENABLED or DISABLE are passed to the target_compile_definitions() command.
# DEFAULT_FROM_ENVIRONMENT get a default value for the option() command from the given
# environment variable if it's defined otherwise use a value from the given <DEFAULT>
# argument.
# DEFAULT and DEFAULT_FROM_ENVIRONMENT can't be if they are passed and they must be
# of the boolean type.
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

    if("DEFAULT" IN_LIST TINY_KEYWORDS_MISSING_VALUES OR
            "DEFAULT_FROM_ENVIRONMENT" IN_LIST TINY_KEYWORDS_MISSING_VALUES
    )
        message(FATAL_ERROR "The ${CMAKE_CURRENT_FUNCTION}() is missing values for \
arguments: ${TINY_KEYWORDS_MISSING_VALUES}")
    endif()

    # Body
    # If an environment variable is defined then use its value otherwise
    # use a value from the 'TINY_DEFAULT' CMake variable
    set(defaultValue OFF)
    # This macro sets the defaultValue value
    tiny_get_default_value_from_environment_wrapper(
        TINY_DEFAULT_FROM_ENVIRONMENT ${TINY_DEFAULT}
    )

    string(CONCAT description "${TINY_DESCRIPTION} (default: ${defaultValue})")

    option(${TINY_NAME} "${description}" ${defaultValue}) # No need to quote the defaultValue as it can't be empty, is unquoted everywhere

    # No need to check for empty TINY_ENABLED/DISABLED values
    # for target_compile_definitions() because it ignores empty values
    if(${${TINY_NAME}}) # Quotes not needed, don't care about lists for now
        target_compile_definitions(${target} ${scope} ${TINY_ENABLED}) # Don't quote TINY_ENABLED (it works also quoted for list values)
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

# Get a default value from the given environment variable if defined otherwise return
# a value from the given 'default' CMake variable. Used by our option() helper functions
# to set their default values ​​from an environment variable.
# The 'default' CMake variable must be of the boolean type and can't be empty!
function(tiny_get_default_value_from_environment out_variable name default)

    # If an environment variable is defined then use its value
    if(DEFINED ENV{${name}})
        if("$ENV{${name}}")
            set(defaultValue ON)
        else()
            set(defaultValue OFF)
        endif()

    # Otherwise, use a value from the 'default' CMake variable
    else()
        set(defaultValue ${default})
    endif()

    set(${out_variable} ${defaultValue} PARENT_SCOPE)

endfunction()

# Helper macro() for the target_optional_compile_definitions() for nice and terser code.
# It must be a macro() because of if(DEFINED).
# The 'default' CMake variable must be of the boolean type and can't be empty!
macro(tiny_get_default_value_from_environment_wrapper name default)

    # If an environment variable is defined then use its value
    if(DEFINED ${name})
        set(defaultValue "")

        tiny_get_default_value_from_environment(defaultValue ${${name}} ${default})

    # Otherwise, use a value from the 'TINY_DEFAULT' CMake variable
    else()
        set(defaultValue ${TINY_DEFAULT})
    endif()

endmacro()
