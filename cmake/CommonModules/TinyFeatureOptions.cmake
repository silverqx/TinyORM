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
