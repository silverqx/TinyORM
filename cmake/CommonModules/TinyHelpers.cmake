# Invert a boolean variable value
function(tiny_invert_bool variable value)

    if(${value})
        set(${variable} FALSE PARENT_SCOPE)
    else()
        set(${variable} TRUE PARENT_SCOPE)
    endif()

endfunction()

# Make minimum toolchain version a requirement
function(tiny_toolchain_requirement)

    set(singleargs MSVC)
    cmake_parse_arguments(PARSE_ARGV 0 TINY "" "${singleargs}" "")

    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS TINY_MSVC)
            message(FATAL_ERROR "Minimum required MSVC version was not satisfied, \
required version >=${TINY_MSVC}, your version is ${CMAKE_CXX_COMPILER_VERSION}, upgrade \
Visual Studio")
        endif()
    endif()

endfunction()

# A helper macro that calls find_package() and appends the package (if found) to the
# TINY_PACKAGE_DEPENDENCIES list, which can be used later to generate package config file
macro(tiny_find_package package_name)

    find_package(${package_name} ${ARGN})

    if(${package_name}_FOUND)
        set(args "${package_name}")
        # These arguments will be forwarded to the find_package() by find_dependency()
        list(APPEND args "${ARGN}")
        # REQUIRED and QUIETLY arguments are handled by find_dependency() macro
        # find_dependency() forwards the correct parameters for QUIET and REQUIRED which
        # were passed to the original find_package() call
        list(REMOVE_ITEM args "REQUIRED" "QUIET")
        # Remove all empty items
        list(REMOVE_ITEM args "")
        # Convert to the string
        string(REPLACE ";" " " args "${args}")

        list(APPEND tiny_package_dependencies "${args}")
    endif()

endmacro()

# Generate find_dependency calls for the TinyORM package config file
function(tiny_generate_find_dependency_calls)

    set(find_dependency_calls)

    string(REGEX REPLACE "([^;]+)" "find_dependency(\\1)" find_dependency_calls
        "${tiny_package_dependencies}")

    string(REPLACE ";" "\n" find_dependency_calls "${find_dependency_calls}")

    set(tiny_find_dependency_calls ${find_dependency_calls} PARENT_SCOPE)

endfunction()

# Add a simple build option which controls compile definition(s) for a target.
#
# Synopsis:
# target_optional_compile_definitions(<target> <scope> [FEATURE]
#   NAME <name> DESCRIPTION <description> DEFAULT <default_value>
#   [ENABLED [enabled_compile_definitions...]]
#   [DISABLED [disabled_compile_defnitions...]]
# )
#
# NAME, DESCRIPTION and DEFAULT are passed to option() command.
# If FEATURE is given, they are also passed to add_feature_info() command.
# <scope> determines the scope for the following compile definitions.
# ENABLED lists compile definitions that will be set on <target> when option is enabled,
# DISABLED lists definitions that will be set otherwise.
function(target_optional_compile_definitions target scope)

    set(options FEATURE)
    set(oneValueArgs NAME DESCRIPTION DEFAULT)
    set(multiValueArgs ENABLED DISABLED)
    cmake_parse_arguments(TINY ${options} "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    option(${TINY_NAME} "${TINY_DESCRIPTION}" ${TINY_DEFAULT})

    if(${${TINY_NAME}})
        target_compile_definitions(${target} ${scope} ${TINY_ENABLED})
    else()
        target_compile_definitions(${target} ${scope} ${TINY_DISABLED})
    endif()

    if(${TINY_FEATURE})
        add_feature_info(${TINY_NAME} ${TINY_NAME} "${TINY_DESCRIPTION}")
    endif()

endfunction()

# Create an empty SQLite database file when does not exist
function(create_sqlite_db db_filepath)

    if(EXISTS ${db_filepath})
        return()
    endif()

    message(STATUS "Creating SQLite database at '${db_filepath}'")

    file(TOUCH "${db_filepath}")

endfunction()
