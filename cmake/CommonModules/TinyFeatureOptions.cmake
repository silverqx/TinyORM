# Helper function for coupling option() and add_feature_info()
function(feature_option name description default)

    string(CONCAT desc "${description} (default: ${default})")

    option(${name} "${desc}" "${default}")

    add_feature_info(${name} ${name} "${desc}")

endfunction()

# Helper function for coupling option() and add_feature_info() and use the default value
# from the given environment variable if defined, otherwise, use a value from the given
# default CMake variable
function(feature_option_environment name description environment_variable_name default)

    # If an environment variable is defined, then use its value
    if(DEFINED ENV{${environment_variable_name}})
        if("$ENV{${environment_variable_name}}")
            set(defaultValue ON)
        else()
            set(defaultValue OFF)
        endif()

    # Otherwise, use a value from the default CMake variable
    else()
        set(defaultValue "${default}")
    endif()

    feature_option(${name} "${description}" "${defaultValue}")

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

    set(allowedValues)
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
