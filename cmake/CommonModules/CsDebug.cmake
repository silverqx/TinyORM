# Print various internal CMake variables
function(cs_system_info)
    include(CMakePrintSystemInformation)
endfunction()

# Enable verbose output from Makefile builds
function(cs_verbose_makefile)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endfunction()

# Print all CMake variables (excluding all CMake variables by default)
# cs_print_vars() == cs_print_vars(yes)
function(cs_print_vars)
    set(exclude_cmake yes) # Exclude CMake by default
    if(ARGC GREATER_EQUAL 1 AND DEFINED ARGV0 AND NOT ARGV0)
        set(exclude_cmake no)
        message(STATUS "All variables:")
    else()
        message(STATUS "All non-cmake variables:")
    endif()

    get_cmake_property(variable_names VARIABLES)
    foreach (variable ${variable_names})
        if(exclude_cmake AND variable MATCHES "(^(CMAKE_.*)|^(_.*))")
            continue()
        endif()

        message("${variable}=${${variable}}")
    endforeach()
endfunction()

# Print all env. variables
function(cs_print_env_vars)
    message(STATUS "All environment variables:")
    execute_process(COMMAND "${CMAKE_COMMAND}" "-E" "environment")
endfunction()

# Get all properties that CMake supports
if(NOT CMAKE_PROPERTY_LIST)
    execute_process(
        COMMAND cmake --help-property-list
        OUTPUT_VARIABLE CMAKE_PROPERTY_LIST
    )

    # Convert command output into a CMake list
    string(REGEX REPLACE ";" "\\\\;" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
    string(REGEX REPLACE "\n" ";" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
endif()

# Print all target properties
function(cs_print_target_properties target)

    if(NOT TARGET ${target}) # Quotes not needed, the target cannot be a list and can't contain spaces
        message(FATAL_ERROR "There is no target named: ${target}")
    endif()

    message(STATUS "Target properties for '${target}':")

    # Don't use the TINY_BUILD_TYPE_UPPER here as this function can be used in contexts
    # where the TINY_BUILD_TYPE_UPPER isn't available
    string(TOUPPER "${CMAKE_BUILD_TYPE}" cmakeBuildTypeUpper)

    foreach(property ${CMAKE_PROPERTY_LIST})
        string(REPLACE "<CONFIG>" "${cmakeBuildTypeUpper}" property ${property})

        if(property STREQUAL "LOCATION" OR property MATCHES "^LOCATION_" OR
                property MATCHES "_LOCATION$"
        )
            continue()
        endif()

        get_property(isTargetSet TARGET ${target} PROPERTY ${property} SET)

        if(isTargetSet)
            get_target_property(value ${target} ${property})
            message("${property} = ${value}")
        endif()
    endforeach()

endfunction()

# Print all source file properties
function(cs_print_source_properties source)

    if(NOT EXISTS ${source}) # Quotes not needed, the target cannot be a list (it fails on the list which is good, it quotes it doesn't fail with the list) and spaces are handled correctly without quotes
        message(FATAL_ERROR "There is no source file named: ${source}")
    endif()

    message(STATUS "Source file properties for ${source}:")

    # Don't use the TINY_BUILD_TYPE_UPPER here as this function can be used in contexts
    # where the TINY_BUILD_TYPE_UPPER isn't available
    string(TOUPPER "${CMAKE_BUILD_TYPE}" cmakeBuildTypeUpper)

    foreach(property ${CMAKE_PROPERTY_LIST})
        string(REPLACE "<CONFIG>" "${cmakeBuildTypeUpper}" property ${property})

        if(property STREQUAL "LOCATION" OR property MATCHES "^LOCATION_" OR
                property MATCHES "_LOCATION$"
        )
            continue()
        endif()

        get_property(isTargetSet SOURCE "${source}" PROPERTY ${property} SET)

        if(isTargetSet)
            get_source_file_property(value "${source}" ${property})
            message("${property} = ${value}")
        endif()
    endforeach()

endfunction()

# Print clearly visible notice message about passed variable
function(p variable)
    message("|||-- ${variable} : ${${variable}}")
endfunction()

# Print status message about a passed variable
function(ps variable)
    message(STATUS "${variable} : ${${variable}}")
endfunction()

# Print notice message about a passed variable
function(pn variable)
    message(NOTICE "${variable} : ${${variable}}")
endfunction()

# Print clearly visible notice message about passed BOOL variable
function(pb variable)
    if(NOT DEFINED ${variable}) # Quotes not needed
        message("|||-- ${variable} : ${variable}-NOTFOUND")
    elseif(${variable}) # Quotes not needed because of the DEFINED check above and don't care about lists and strings
        message("|||-- ${variable} : ON")
    else()
        message("|||-- ${variable} : OFF")
    endif()
endfunction()
